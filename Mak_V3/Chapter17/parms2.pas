PROGRAM parms2;

VAR
    i, j : integer;

PROCEDURE proc3(VAR p3 : integer);
    BEGIN
        i := i + 1;
        p3 := i;
    END;

PROCEDURE proc2(VAR p2 : integer);
    VAR
        j : integer;
    BEGIN
        i := i + 1;
        j := i;
        proc3(j);
        p2 := j;
    END;

PROCEDURE proc1(VAR p1 : integer);
    BEGIN
        i := i + 1;
        p1 := i;
        proc2(p1);
    END;

BEGIN
    i := 0;
    j := 0;
    writeln('i = ', i, ', j = ', j);

    proc1(j);

    writeln('i = ', i, ', j = ', j);
END.
