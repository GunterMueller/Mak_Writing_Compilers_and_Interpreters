PROGRAM sqroots (input, output);

VAR
    n : integer;

FUNCTION sqrt (x : real) : real;

    CONST
	epsilon = 1e-12;

    VAR
	root, delta : real;

    BEGIN
	root := 1;
	REPEAT
	    root  := (x/root + root)/2;
	    delta := x/(root*root) - 1;
	UNTIL delta*delta < epsilon;
	sqrt := root;
    END;

BEGIN
    writeln('  n   sqrt(n)');
    writeln('-------------');
    
    FOR n := 90 TO 101 DO BEGIN
	writeln(n:3, sqrt(n):10:6);
    END;
END.
