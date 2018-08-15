PROGRAM ForTest;

VAR
    i, j, k, n : integer;

BEGIN {FOR statements}
    j := 1;

    FOR k := j TO 5 DO n := k;

    FOR k := n DOWNTO 1 DO j := k;

    FOR i := 1 TO 2 DO BEGIN
        FOR j := 1 TO 3 DO BEGIN
            k := i*j
        END
    END
END.
