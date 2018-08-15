PROGRAM roman (output);

VAR
   x, y : integer;

BEGIN
    y := 1;
    REPEAT
        x := y;
        write(x:4, ' ');
        WHILE x >= 1000 DO BEGIN
            write('m');
            x := x - 1000;
        END;
        WHILE x >= 500 DO BEGIN
            write('d');
            x := x - 500;
        END;
        WHILE x >= 100 DO BEGIN
            write('c');
            x := x - 100;
        END;
        WHILE x >= 50 DO BEGIN
            write('l');
            x := x - 50;
        END;
        WHILE x >= 10 DO BEGIN
            write('x');
            x := x - 10;
        END;
        WHILE x >= 5 DO BEGIN
            write('v');
            x := x - 5;
        END;
        WHILE x >= 1 DO BEGIN
            write('i');
            x := x - 1;
        END;
        writeln;
        y := 2*y;
    UNTIL y > 5000;
END.

