PROGRAM hello (output);

{Write 'Hello, world.' ten times.}

VAR
    i : integer;

BEGIN {hello}
    FOR i := 1 TO 10 DO Begin
        writeln('Hello, world.');
    END;
END {hello}.
