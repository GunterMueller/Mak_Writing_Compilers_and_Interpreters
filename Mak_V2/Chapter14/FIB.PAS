PROGRAM fibonacci (output);

VAR
    number : integer;

FUNCTION fib (n : integer) : integer;

    BEGIN
	IF n <= 1 THEN fib := n
	ELSE fib := fib(n - 2) + fib(n - 1)
    END;

BEGIN
    FOR number:= 0 TO 20 DO BEGIN
	writeln('number = ', number:2,
		' fibonacci = ', fib(number):4);
    END;
END.
