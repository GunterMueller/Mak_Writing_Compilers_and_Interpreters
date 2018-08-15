PROGRAM graph (output);

CONST
    d = 0.0625;
    s = 32;
    h = 34;
    c = 6.28318;
    lim = 32;

VAR
    x, y : real;
    i, n : integer;

BEGIN
    i := 0;
    WHILE i <= lim DO BEGIN
	x := d*i;
	y := exp(-x)*sin(c*x);
	n := round(s*y) + h;

        IF n < h THEN BEGIN
	    write(' ':n, '*');
            IF h-n-1 > 0 THEN write(' ':h-n-1);
            writeln('|')
        END
        ELSE IF n > h THEN BEGIN
            write(' ':h, '|');
            IF n-h-1 > 0 THEN write(' ':n-h-1);
            writeln('*')
        END
        ELSE writeln(' ':n, '*');

        i := i + 1;
    END;
END.