/**
 * <h1>RunTimer</h1>
 *
 * <p>Pascal Runtime Library:
 * Compute and print the elapsed run time of a compiled Pascal program.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class RunTimer
{
    long startTime;

    /**
     * Constructor.
     */
    public RunTimer()
    {
        startTime = System.currentTimeMillis();
    }

    /**
     * Compute and print the elapsed run time.
     */
    public void printElapsedTime()
    {
        float elapsedTime = (System.currentTimeMillis() - startTime)/1000f;
        System.out.println(
            String.format("\n%,20.2f seconds total execution time.",
                          elapsedTime));
    }
}
