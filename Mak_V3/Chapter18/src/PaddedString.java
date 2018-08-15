/**
 * <h1>PaddedString</h1>
 *
 * <p>Pascal Runtime Library:
 * The implementation of a Pascal string.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class PaddedString
{
    public static StringBuilder create(int length)
    {
        return blanks(length, 0);
    }

    public static StringBuilder blanks(int targetLength, int sourceLength)
    {
        int padLength = targetLength - sourceLength;
        StringBuilder padding = new StringBuilder(padLength);

        while (--padLength >= 0) {
            padding.append(' ');
        }

        return padding;
    }
}
