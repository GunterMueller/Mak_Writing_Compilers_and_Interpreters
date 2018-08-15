PROGRAM eratosthenes (output);

CONST
    max = 1000;

VAR
    sieve : ARRAY [1..max] OF boolean;
    i, j, limit, prime, factor : integer;

BEGIN
    limit := max DIV 2;
    sieve[1] := false;
    FOR i := 2 TO max DO sieve[i] := true;

    prime := 1;
    REPEAT
        prime := prime + 1;
	WHILE NOT sieve[prime] DO prime := prime + 1;
	factor := 2*prime;
	WHILE factor <= max DO BEGIN
	    sieve[factor] := false;
	    factor := factor + prime;
	END
    UNTIL prime > limit;

    writeln('Sieve of Eratosthenes');
    writeln;

    i := 1;
    REPEAT
	FOR j := 0 TO 19 DO BEGIN
	    prime := i + j;
	    IF sieve[prime] THEN write(prime:3)
			    ELSE write('   ');
	END;
        writeln;
	i := i + 20
    UNTIL i > max
END.
