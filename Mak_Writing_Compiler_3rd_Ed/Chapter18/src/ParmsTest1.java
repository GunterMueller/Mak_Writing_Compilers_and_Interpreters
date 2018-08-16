import java.util.Scanner;

public class ParmsTest1
{
    private static RunTimer _runTimer;
    private static Scanner _standardIn;

    private static int i;
    private static int j;
    private static float x;
    private static float y;

    public static void goodSwap(IWrap v1,
                                IWrap v2)
    {
        int t = v1.value;
        v1.value = v2.value;
        v2.value = t;
    }

    public static void badSwap(float v1, float v2)
    {
        float t = v1;
        v1 = v2;
        v2 = t;
    }

    public static void main(String[] args)
    {
        _runTimer = new RunTimer();
        _standardIn = new Scanner(System.in);

        i = 10;
        j = 20;

        System.out.println("Calling goodSwap: i = " + i + ", j = " + j);
        IWrap iw = new IWrap(i);
        IWrap jw = new IWrap(j);
        goodSwap(iw, jw);
        i = iw.value;
        j = jw.value;
        System.out.println("          Result: i = " + i + ", j = " + j);

        x = 10;
        y = 20;

        System.out.println("Calling  badSwap: x = " + x + ", y = " + y);
        goodSwap(iw, jw);
        System.out.println("          Result: x = " + x + ", y = " + y);

        _runTimer.printElapsedTime();
    }
}
