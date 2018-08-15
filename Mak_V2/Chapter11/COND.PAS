PROGRAM conditional (output);

VAR
    i, j : integer;
    p, q : boolean;

BEGIN
    p := true;
    q := false;

    IF p THEN i :=  1;
    IF q THEN j := -1;

    IF p THEN i := 1 ELSE j := -1;
    IF q THEN i := 1 ELSE j := -1;
END.
