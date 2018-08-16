PROGRAM recursive1;

PROCEDURE proc2(n : integer); forward;

FUNCTION func2(n : integer) : integer;

    FUNCTION func3(n : integer) : integer;
        BEGIN
            writeln('    Start func3');
            proc2(6);
            func3 := 3;
            writeln('    Return func3');
        END;

    BEGIN {func2}
        writeln('  Start func2');
        func2 := func3(5) - 1;
        writeln('  Return func2');
    END;

PROCEDURE proc2;

    PROCEDURE proc3(n : integer);
        VAR k : integer;

        BEGIN
            writeln('    Start proc3');
            CASE n OF
                2: proc3(3);
                3: k := func2(4);
            END;
            writeln('    Return proc3');
        END;

    BEGIN {proc2}
        writeln('  Start proc2');
        CASE n OF
            1: proc3(2);
            6: proc3(7);
        END;
        writeln('  Return proc2');
    END;

BEGIN {main1}
    writeln('Start recursive1');
    proc2(1);
    writeln('End recursive1');
END.
