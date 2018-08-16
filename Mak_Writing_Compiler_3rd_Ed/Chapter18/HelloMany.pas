PROGRAM HelloMany;

VAR
    count : integer;

PROCEDURE sayHello(howManyTimes : integer);

    VAR
        i : integer;

    BEGIN
        FOR i := 1 TO howManyTimes DO BEGIN
            writeln(i:3, ': ', 'Hello, world.')
        END
    END;

BEGIN {HelloMany}
    write('How many times? ');
    read(count);
    writeln;

    sayHello(count);
END.
