PROGRAM queens (output);

VAR
    i : integer;
    q : boolean;
    a : ARRAY [ 1.. 8] OF boolean;
    b : ARRAY [ 2..16] OF boolean;
    c : ARRAY [-7.. 7] OF boolean;
    x : ARRAY [ 1.. 8] OF integer;

PROCEDURE try (i : integer; VAR q : boolean);

    VAR
        j : integer;

    BEGIN
        j := 0;
        REPEAT
            j := j + 1;
            q := false;
            IF a[j] AND b[i+j] AND c[i-j] THEN BEGIN
		x[i]   := j;
                a[j]   := false;
                b[i+j] := false;
                c[i-j] := false;
                IF i < 8 THEN BEGIN
                    try(i+1, q);
                    IF NOT q THEN BEGIN
                        a[j]   := true;
                        b[i+j] := true;
                        c[i-j] := true;
                    END
                END
                ELSE q := true;
            END
        UNTIL q OR (j = 8);
    END;

BEGIN
    FOR i :=  1 TO  8 DO a[i] := true;
    FOR i :=  2 TO 16 DO b[i] := true;
    FOR i := -7 TO  7 DO c[i] := true;
    try(1, q);
    IF q THEN FOR i := 1 TO 8 DO write(x[i]:4);
    writeln;
END.