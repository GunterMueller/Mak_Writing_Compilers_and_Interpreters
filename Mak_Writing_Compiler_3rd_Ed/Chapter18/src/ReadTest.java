public class ReadTest
{
    private static PascalTextIn _standardIn;

    private static int i;
    private static float x;
    private static char c;
    private static boolean b;

    public static void main(String[] args)
        throws Exception
    {
        _standardIn = new PascalTextIn();

        System.out.print("Type an integer: ");
        i = _standardIn.readInteger();
        _standardIn.nextLine();
        System.out.println("You typed: " + i);
        System.out.println();

        System.out.print("Type a real: ");
        x = _standardIn.readReal();
        _standardIn.nextLine();
        System.out.println("You typed: " + x);
        System.out.println();

        System.out.print("Type a character: ");
        c = _standardIn.readChar();
        _standardIn.nextLine();
        System.out.println("You typed: " + c);
        System.out.println();

        System.out.print("Type a boolean: ");
        b = _standardIn.readBoolean();
        _standardIn.nextLine();
        System.out.println("You typed: " + b);
        System.out.println();
    }
}

/*
 PROGRAM reads (input, output);

 VAR
     i : integer;
     x : real;
     c : char;
     b : boolean;

 BEGIN
     write('Type an integer: ');
     readln(i);
     writeln('You typed: ', i);
     writeln;

     write('Type a real: ');
     readln(x);
     writeln('You typed: ', x);
     writeln;

     write('Type a character: ');
     readln(c);
     writeln('You typed: ', c);
     writeln;

     write('Type a boolean: ');
     readln(b);
     writeln('You typed: ', b);
     writeln;
END.
*/
