PROGRAM exrepeat (output);

VAR
    i : integer;

BEGIN
    i := 10;
    REPEAT
	writeln(i);
	i := i - 1;
    UNTIL i <= 0;
END.
