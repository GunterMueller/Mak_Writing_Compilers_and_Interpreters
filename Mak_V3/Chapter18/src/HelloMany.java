public class HelloMany
{
    private static RunTimer     _runTimer;
    private static PascalTextIn _standardIn;

    private static int count;

    private static void sayHello(int howManyTimes)
    {
        int i;

        for (i = 1; i <= howManyTimes; ++i) {
            System.out.print(String.format("%3d: Hello, world.\n", i));
        }
    }

    public static void main(String[] args)
        throws Exception
    {
        _runTimer = new RunTimer();
        _standardIn = new PascalTextIn();

        System.out.print("How many times? ");
        count = _standardIn.readInteger();
        System.out.println();

        sayHello(count);

        _runTimer.printElapsedTime();
    }
}
