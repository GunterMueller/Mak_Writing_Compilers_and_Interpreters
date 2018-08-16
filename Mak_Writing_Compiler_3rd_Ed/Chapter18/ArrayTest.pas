PROGRAM ArrayTest;

TYPE
    vector = ARRAY[0..9] OF integer;
    matrix = ARRAY[0..4, 0..4] OF integer;
    cube   = ARRAY[0..1, 0..2, 0..3] OF integer;

VAR
    i, j, k, n : integer;
    a1         : vector;
    a2         : matrix;
    a3         : cube;

BEGIN
    i := 0; j := 0; k := 0;

    j := a1[i];
    k := a2[i, j];
    n := a3[i, j, k];

    a1[i] := j;
    a2[i, j] := k;
    a3[i, j, k] := n;

    a3[i][a1[j]][k] := a2[i][j] - a3[k, 2*n][k+1];
END.
