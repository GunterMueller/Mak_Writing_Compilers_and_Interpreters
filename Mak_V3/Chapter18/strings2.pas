PROGRAM strings2;

TYPE
    string5 = ARRAY [1..5] OF char;
    cube    = ARRAY [0..1, 0..1, 0..1] OF string5;

VAR
    name1, name2 : string5;
    cube1, cube2 : cube;

PROCEDURE xfer1(str1 : string5; VAR str2 : string5);
    BEGIN
        str2 := str1
    END;

PROCEDURE noxfer1(str1 : string5; str2 : string5);
    BEGIN
        str2 := str1
    END;

PROCEDURE xfer2(cb1 : cube; VAR cb2 : cube);
    BEGIN
        cb2[1,0,1] := cb1[1,0,1]
    END;

PROCEDURE noxfer2(cb1 : cube; cb2 : cube);
    BEGIN
        cb2[1,0,1] := cb1[1,0,1]
    END;

BEGIN
    name1 := 'AAAAA';
    name2 := 'BBBBB';
    writeln('''', name1, ', ', name2, '''');

    noxfer1(name1, name2);
    writeln('''', name1, ', ', name2, '''');

    xfer1(name1, name2);
    writeln('''', name1, ', ', name2, '''');

    cube1[1,0,1] := '1,0,1';
    cube2[1,0,1] := 'xxxxx';
    writeln('''', cube1[1,0,1], ', ', cube2[1,0,1], '''');

    noxfer2(cube1, cube2);
    writeln('''', cube1[1,0,1], ', ', cube2[1,0,1], '''');

    xfer2(cube1, cube2);
    writeln('''', cube1[1,0,1], ', ', cube2[1,0,1], '''');
END.
