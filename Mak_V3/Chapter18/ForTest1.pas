PROGRAM ForTest1;

TYPE
    grades = (A, B, C, D, F);

VAR
    i, j, k, n : integer;
    grade : grades;
    ch : char;

BEGIN {FOR statements}
    j := 2;

    FOR k := j TO 5 DO BEGIN
        n := k;
        writeln('k = ', k, ', n = ', n);
    END;

    writeln;
    FOR k := n+1 DOWNTO j+2 DO writeln('k = ', k);

    writeln;
    FOR i := 1 TO 2 DO BEGIN
        FOR j := 1 TO 3 DO BEGIN
            k := i*j;
            writeln('i = ', i, ', j = ', j, ', k = ', k)
        END
    END;

    writeln;
    FOR grade := F DOWNTO a DO write(ord(grade):2);
    writeln;

    ch := 'x';

    writeln;
    FOR i := ord(ch) TO ord('z') DO write(chr(i));
    writeln;
END.
