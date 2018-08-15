PROGRAM varparms (output);

    VAR
	i, j, k : integer;

    PROCEDURE proc2 (    pi : integer;
		     VAR pj : integer);

	VAR
	    k : integer;

	PROCEDURE proc3 (    ppi : integer;
			 VAR ppj : integer);

	    VAR
		k : integer;

	    BEGIN {proc3}
		ppi := 123;     writeln(ppi);
		ppj := -123;    writeln(ppj);
		k := ppi;       writeln(k);
		k := pi;        writeln(k);
		k := i;         writeln(k);
		k := ppj;       writeln(k);
		k := pj;        writeln(k);
		k := j;         writeln(k);
	    END {proc3};

	BEGIN {proc2}
	    pi := 12;   writeln(pi);
	    pj := -12;  writeln(pj);
	    k := pi;    writeln(k);
	    k := i;     writeln(k);
	    k := pj;    writeln(k);
	    k := j;     writeln(k);

	    proc3(i, j);

	    k := pi;    writeln(k);
	    k := i;     writeln(k);
	    k := pj;    writeln(k);
	    k := j;     writeln(k);
	END {proc2};

    BEGIN {main}
	i := 1;         writeln(i);
	j := -1;        writeln(j);

	proc2(i, j);

	k := i;         writeln(k);
	k := j;         writeln(k);
    END {main}.



