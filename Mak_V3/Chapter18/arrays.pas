PROGRAM arrays;

TYPE
    vector = ARRAY[0..9] OF integer;
    matrix = ARRAY[0..4, 0..4] OF integer;
    cube   = ARRAY[0..1, 0..2, 0..3] OF integer;

VAR
    i, j, k : integer;
    v       : vector;
    vv      : matrix;
    vvv     : cube;

PROCEDURE doMatrix(VAR m : matrix);
    BEGIN
        FOR i := 0 TO 4 DO BEGIN
            FOR j := 0 TO 4 DO BEGIN
                m[i,j] := i*j;
            END;
        END;
    END;

PROCEDURE printCube(VAR c : cube);
    BEGIN
        writeln;
        FOR j := 0 TO 2 DO BEGIN
            FOR i := 0 TO 1 DO BEGIN
                FOR k := 0 TO 3 DO BEGIN
                    write(c[i,j][k]:4);
                END;
                write('     ');
            END;
            writeln;
        END;
    END;

BEGIN
    v[0] := 1;
    writeln(v[0]);

    vv[3][4] := 34;
    writeln(vv[3][4]);

    vvv[1][2][3] := 123;
    writeln(vvv[1][2][3]);

    FOR i := 1 TO 9 DO BEGIN
        v[i] := 2*v[i-1];
    END;

    writeln;
    FOR i := 0 TO 9 DO BEGIN
        write(v[i]:4);
    END;
    writeln;

    doMatrix(vv);

    writeln;
    FOR i := 0 TO 4 DO BEGIN
        FOR j := 0 TO 4 DO BEGIN
            write(vv[i,j]:3);
        END;
        writeln;
    END;

    FOR i := 0 TO 1 DO BEGIN
        FOR j := 0 TO 2 DO BEGIN
            FOR k := 0 TO 3 DO BEGIN
                vvv[i,j][k] := 100*i + 10*j +k;
            END;
        END;
    END;

    printCube(vvv);
END.
