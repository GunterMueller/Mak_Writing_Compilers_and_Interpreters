PROGRAM fortest (output);

TYPE
    enum = (zero, one, two, three, four, five);

VAR
    e  : enum;
    ch : char;

BEGIN
    FOR e := one TO five DO BEGIN
        write(ord(e));
        CASE e OF
	    one:   writeln(' one');
	    two:   writeln(' two');
	    three: writeln(' three');
	    four:  writeln(' four');
	    five:  writeln(' five')
        END
    END;

    FOR ch := 'c' DOWNTO 'a' DO BEGIN
        write(ch);
        CASE ch OF
            'a', 'c':  writeln(' a or c');
            'b':       writeln(' b')
        END
    END
END.
