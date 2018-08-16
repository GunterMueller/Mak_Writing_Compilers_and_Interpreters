PROGRAM ForTest;

TYPE
    grades = (A, B, C, D, F);

VAR
    i, j, k, n : integer;
    grade : grades;
    ch : char;

BEGIN {FOR statements}
    j  := 2;
    ch := 'x';

    FOR k := j TO 5 DO BEGIN
        n := k;
    END;

    FOR k := n+1 DOWNTO j+2 DO i := k;

    FOR i := 1 TO 2 DO BEGIN
        FOR j := 1 TO 3 DO BEGIN
            k := i*j;
        END
    END;

    FOR grade := F DOWNTO a DO i := ord(grade);

    FOR i := ord(ch) TO ord('z') DO j := i;
END.
