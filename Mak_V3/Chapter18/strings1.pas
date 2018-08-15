PROGRAM strings1;

TYPE
    string5 = ARRAY [1..5] OF char;
    string8 = ARRAY [1..8] OF char;

VAR
    str5   : string5;
    str8   : string8;
    list   : ARRAY [1..3] OF string5;
    matrix : ARRAY [0..1, 0..2] OF string5;
    cube   : ARRAY [0..1, 0..1, 0..1] OF string5;

BEGIN
    str5 := 'hello';
    str8 := 'everyone';
    writeln('''', str5, ', ', str8, '''');

    IF (str5 > str8) THEN writeln('greater');
    IF (str5 > 'goodbye') THEN writeln('greater');
    IF ('nobody' >= str8) THEN writeln('greater');

    str5 := str8;
    writeln('''', str5, '''');

    str5 := 'hello';
    str8 := str5;
    writeln('''', str8, '''');

    str8[8] := '!';
    writeln('''', str8, '''');

    list[1] := 'Ron';
    list[2] := 'John';
    writeln('''', list[1], '''');
    writeln('''', list[2], '''');

    matrix[0,1] := '  0,1';
    matrix[1,2] := '  1,2';
    writeln('''', matrix[0,1], ', ', matrix[1,2], '''');

    cube[0,1,1] := '0,1,1';
    cube[1,0,1] := '1,0,1';
    writeln('''', cube[0,1,1], ', ', cube[1,0,1], '''');

    cube[1,0,1][3] := cube[0,1,1][5];
    writeln('''', cube[1,0,1], '''');

    IF (cube[1,0,1] > cube[0,1,1]) THEN writeln('greater');
    IF (cube[1,0,1][3] > '0') THEN writeln('greater');
END.
