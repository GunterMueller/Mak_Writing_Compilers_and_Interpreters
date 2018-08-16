PROGRAM ReadTest(input, output);

PROCEDURE doRead;

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
    END

BEGIN
    doRead;
END.
