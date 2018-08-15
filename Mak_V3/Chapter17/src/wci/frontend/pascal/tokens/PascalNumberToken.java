package wci.frontend.pascal.tokens;

import wci.frontend.*;
import wci.frontend.pascal.*;

import static wci.frontend.pascal.PascalTokenType.*;
import static wci.frontend.pascal.PascalErrorCode.*;

/**
 * <h1>PascalNumberToken</h1>
 *
 * <p>Pascal number tokens (integer and real).</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class PascalNumberToken extends PascalToken
{
    private static final int MAX_EXPONENT = 37;

    /**
     * Constructor.
     * @param source the source from where to fetch the token's characters.
     * @throws Exception if an error occurred.
     */
    public PascalNumberToken(Source source)
        throws Exception
    {
        super(source);
    }

    /**
     * Extract a Pascal number token from the source.
     * @throws Exception if an error occurred.
     */
    protected void extract()
        throws Exception
    {
        StringBuilder textBuffer = new StringBuilder();  // token's characters
        extractNumber(textBuffer);
        text = textBuffer.toString();
    }

    /**
     * Extract a Pascal number token from the source.
     * @param textBuffer the buffer to append the token's characters.
     * @throws Exception if an error occurred.
     */
    protected void extractNumber(StringBuilder textBuffer)
        throws Exception
    {
        String wholeDigits = null;     // digits before the decimal point
        String fractionDigits = null;  // digits after the decimal point
        String exponentDigits = null;  // exponent digits
        char exponentSign = '+';       // exponent sign '+' or '-'
        boolean sawDotDot = false;     // true if saw .. token
        char currentChar;              // current character

        type = INTEGER;  // assume INTEGER token type for now

        // Extract the digits of the whole part of the number.
        wholeDigits = unsignedIntegerDigits(textBuffer);
        if (type == ERROR) {
            return;
        }

        // Is there a . ?
        // It could be a decimal point or the start of a .. token.
        currentChar = currentChar();
        if (currentChar == '.') {
            if (peekChar() == '.') {
                sawDotDot = true;  // it's a ".." token, so don't consume it
            }
            else {
                type = REAL;  // decimal point, so token type is REAL
                textBuffer.append(currentChar);
                currentChar = nextChar();  // consume decimal point

                // Collect the digits of the fraction part of the number.
                fractionDigits = unsignedIntegerDigits(textBuffer);
                if (type == ERROR) {
                    return;
                }
            }
        }

        // Is there an exponent part?
        // There cannot be an exponent if we already saw a ".." token.
        currentChar = currentChar();
        if (!sawDotDot && ((currentChar == 'E') || (currentChar == 'e'))) {
            type = REAL;  // exponent, so token type is REAL
            textBuffer.append(currentChar);
            currentChar = nextChar();  // consume 'E' or 'e'

            // Exponent sign?
            if ((currentChar == '+') || (currentChar == '-')) {
                textBuffer.append(currentChar);
                exponentSign = currentChar;
                currentChar = nextChar();  // consume '+' or '-'
            }

            // Extract the digits of the exponent.
            exponentDigits = unsignedIntegerDigits(textBuffer);
        }

        // Compute the value of an integer number token.
        if (type == INTEGER) {
            int integerValue = computeIntegerValue(wholeDigits);

            if (type != ERROR) {
                value = new Integer(integerValue);
            }
        }

        // Compute the value of a real number token.
        else if (type == REAL) {
            float floatValue = computeFloatValue(wholeDigits, fractionDigits,
                                                 exponentDigits, exponentSign);

            if (type != ERROR) {
                value = new Float(floatValue);
            }
        }
    }

    /**
     * Extract and return the digits of an unsigned integer.
     * @param textBuffer the buffer to append the token's characters.
     * @return the string of digits.
     * @throws Exception if an error occurred.
     */
    private String unsignedIntegerDigits(StringBuilder textBuffer)
        throws Exception
    {
        char currentChar = currentChar();

        // Must have at least one digit.
        if (!Character.isDigit(currentChar)) {
            type = ERROR;
            value = INVALID_NUMBER;
            return null;
        }

        // Extract the digits.
        StringBuilder digits = new StringBuilder();
        while (Character.isDigit(currentChar)) {
            textBuffer.append(currentChar);
            digits.append(currentChar);
            currentChar = nextChar();  // consume digit
        }

        return digits.toString();
    }

    /**
     * Compute and return the integer value of a string of digits.
     * Check for overflow.
     * @param digits the string of digits.
     * @return the integer value.
     */
    private int computeIntegerValue(String digits)
    {
        // Return 0 if no digits.
        if (digits == null) {
            return 0;
        }

        int integerValue = 0;
        int prevValue = -1;    // overflow occurred if prevValue > integerValue
        int index = 0;

        // Loop over the digits to compute the integer value
        // as long as there is no overflow.
        while ((index < digits.length()) && (integerValue >= prevValue)) {
            prevValue = integerValue;
            integerValue = 10*integerValue +
                           Character.getNumericValue(digits.charAt(index++));
        }

        // No overflow:  Return the integer value.
        if (integerValue >= prevValue) {
            return integerValue;
        }

        // Overflow:  Set the integer out of range error.
        else {
            type = ERROR;
            value = RANGE_INTEGER;
            return 0;
        }
    }

    /**
     * Compute and return the float value of a real number.
     * @param wholeDigits the string of digits before the decimal point.
     * @param fractionDigits the string of digits after the decimal point.
     * @param exponentDigits the string of exponent digits.
     * @param exponentSign the exponent sign.
     * @return the float value.
     */
    private float computeFloatValue(String wholeDigits, String fractionDigits,
                                    String exponentDigits, char exponentSign)
    {
        double floatValue = 0.0;
        int exponentValue = computeIntegerValue(exponentDigits);
        String digits = wholeDigits;  // whole and fraction digits

        // Negate the exponent if the exponent sign is '-'.
        if (exponentSign == '-') {
            exponentValue = -exponentValue;
        }

        // If there are any fraction digits, adjust the exponent value
        // and append the fraction digits.
        if (fractionDigits != null) {
            exponentValue -= fractionDigits.length();
            digits += fractionDigits;
        }

        // Check for a real number out of range error.
        if (Math.abs(exponentValue + wholeDigits.length()) > MAX_EXPONENT) {
            type = ERROR;
            value = RANGE_REAL;
            return 0.0f;
        }

        // Loop over the digits to compute the float value.
        int index = 0;
        while (index < digits.length()) {
            floatValue = 10*floatValue +
                         Character.getNumericValue(digits.charAt(index++));
        }

        // Adjust the float value based on the exponent value.
        if (exponentValue != 0) {
            floatValue *= Math.pow(10, exponentValue);
        }

        return (float) floatValue;
    }
}
