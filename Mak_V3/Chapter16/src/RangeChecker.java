/**
 * <h1>RangeChecker</h1>
 *
 * <p>Pascal Runtime Library: Perform a runtime range check.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class RangeChecker
{
    public static void check(int value, int minValue, int maxValue)
        throws PascalRuntimeException
    {
        if ((value < minValue) || (value > maxValue)) {
            throw new PascalRuntimeException(
                String.format("Range error: %1d not in [%1d, %1d]",
                              value, minValue, maxValue));
        }
    }
}
