PROGRAM main1 (output);
    VAR m1v : integer;

    PROCEDURE proc2(p2p : integer);
	VAR p2v : integer;
            p2a : ARRAY [-5..5] OF integer;

	FUNCTION func3 (f3p : integer) : integer;
	    VAR f3v : integer;

	    FUNCTION func4 (f4p : integer) : integer;
		VAR f4v : integer;

		BEGIN {func4}
		    f4v := -4;          writeln(f4v);
		    f3v := f4v;         writeln(f3v);
		    p2v := f3v;         writeln(p2v);
		    m1v := f3v;         writeln(m1v);
		    func3 := -3;
		    func4 := -4;
		    f4p := f3p;         writeln(f4p);
		    f3p := m1v;         writeln(f3p);
		    p2p := f3p;         writeln(p2p);
                    p2a[f3v] := p2p;        writeln(p2a[f3v]);
                    p2a[p2v] := p2a[f3v];   writeln(p2a[p2v]);
		END {func4};

	    BEGIN {func3}
	       f3v := -3;               writeln(f3v);
	       p2v := f3v;              writeln(p2v);
	       m1v := p2v;              writeln(m1v);
	       func3 := func4(f3v);
	       f3p := m1v;              writeln(f3p);
	       p2p := f3p;              writeln(p2p);
	    END {func3};

	PROCEDURE proc3 (p3p : integer);
	    VAR p3v : integer;

	    PROCEDURE proc4 (p4p : integer);
		VAR p4v : integer;

		BEGIN {proc4}
		    p4v := 4;           writeln(p4v);
		    p3v := p4v;         writeln(p3v);
		    p2v := p3v;         writeln(p2v);
		    m1v := p2v;         writeln(m1v);
		    p4p := m1v;         writeln(p4p);
		    p3p := p4p;         writeln(p3p);
		    p2p := p3p;         writeln(p2p);
		    p4p := func3(p2p);  writeln(p4p);
		END {proc4};

	    BEGIN {proc3}
		p3v := 3;               writeln(p3v);
		p2v := p3v;             writeln(p2v);
		m1v := p2v;             writeln(m1v);
		p3p := m1v;             writeln(p3p);
		proc4(p3p);
	    END {proc3};

	BEGIN {proc2}
	    p2v := m1v;                 writeln(p2v);
	    p2p := p2v;                 writeln(p2p);
	    proc3(p2p);
	END {proc2};

    BEGIN {main1}
	m1v := 1;                       writeln(m1v);
	proc2(m1v);
    END {main1}.
