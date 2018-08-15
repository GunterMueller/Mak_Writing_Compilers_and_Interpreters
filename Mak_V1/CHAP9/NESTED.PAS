PROGRAM main1 (output);

    VAR
	i, j : integer;

    FUNCTION func2 : integer;

	VAR
	    i, j : integer;

	FUNCTION func3 : integer;

	    VAR
		i, j : integer;

	    BEGIN
		i := 123;	{123}
		func3 := 0;	{0}
		j := i;		{123}
	    END;

	BEGIN
	    i := 12;		{12}
	    func2 := func3;	{0}
	    j := i;		{12}
	END;

    PROCEDURE proc2;

	VAR
	    i, j : integer;

	PROCEDURE proc3;

	    VAR
		i, j : integer;

	    BEGIN
		i := -123;	{-123}
		j := func2;	{0}
		j := i;		{-123}
	    END;

	BEGIN
	    i := -12;	{-12}
	    proc3;
	    j := i;	{-12}
	END;

    BEGIN
	i := 1;	    {1}
	proc2;
	j := i;	    {1}
    END.



