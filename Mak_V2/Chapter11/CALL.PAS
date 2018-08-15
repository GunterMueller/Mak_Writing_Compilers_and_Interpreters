PROGRAM call (output);

VAR
    i, j, m : integer;

FUNCTION func (z : integer) : integer;

    BEGIN
	func := i + z;  {3 + z}
    END;

PROCEDURE proc (x : integer);

    VAR
	k, n : integer;

    BEGIN
	writeln('Enter proc');
	k := x;        {5}  writeln(k);
	n := func(i);  {6}  writeln(n);
	n := func(k);  {8}  writeln(n);
	writeln('Exit proc');
    END;

BEGIN
    writeln('Enter call');
    i := 3;  {3}  writeln(i);
    m := 5;  {5}  writeln(m);
    proc(m);
    j := i;  {3}  writeln(j);
    writeln('Exit call');
END.
