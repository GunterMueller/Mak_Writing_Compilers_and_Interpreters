PROGRAM RepeatTest;

VAR
    number : integer;

FUNCTION sqroot(n : integer) : real;

    CONST
        epsilon = 1.0e-6;

    VAR
        r : real;

    BEGIN
        r := n;

        REPEAT
            r := (n/r + r)/2
        UNTIL r*r - n < epsilon;

        sqroot := r
    END;

BEGIN {Calculate a square root using Newton's method.}
    number := 1024;

    writeln('The square root of ', number:4);
    writeln('    by standard sqrt() function: ', sqrt(number):9:6);
    writeln('  by declared sqroot() function: ', sqroot(number):9:6);
END.
