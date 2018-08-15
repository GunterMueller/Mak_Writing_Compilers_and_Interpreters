PROGRAM call (input, output);

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
	k := x;        {5}
	n := func(i);  {6}
	n := func(k);  {8}
    END;

BEGIN
    i := 3;  {3}
    m := 5;  {5}
    proc(m);
    j := i;  {3}
END.
