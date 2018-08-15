PROGRAM primes (output);

CONST
    max = 1000;

VAR
    number, divisor, limit : integer;

BEGIN
    writeln;
    writeln('Primes');
    writeln;
    write(2:4);

    number := 3;
    WHILE number <= max DO BEGIN
        limit   := trunc(sqrt(number));
        divisor := 1;

        REPEAT
            divisor := divisor + 2
        UNTIL (divisor > limit) OR (number MOD divisor = 0);

        IF divisor > limit THEN write(number:4);
        number := number + 2
    END;

    writeln
END.
