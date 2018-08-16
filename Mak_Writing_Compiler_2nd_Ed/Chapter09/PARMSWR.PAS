PROGRAM parms (output);

TYPE
    matrix = ARRAY [1..2, 1..3] OF integer;

VAR
    j      : integer;
    i1, i2 : integer;
    m1, m2 : matrix;

PROCEDURE proc2 (    pi1 : integer;
		 VAR pi2 : integer;
		     pm1 : matrix;
		 VAR pm2 : matrix);

    VAR
	j : integer;

    PROCEDURE proc3 (    ppi1 : integer;
		     VAR ppi2 : integer;
    			 ppm1 : matrix;
		     VAR ppm2 : matrix);

	VAR
	    j : integer;

	BEGIN
	    ppi1      := -3333;
	    ppi2      := -3333;
	    ppm1[2,3] := -3333;
	    ppm2[2,3] := -3333;

	    j := ppi1;		{-3333}  writeln(j);
	    j := ppi2;		{-3333}  writeln(j);
	    j := ppm1[2,2];     {-2222}  writeln(j);
	    j := ppm2[2,2];     {-2222}  writeln(j);
	    j := ppm1[2,3];     {-3333}  writeln(j);
	    j := ppm2[2,3];     {-3333}  writeln(j);  writeln;

	    j := pi1;           {-2222}  writeln(j);
	    j := pi2;           {-3333}  writeln(j);
	    j := pm1[2,2];      {-2222}  writeln(j);
	    j := pm2[2,2];      {-2222}  writeln(j);
	    j := pm1[2,3];      {   23}  writeln(j);
	    j := pm2[2,3];      {-3333}  writeln(j);  writeln;

	    j := i1;            {    1}  writeln(j);
	    j := i2;            {-3333}  writeln(j);
	    j := m1[2,2];       {   22}  writeln(j);
	    j := m2[2,2];       {-2222}  writeln(j);
	    j := m1[2,3];       {   23}  writeln(j);
	    j := m2[2,3];       {-3333}  writeln(j);  writeln;
	END;

    BEGIN
        pi1      := -2222;
	pi2      := -2222;
	pm1[2,2] := -2222;
	pm2[2,2] := -2222;

	j := pi1;	    {-2222}  writeln(j);
	j := pi2;	    {-2222}  writeln(j);
	j := pm1[2,2];      {-2222}  writeln(j);
	j := pm2[2,2];      {-2222}  writeln(j);
	j := pm1[2,3];      {   23}  writeln(j);
	j := pm2[2,3];      {   23}  writeln(j);  writeln;

	j := i1;	    {    1}  writeln(j);
	j := i2;            {-2222}  writeln(j);
	j := m1[2,2];       {   22}  writeln(j);
	j := m2[2,2];       {-2222}  writeln(j);
	j := m1[2,3];       {   23}  writeln(j);
	j := m2[2,3];       {   23}  writeln(j);  writeln;

	proc3(pi1, pi2, pm1, pm2);

	j := pi1;	    {-2222}  writeln(j);
	j := pi2;	    {-3333}  writeln(j);
	j := pm1[2,2];      {-2222}  writeln(j);
	j := pm2[2,2];      {-2222}  writeln(j);
	j := pm1[2,3];      {   23}  writeln(j);
	j := pm2[2,3];      {-3333}  writeln(j);  writeln;

	j := i1;	    {    1}  writeln(j);
	j := i2;            {-3333}  writeln(j);
	j := m1[2,2];       {   22}  writeln(j);
	j := m2[2,2];       {-2222}  writeln(j);
	j := m1[2,3];       {   23}  writeln(j);
	j := m2[2,3];       {-3333}  writeln(j);  writeln;
    END;

BEGIN
    i1 := 1;
    i2 := 2;

    m1[1,1] := 11;  m1[1,2] := 12;  m1[1,3] := 13;
    m1[2,1] := 21;  m1[2,2] := 22;  m1[2,3] := 23;

    m2[1,1] := 11;  m2[1,2] := 12;  m2[1,3] := 13;
    m2[2,1] := 21;  m2[2,2] := 22;  m2[2,3] := 23;

    j := m1[2,2];       {22}     writeln(j);
    j := m2[2,2];       {22}     writeln(j);
    j := m1[2,3];       {23}     writeln(j);
    j := m2[2,3];       {23}     writeln(j);  writeln;

    proc2(i1, i2, m1, m2);

    j := i1;		{    1}  writeln(j);
    j := i2;		{-3333}  writeln(j);
    j := m1[2,2];       {   22}  writeln(j);
    j := m2[2,2];       {-2222}  writeln(j);
    j := m1[2,3];       {   23}  writeln(j);
    j := m2[2,3];       {-3333}  writeln(j);  writeln;
END.
