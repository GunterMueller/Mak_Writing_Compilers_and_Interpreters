import java.util.Scanner;

public class NewtonTest1
{
    private static RunTimer _runTimer;
    private static Scanner _standardIn;

    private static int number;

    private static float root(float x)
    {
        float r;

        r = 1;

        r = (x/r + r)/2;
        r = (x/r + r)/2;
        r = (x/r + r)/2;
        r = (x/r + r)/2;
        r = (x/r + r)/2;

        return r;
    }

    private static void print(int n, float root)
    {
        System.out.println(String.format("The square root of %4d is %8.4f",
                                         n, root));
    }

    public static void main(String[] args)
    {
        _runTimer = new RunTimer();
        _standardIn = new Scanner(System.in);

        number = 4;
        print(number, root(number));

        _runTimer.printElapsedTime();
    }
}

/*
 PROGRAM newton1;

 CONST
     epsilon = 1e-6;

 VAR
     number : integer;

 FUNCTION root(x : real) : real;
     VAR
         r : real;

     BEGIN
         r := 1;

         r := (x/r + r)/2;
         r := (x/r + r)/2;
         r := (x/r + r)/2;
         r := (x/r + r)/2;
         r := (x/r + r)/2;

         root := r;
     END;

 PROCEDURE print(n : integer; root : real);
     BEGIN
         writeln('The square root of ', number:4, ' is ', root:8:4);
     END;

 BEGIN
     number := 4;
     print(number, root(number));
END.
*/
