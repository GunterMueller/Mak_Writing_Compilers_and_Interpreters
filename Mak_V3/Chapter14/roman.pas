PROGRAM roman (output);

VAR
   x, y : integer;

BEGIN
    y := 1;
    REPEAT
        x := y;
        write(x:4, ' ');
        WHILE x >= 1000 DO BEGIN
            write('M');
            x := x - 1000;
        END;
        WHILE x >= 500 DO BEGIN
            write('D');
            x := x - 500;
        END;
        WHILE x >= 100 DO BEGIN
            write('C');
            x := x - 100;
        END;
        WHILE x >= 50 DO BEGIN
            write('L');
            x := x - 50;
        END;
        WHILE x >= 10 DO BEGIN
            write('X');
            x := x - 10;
        END;
        WHILE x >= 5 DO BEGIN
            write('V');
            x := x - 5;
        END;
        WHILE x >= 1 DO BEGIN
            write('I');
            x := x - 1;
        END;
        writeln;
        y := 2*y;
    UNTIL y > 5000;
END.

