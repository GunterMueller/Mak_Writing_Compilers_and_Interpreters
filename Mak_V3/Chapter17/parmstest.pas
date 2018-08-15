PROGRAM ParmsTest;

VAR
    i, j : integer;
    x, y : real;

PROCEDURE proc(ii : integer; VAR jj : integer;
               xx : real; VAR yy : real);

    BEGIN
        ii := jj;
        yy := xx;
    END;

BEGIN
    proc(i, j, x, y)
END.
