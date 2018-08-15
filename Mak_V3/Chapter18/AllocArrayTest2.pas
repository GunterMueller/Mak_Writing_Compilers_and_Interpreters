PROGRAM AllocArrayTest2;

TYPE
    string = ARRAY[1..5] OF char;
    vector = ARRAY[0..9] OF string;
    matrix = ARRAY[0..4, 0..3] OF string;
    cube   = ARRAY[0..1, 0..2, 0..3] OF string;

VAR
    a1 : vector;
    a2 : matrix;
    a3 : cube;

BEGIN
END.
