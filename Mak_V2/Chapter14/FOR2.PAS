PROGRAM fortest (output);

VAR
    i : integer;

PROCEDURE proc (VAR n : integer);

    BEGIN
	FOR n := 1 TO 10 DO writeln('n = ', n);
    END;

BEGIN
    proc(i);
    writeln;
    writeln('i = ', i);
END.
