PROGRAM parms1;

VAR
    i, j : integer;
    x, y : real;

PROCEDURE goodSwap(VAR v1, v2 : integer);
    VAR
        t : integer;

    BEGIN
        t := v1;
        v1 := v2;
        v2 := t;
    END;

PROCEDURE badSwap(v1, v2 : real);
    VAR
        t : real;

    BEGIN
        t := v1;
        v1 := v2;
        v2 := t;
    END;

BEGIN
    i := 10;
    j := 20;
    writeln('Calling goodSwap: i = ', i:0, ', j = ', j:0);
    goodSwap(i, j);
    writeln('          Result: i = ', i:0, ', j = ', j:0);

    x := 10;
    y := 20;
    writeln('Calling  badSwap: x = ', x:0:1, ', y = ', y:0:1);
    badSwap(x, y);
    writeln('          Result: x = ', x:0:1, ', y = ', y:0:1);
END.
