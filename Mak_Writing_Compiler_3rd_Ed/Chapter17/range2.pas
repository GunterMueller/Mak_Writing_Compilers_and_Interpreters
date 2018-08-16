PROGRAM range2;

TYPE
    subrange = 10..20;

VAR
    i : subrange;

PROCEDURE bad(k : subrange);
    BEGIN
    END;

BEGIN
    i := 12;
    bad(i-10);
END.
