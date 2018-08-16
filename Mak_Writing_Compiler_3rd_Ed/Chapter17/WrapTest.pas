PROGRAM WrapTest;

VAR
   i, j : integer;

PROCEDURE proc(VAR p1 : integer; p2 : integer);

    VAR
        m, n : integer;

    BEGIN
        m  := p1;
        n  := p2;
        p1 := 10*m;
        p2 := 10*n;
    END;

BEGIN
    i := 1;
    j := 2;

    proc(i, j);
END.
