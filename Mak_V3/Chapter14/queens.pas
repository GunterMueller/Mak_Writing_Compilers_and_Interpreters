PROGRAM queens (output);

VAR
    i : integer;
    q : boolean;
    a : ARRAY [ 1.. 8] OF boolean;
    b : ARRAY [ 2..16] OF boolean;
    c : ARRAY [-7.. 7] OF boolean;
    x : ARRAY [ 1.. 8] OF integer;

PROCEDURE print;

    VAR
        k : integer;

    BEGIN
        FOR k := 1 TO 8 DO write(x[k]:4);
        writeln;
    END;

PROCEDURE try (i : integer);

    VAR
        j : integer;

    BEGIN
        FOR j := 1 TO 8 DO BEGIN
            IF a[j] AND b[i+j] AND c[i-j] THEN BEGIN
		x[i]   := j;
                a[j]   := false;
                b[i+j] := false;
                c[i-j] := false;
                IF i < 8 THEN try(i+1)
                         ELSE print;
                a[j]   := true;
                b[i+j] := true;
		c[i-j] := true;
            END
        END
    END;

BEGIN
    FOR i :=  1 TO  8 DO a[i] := true;
    FOR i :=  2 TO 16 DO b[i] := true;
    FOR i := -7 TO  7 DO c[i] := true;
    try(1);
END.