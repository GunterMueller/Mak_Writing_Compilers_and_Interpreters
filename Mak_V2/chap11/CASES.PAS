PROGRAM cases (output);

VAR
    i, j, k : integer;
    ch      : char;
    e, ee   : (fee, fie, foe, fum);

BEGIN
    FOR i := -3 TO 3 DO BEGIN
	CASE i + 1 OF
	    3, 1    :	BEGIN
			    k := i + 1;
			    CASE k OF
				1 : j := 10;
				3 : j := 30;
			    END;
			END;
	    4 : j := 40;
	    -1, 2, 0 : 	BEGIN
			    k := i + 1;
			    CASE k OF
				-1 : j := -10;
				 0 : j := 0;
				 2 : j := 20
			    END
			END;
	    -2: j := -20;
	END;
	writeln(j);
    END;

    CASE 'x' OF
	'q': ch := 'q';
	'x': ch := 'x';
	'r': ch := 'r';
    END;
    writeln;
    writeln(ch);
    writeln;

    FOR e := fum DOWNTO fee DO BEGIN
	CASE e OF
	    fie: ee := foe;
	    foe: ee := fie;
	    fum: ee := fee;
	    fee: ee := fum;
	END;
	writeln(ord(ee));
    END;
END.