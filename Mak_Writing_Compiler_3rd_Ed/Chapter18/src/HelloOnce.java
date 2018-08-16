public class HelloOnce
{
    private static RunTimer     _runTimer;
    private static PascalTextIn _standardIn;

    public static void main(String[] args)
    {
        _runTimer = new RunTimer();
        _standardIn = new PascalTextIn();

        System.out.println("Hello, world.");

        _runTimer.printElapsedTime();
    }
}
