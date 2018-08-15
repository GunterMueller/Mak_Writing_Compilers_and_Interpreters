PROGRAM simple (output);

    VAR
	n : integer;
	x : real;

    PROCEDURE proc (i : integer; VAR j : integer);

	FUNCTION func (y : real) : real;

	    BEGIN {func}
		j := 5;
		func := i + y + 0.5;
	    END {func};

	BEGIN {proc}
	    j := i DIV 2;
	    writeln('In proc, the value of j is', j:3);
	    x := func(3.14);
	    writeln('In proc, the value of j is', j:3);
	END {proc};

    BEGIN {simple}
	n := 1;
	writeln('In simple, the value of n is', n:3);
	proc(7, n);
	writeln('In simple, the value of n is', n:3,
		' and the value of x is', x:8:4);
    END {simple}.


