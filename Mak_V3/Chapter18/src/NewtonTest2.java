import java.io.*;
import java.util.Scanner;

public class NewtonTest2
{
    private static final float EPSILON = 1.0e-6f;

    private static Scanner _standardIn = new Scanner(System.in);
    private static int number;

    private static float root(float x)
    {
        float r;

        r = 1;
        do {
            r = (x/r + r)/2;
        } while (Math.abs(x/(r*r) - 1) >= EPSILON);

        return r;
    }

    private static void print(int n, float root)
    {
        System.out.println(String.format("The square root of %4d is %8.4f",
                                         n, root));
    }

    public static void main(String[] args)
    {
        do {
            System.out.println();
            System.out.print("Enter new number (0 to quit): ");
            number = _standardIn.nextInt();

            if (number == 0) {
                print(number, 0.0f);
            }
            else if (number < 0) {
                System.out.println("*** ERROR:  number < 0");
            }
            else {
                print(number, root(number));
            }

        } while (number != 0);
    }
}

/*
 PROGRAM newton;

 CONST
     epsilon = 1e-6;

 VAR
     number : integer;

 FUNCTION root(x : real) : real;
     VAR
         r : real;

     BEGIN
         r := 1;
         REPEAT
             r := (x/r + r)/2;
         UNTIL abs(x/sqr(r) - 1) < epsilon;
         root := r;
     END;

 PROCEDURE print(n : integer; root : real);
     BEGIN
         writeln('The square root of ', number:4, ' is ', root:10:6);
     END;

 BEGIN
     REPEAT
         writeln;
         write('Enter new number (0 to quit): ');
         read(number);

         IF number = 0 THEN BEGIN
             print(number, 0.0);
         END
         ELSE IF number < 0 THEN BEGIN
             writeln('*** ERROR:  number < 0');
         END
         ELSE BEGIN
             print(number, root(number));
         END
     UNTIL number = 0
END.
*/
