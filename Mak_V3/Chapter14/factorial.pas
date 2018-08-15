PROGRAM factorials (output);

VAR
    number : integer;

FUNCTION fact(n : integer) : integer;

    BEGIN
        IF n <= 1 THEN fact := 1
                  ELSE fact := n*fact(n - 1);
    END;

BEGIN
    number := 0;
    REPEAT
        writeln('number = ', number:1,
                '   number! = ', fact(number):5);
        number := number + 1;
    UNTIL number > 7;
END.
