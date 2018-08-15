PROGRAM lineq (input, output);

{Solves a set of simultaneous linear equations Ax = b of rank 1..5.
 Sample input:

	rank:	3
	row 1:	2  1 -3  5
	row 2:  3 -2  2  5
	row 3:  5 -3 -1 16

 Represents the system:

	2x +  y - 3z =  5
	3x - 2y + 2z =  5
	5x - 3y -  z = 16

 With the solution (1, -3, -2).}

CONST
    max = 5;

TYPE
    vector = ARRAY [1..max]         OF real;
    matrix = ARRAY [1..max, 1..max] OF real;

VAR
    ps      : ARRAY [1..max] OF integer;  {global pivot index array}
    A       : matrix;                     {coefficient matrix}
    b, x    : vector;                     {RHS and solution vectors}
    i, j, n : 1..max;

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

BEGIN
    write('Rank of system? '); read(n);

    FOR i := 1 TO n DO BEGIN
       write('Row ', i:0, ' and its RHS? ');
       FOR j := 1 TO n DO read (A[i,j]);
       read(b[i]);
    END;

    decompose(n, A, A);
    solve(n, A, b, x);

    writeln;
    write('Solution = (');
    FOR i := 1 TO n-1 DO write(x[i]:4:2, ', ');
    writeln(x[n]:4:2, ')');
END.
