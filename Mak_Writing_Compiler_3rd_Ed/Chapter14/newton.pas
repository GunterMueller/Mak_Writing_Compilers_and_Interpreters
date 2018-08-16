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
        writeln('The square root of ', number:4, ' is ', root:8:4);
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
