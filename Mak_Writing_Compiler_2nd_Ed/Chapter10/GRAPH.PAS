PROGRAM graphs (output);

CONST
    xMax      =  2;        {max value of x}
    xUnits    = 16;        {no. of plot units in [x, x+1]}
    yUnits    = 32;        {no. of plot units in [y, y+1]}
    fullWidth = 69;        {full plot line width in plot units}
    halfWidth = 34;        {half of plot line width}
    twoPi     =  6.28318;  {two times pi}

VAR
    x, dx        : real;
    i, maxXUnits : integer;
    line         : ARRAY [-halfWidth .. +halfWidth] OF char;

FUNCTION expsin (x : real) : real;

    BEGIN
	expsin := exp(-x)*sin(x*twoPi);
    END;

PROCEDURE plot (x, y : real);

    VAR
	i, k, n : integer;

    BEGIN
	write(x:5:2, y:6:2);
	n := round(y*yUnits);
	line[0] := ':';  line[n] := '*';

	IF n < 0 THEN k := 0 ELSE k := n;
	FOR i := -halfWidth TO k DO write(line[i]);
	writeln;

	line[n] := ' ';
    END;

BEGIN
    dx        := 1/xUnits;  
    maxXUnits := xMax*xUnits;
    FOR i := -halfWidth TO +halfWidth DO line[i] := ' ';

    FOR i := 0 TO maxXUnits DO BEGIN
	x := i*dx;
	plot(x, expsin(x));
    END;
END.
