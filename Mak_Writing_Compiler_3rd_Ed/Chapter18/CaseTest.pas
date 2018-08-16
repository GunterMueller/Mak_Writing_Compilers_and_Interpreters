PROGRAM CaseTest;

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
        -8:      j := 8*i;
        5, 7, 4: j := 574*i;
    END;

    CASE ch OF
        'c', 'b' : str := 'p';
        'a'      : str := 'q'
    END;

    CASE size OF
        small:  str := 's';
        medium: str := 'm';
        large:  str := 'l';
    END;

    FOR i := -5 TO 15 DO BEGIN
        CASE i OF
            2: prime := i;
            -4, -2, 0, 4, 6, 8, 10, 12: even := i;
            -3, -1, 1, 3, 5, 7, 9, 11:  CASE i OF
                                            -3, -1, 1, 9:   odd := i;
                                            2, 3, 5, 7, 11: prime := i;
                                        END
        END;
    END
END.
