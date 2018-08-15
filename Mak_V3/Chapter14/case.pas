PROGRAM casetest;

TYPE
    sizes = (small, medium, large);

VAR
    i, j, even, odd, prime : integer;
    ch, str : char;
    size : sizes;

BEGIN {CASE statements}
    i := 3;  ch := 'b';
    size := medium;
    even := -990; odd := -999; prime := 0;

    CASE i+1 OF
        1:       j := i;
        4:       j := 4*i;
        5, 2, 3: j := 523*i;
    END;

    writeln('j = ', j);

    CASE ch OF
        'c', 'b' : str := 'p';
        'a'      : str := 'q'
    END;

    writeln('str = ''', str, '''');

    CASE size OF
        small:  str := 's';
        medium: str := 'm';
        large:  str := 'l';
    END;

    writeln('str = ''', str, '''');

    FOR i := -5 TO 15 DO BEGIN
        CASE i OF
            2: prime := i;
            -4, -2, 0, 4, 6, 8, 10, 12: even := i;
            -3, -1, 1, 3, 5, 7, 9, 11:  CASE i OF
                                            -3, -1, 1, 9:   odd := i;
                                            2, 3, 5, 7, 11: prime := i;
                                        END
        END;

        writeln('i = ', i, ', even = ', even, ', odd = ', odd,
                ', prime = ', prime);
    END
END.
