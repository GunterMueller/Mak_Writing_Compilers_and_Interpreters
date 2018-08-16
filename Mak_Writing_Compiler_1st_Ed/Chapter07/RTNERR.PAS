PROGRAM rtn(input, output);

    VAR
	i, j, k    : integer;
	p, z       : real;
	ch, letter : char;

    FUNCTION func(VAR ch : char) : real;
       forward;

    PROCEDURE proc(b     : boolean;
		   VAR x : real;
		   y     : real);

	CONST
	    n = 5;

	VAR
	    p, q : boolean;

	BEGIN
	    p := x + z - n*func(letter);
	    proc(ch);
	END;

    FUNCTION func(i : integer) : boolean;

	TYPE
	    stooge = (larry, moe, curly);

	VAR
	    s, t : stooge;

	BEGIN
	    s := pred(t);
	    proc(s = t, p, j DIV k);
	    func(letter) := 'xyz';
	    func := round(3.14) - trunc(3);
	END;

    BEGIN
	j := -3;
	proc(false, 4.5*z, z);
	z := 3.14 + func(ch, 123) - func + func(p) + func(-3);
    END.
