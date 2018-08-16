PROGRAM main1 (output);

    VAR
	i, j, k : integer;

    FUNCTION func2 : integer;
	forward;

    PROCEDURE proc2 (flag : boolean);

	VAR
	    i, j : integer;

	PROCEDURE proc3;

	    VAR
		i, j : integer;

	    BEGIN {proc 3}
		i := -123;      {-123}
		j := func2;     {777}
		j := i;         {-123}
	    END {proc 3};

	BEGIN {proc 2}
	    i := -12;   {-12}
	    IF flag THEN proc3;
	    j := i;     {-12}
	END {proc 2};

    FUNCTION func2;

	VAR
	    i, j : integer;

	FUNCTION func3 : integer;

	    VAR
		i, j : integer;

	    BEGIN {func3}
		i := 123;       {123}
		proc2(false);
		func3 := 777;   {777}
		j := i;         {123}
	    END {func3};

	BEGIN {func2}
	    i := 12;            {12}
	    func2 := func3;     {777}
	    j := i;             {12}
	END {func2};

    BEGIN {main1}
	i := 1;     {1}
	k := 14;    {14}
	proc2(true);
	j := i;     {1}
    END {main1}.
