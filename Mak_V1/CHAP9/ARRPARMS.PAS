PROGRAM arrparms (output);

TYPE
    matrix = ARRAY [1..2, 1..3] OF integer;

VAR
    i      : integer;
    m1, m2 : matrix;

PROCEDURE proc2 (    pm1 : matrix;
		 VAR pm2 : matrix);

    VAR
	j : integer;

    PROCEDURE proc3 (    ppm1 : matrix;
		     VAR ppm2 : matrix);

	VAR
	    j : integer;

	BEGIN
	    ppm1[1,1] := 99;	{99}
	    ppm2[1,1] := -99;	{-99}
	    j := ppm1[1,1];	{99}
	    j := ppm2[1,1];	{-99}
	    j := m1[1,1];	{11}
	    j := m2[1,1];	{-99}
	END;

    BEGIN
	pm1[2,2] := 77;	    {77}
	pm2[2,2] := -77;    {-77}
	j := pm1[2,2];	    {77}
	j := pm2[2,2];	    {-77}
	j := m1[2,2];	    {22}
	j := m2[2,2];	    {-77}

	proc3(pm1, pm2);
    END;

BEGIN
    m1[1,1] := 11;  m1[1,2] := 12;  m1[1,3] := 13;
    m1[2,1] := 21;  m1[2,2] := 22;  m1[2,3] := 23;

    m2[1,1] := -11;  m2[1,2] := -12;  m2[1,3] := -13;
    m2[2,1] := -21;  m2[2,2] := -22;  m2[2,3] := -23;

    i := m1[1,1];	{11}
    i := m2[1,1];	{-11}
    i := m1[2,2];	{22}
    i := m2[2,2];	{-22}

    proc2(m1, m2);

    i := m1[1,1];	{11}
    i := m2[1,1];	{-99}
    i := m1[2,2];	{22}
    i := m2[2,2];	{-77}
END.

