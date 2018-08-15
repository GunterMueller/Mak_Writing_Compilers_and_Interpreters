PROGRAM newton (input, output);

CONST
    epsilon = 1e-6;

VAR
    number, root, sqroot : real;

BEGIN
    REPEAT
	writeln;
	write('Enter new number (0 to quit): ');
	read(number);

	IF number = 0 THEN BEGIN
	    writeln(number:12:6, 0.0:12:6);
	END
	ELSE IF number < 0 THEN BEGIN
	    writeln('*** ERROR:  number < 0');
	END
	ELSE BEGIN
	    sqroot := sqrt(number);
	    writeln(number:12:6, sqroot:12:6);
	    writeln;

	    root := 1;
	    REPEAT
		root := (number/root + root)/2;
		writeln(root:24:6,
			100*abs(root - sqroot)/sqroot:12:2,
			'%')
	    UNTIL abs(number/sqr(root) - 1) < epsilon;
	END
    UNTIL number = 0
END.
