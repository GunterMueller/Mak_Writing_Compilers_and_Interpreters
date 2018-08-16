PROGRAM hilbert (input, output);

{Uses the decompose and solve procedures from the linear equations
 solver (lineq.pas) to invert matrices.  Uses a Hilbert matrix of
 rank 1..5 as an example.  Hilbert matrices are ill-conditioned and
 difficult to invert accurately.  This program inverts a Hilbert
 matrix, and then it inverts the inverse.}

CONST
    max = 5;

TYPE
    vector = ARRAY [1..max]         OF real;
    matrix = ARRAY [1..max, 1..max] OF real;

VAR
    ps      : ARRAY [1..max] OF integer;  {global pivot index array}
    H       : matrix;                     {Hilbert matrix}
    b, x    : vector;                     {RHS and solution vectors}
    i, j, n : integer;

PROCEDURE singular (why : integer);

    BEGIN
        CASE why OF
            0 : writeln('Matrix with zero row in decompose.');
            1 : writeln('Singular matrix in decompose.');
            2 : writeln('No convergence in improve.');
        END;
    END;

PROCEDURE decompose (n : integer; VAR A, LU : matrix);

    {Computes triangular matrices L and U and permutation matrix P
     so that LU = PA.  Stores L-I and U in LU.  Vector ps contains
     permuted row indices.  Note that A and LU are often passed the
     same matrix.}

    VAR
        scales : vector;
        i, j, k, pivotindex : integer;
        normrow, pivot, size, biggest, mult : real;

    BEGIN

        {Initialize ps, LU, and scales.}
        FOR i := 1 TO n DO BEGIN  {rows}
            ps[i] := i;
            normrow := 0;

            FOR j := 1 TO n DO BEGIN  {columns}
                LU[i,j] := A[i,j];

                {Find the largest row element.}
                IF normrow < abs(LU[i,j]) THEN normrow := abs(LU[i,j]);
            END;

            {Set the scaling factor for row equilibration.}
            IF normrow <> 0 THEN scales[i] := 1/normrow
            ELSE BEGIN
                scales[i] := 0;
                singular(0);
            END;
        END;

        {Gaussian elimination with partial pivoting.}
        FOR k := 1 TO n-1 DO BEGIN  {pivot row k}
            pivotindex := 0;
            biggest := 0;

            {Go down rows from row k.}
            FOR i := k TO n DO BEGIN

                {Divide by the largest row element.}
                size := abs(LU[ps[i], k])*scales[ps[i]];

                IF biggest < size THEN BEGIN
                    biggest    := size;  {biggest scales column element}
                    pivotindex := i;     {row index of this element}
                END;
            END;

            IF biggest = 0 THEN singular(1)
            ELSE BEGIN
                IF pivotindex <> k THEN BEGIN

                    {Exchange rows.}
                    j := ps[k];
                    ps[k] := ps[pivotindex];
                    ps[pivotindex] := j;
                END;

                pivot := LU[ps[k], k];  {pivot element}

                {Go down rest of rows.}
                FOR i := k+1 TO n DO BEGIN
                    mult := LU[ps[i], k]/pivot;
                    LU[ps[i], k] := mult;

                    IF mult <> 0 THEN BEGIN

                        {Inner loop.  Only column subscript varies.}
                        FOR j := k+1 TO n DO BEGIN
                            LU[ps[i], j] := LU[ps[i], j]
                                                - mult*LU[ps[k], j];
                        END;
                    END;
                END;
            END;
        END;

        {Check bottom right element of permuted matrix.}
        IF LU[ps[n], n] = 0 THEN singular(1);
    END;

PROCEDURE solve (n : integer; VAR LU : matrix; VAR b, x : vector);

    {Solves Ax = b using LU from decompose.}

    VAR
        i, j : integer;
        dot  : real;

    BEGIN

        {Ly = b : solve for y.}
        FOR i := 1 TO n DO BEGIN
            dot := 0;
            FOR j := 1 TO i-1 DO BEGIN
                dot := dot + LU[ps[i], j]*x[j];
            END;
            x[i] := b[ps[i]] - dot;
        END;

        {Ux = y : solve for x.}
        FOR i := n DOWNTO 1 DO BEGIN
            dot := 0;
            FOR j := i+1 TO n DO BEGIN
                dot := dot + LU[ps[i], j]*x[j];
            END;
            x[i] := (x[i] - dot)/LU[ps[i], i];
        END;
    END;

PROCEDURE invert (n : integer; VAR A, AINV : matrix);

    {Compute AINV = inverse(A).  Note that A and AINV are often
     passed the same matrix.}

    VAR
        LU :   matrix;
        b, x : vector;
        i, j : integer;

    BEGIN
        decompose(n, A, LU);

        FOR j := 1 TO n DO BEGIN
            FOR i := 1 TO n DO BEGIN
                IF i = j THEN b[i] := 1
                         ELSE b[i] := 0;
            END;

            solve(n, LU, b, x);

            FOR i := 1 TO n DO AINV[i,j] := x[i];
        END;
    END;

PROCEDURE printmatrix (VAR M : matrix);

    VAR
        i, j : integer;

    BEGIN
        writeln;
        FOR i := 1 TO n DO BEGIN
            FOR j := 1 TO n DO write(M[i,j]:15:6);
            writeln;
        END;
        writeln;
    END;

BEGIN
    writeln;
    write('Rank of Hilbert matrix (1-', max:0, ')? ');
    read(n);

    {Compute the Hilbert matrix.}
    FOR i := 1 TO n DO BEGIN
        FOR j := 1 TO n DO BEGIN
            H[i,j] := 1/(i + j - 1);
        END;
    END;

    writeln;
    writeln('Hilbert matrix:');
    printmatrix(H);

    {Invert the Hilbert matrix.}
    invert(n, H, H);

    writeln('Hilbert matrix inverted:');
    printmatrix(H);

    {Invert the inverse.}
    invert(n, H, H);

    writeln('Inverse matrix inverted:');
    printmatrix(H);
END.
