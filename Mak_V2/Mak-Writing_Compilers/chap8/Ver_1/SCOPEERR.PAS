PROGRAM scope (input, output);

VAR
    i, j, k : integer;

FUNCTION f (j : boolean) : real;

    VAR
	i : real;

    BEGIN {f}
	i := 1.0;    {local of f}
	j := false;  {parm  of f}
	k := 3;      {global}

	i := 1;  j := 2;  k := 3;  {j ERROR}
	
	f := i;
    END {f};

PROCEDURE p1 (j : integer);

    VAR
	i : char;

    PROCEDURE p2 (k : boolean);

	VAR
	    n : integer;

	BEGIN {p2}
	    i := 'x';   {local of p1}
	    j := 5;     {parm  of p1}
	    k := true;  {parm  of p2}
	    n := 7;	{local of p2}

	    i := 1;  j := 2;  k := 3;  {i, k ERROR}
	END {p2};

    BEGIN {p1}
	i := 'z';  {local of p1}
	j := 7;    {parm  of p1}
	k := 9;    {global}

	i := 1;  j := 2;  k := 3;  n := 4;  {i, n ERROR}
    END {p1};


BEGIN {scope}
   i := 1;  {global}
   j := 2;  {global}
   k := 3;  {global}
END {scope}.
