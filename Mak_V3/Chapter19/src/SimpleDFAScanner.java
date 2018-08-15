import java.io.*;

/**
 * <h1>SimpleDFAScanner</h1>
 *
 * <p>A simple DFA scanner that recognizes Pascal identifers and numbers.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class SimpleDFAScanner
{
    // Input characters.
    private static final int LETTER = 0;
    private static final int DIGIT  = 1;
    private static final int PLUS   = 2;
    private static final int MINUS  = 3;
    private static final int DOT    = 4;
    private static final int E      = 5;
    private static final int OTHER  = 6;

    // Error state.
    private static final int ERR = -99999;

    // State-transition matrix (acceptance states < 0)
    private static final int matrix[][] = {

        /*        letter digit   +    -    .    E other */
        /*  0 */ {   1,    4,    3,   3, ERR,   1, ERR },
        /*  1 */ {   1,    1,   -2,  -2,  -2,   1,  -2 },
        /*  2 */ { ERR,  ERR,  ERR, ERR, ERR, ERR, ERR },
        /*  3 */ { ERR,    4,  ERR, ERR, ERR, ERR, ERR },
        /*  4 */ {  -5,    4,   -5,  -5,   6,   9,  -5 },
        /*  5 */ { ERR,  ERR,  ERR, ERR, ERR, ERR, ERR },
        /*  6 */ { ERR,    7,  ERR, ERR, ERR, ERR, ERR },
        /*  7 */ {  -8,    7,   -8,  -8,  -8,   9,  -8 },
        /*  8 */ { ERR,  ERR,  ERR, ERR, ERR, ERR, ERR },
        /*  9 */ { ERR,   11,   10,  10, ERR, ERR, ERR },
        /* 10 */ { ERR,   11,  ERR, ERR, ERR, ERR, ERR },
        /* 11 */ { -12,   11,  -12, -12, -12, -12, -12 },
        /* 12 */ { ERR,  ERR,  ERR, ERR, ERR, ERR, ERR },
    };

    private char ch;    // current input character
    private int state;  // current state

    /**
     * Extract the next token from the source file.
     * @return name of the next token
     * @throws Exception if an error occurs.
     */
    private String nextToken()
        throws Exception
    {
        // Skip blanks.
        while (Character.isWhitespace(ch)) {
            nextChar();
        }

        // At EOF?
        if (ch == 0) {
            return null;
        }

        state = 0;  // start state
        StringBuilder buffer = new StringBuilder();

        // Loop to do state transitions.
        while (state >= 0) {   // not acceptance state
            state = matrix[state][typeOf(ch)];  // transition

            if ((state >= 0) || (state == ERR)) {
                buffer.append(ch);  // build token string
                nextChar();
            }
        }

        return buffer.toString();
    }

    /**
     * Scan the source file.
     * @throws Exception if an error occurs.
     */
    private void scan()
        throws Exception
    {
        nextChar();

        while (ch != 0) {  // EOF?
            String token = nextToken();

            if (token != null) {
                System.out.print("=====> \"" + token + "\" ");
                String tokenType =
                      (state ==  -2) ? "IDENTIFIER"
                    : (state ==  -5) ? "INTEGER"
                    : (state ==  -8) ? "REAL (fraction only)"
                    : (state == -12) ? "REAL"
                    :                  "*** ERROR ***";
                System.out.println(tokenType);
            }
        }
    }

    /**
     * Return the character type.
     * @param ch the character.
     * @return the type.
     */
    int typeOf(char ch)
    {
        return   (ch == 'E')            ? E
               : Character.isLetter(ch) ? LETTER
               : Character.isDigit(ch)  ? DIGIT
               : (ch == '+')            ? PLUS
               : (ch == '-')            ? MINUS
               : (ch == '.')            ? DOT
               :                          OTHER;
    }

    private BufferedReader reader;
    private String line = null;
    private int lineNumber = 0;
    private int linePos = -1;

    /**
     * Get the next character form the source file.
     * @throws Exception if an error occurred.
     */
    void nextChar()
        throws Exception
    {
        if ((line == null) || (++linePos >= line.length())) {
            line = reader.readLine();

            if (line != null) {
                System.out.println("[" + ++lineNumber + "] " + line);
                line += " ";
                linePos = 0;
                ch = line.charAt(0);
            }
            else ch = 0;
        }
        else {
            ch = line.charAt(linePos);
        }
    }

    /**
     * Constructor.
     * @param sourcePath the source path.
     */
    public SimpleDFAScanner(String sourcePath)
    {
        try {
            reader = new BufferedReader(new FileReader(sourcePath));
        }
        catch(IOException ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
    }

    /**
     * Main.
     * @param args command-line arguments.
     * @throws Exception if an error occurred.
     */
    public static void main(String[] args)
        throws Exception
    {
        SimpleDFAScanner scanner = new SimpleDFAScanner(args[0]);
        scanner.scan();
    }
}
