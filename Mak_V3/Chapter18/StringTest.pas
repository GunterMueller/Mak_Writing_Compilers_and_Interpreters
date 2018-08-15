PROGRAM StringTest;

TYPE
    string5 = ARRAY [1..5] OF char;
    string8 = ARRAY [1..8] OF char;

VAR
    str5, strV : string5;
    str8 : string8;

PROCEDURE testStrings(VAR s5 : string5; s8 : string8);

    VAR
        b1, b2, b3 : boolean;

    BEGIN
        b1 := s5 > s8;
        b2 := s5 < 'goodbye';
        b3 := 'nobody' >= s8;

        writeln(b1:6, b2:6, b3:6);
    END;

BEGIN
    str5 := 'hello';
    str8 := 'everyone';
    writeln('''', str5, ', ', str8, '''');

    testStrings(str5, str8);

    str5 := str8;  {truncate}
    strV := str5;
    writeln('''', str5, ', ', strV, ', ', str8, '''');

    str5 := 'hello';
    str8 := str5;  {blank pad}
    writeln('''', str8, '''');
END.
