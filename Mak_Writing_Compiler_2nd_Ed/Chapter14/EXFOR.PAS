PROGRAM exfor (output);

VAR
    i  : integer;
    ch : char;

BEGIN
    FOR i := 1 TO 5 DO writeln('i = ', i:0);
    writeln;
    FOR i := 5 DOWNTO 1 DO writeln('i = ', i:0);
    writeln;
    FOR ch := 'a' TO 'e' DO writeln('ch = ', ch:0);
END.
