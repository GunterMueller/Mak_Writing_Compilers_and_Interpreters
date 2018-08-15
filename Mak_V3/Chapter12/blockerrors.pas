PROGRAM BlockErrors;

CONST
    Seven =  7;
    Ten   = 10;

TYPE
    range1 = 0..ten;
    range2 = 'a'..'q';
    range3 = range1;

    enum1 = (a, b, c, d, e);
    enum2 = enum1;

    range4 = b..d;

    arr1 = ARRAY [range1] OF real;
    arr2 = ARRAY [(alpha, beta, gamma)] OF range2;
    arr3 = ARRAY [enum2] OF arr1;
    arr4 = ARRAY [range3] OF (foo, bar, baz);
    arr5 = ARRAY [range1] OF ARRAY[range2] OF ARRAY[c..e] OF enum2;
    arr6 = ARRAY [range1, range2, c..e] OF enum2;

    rec7 = RECORD
               i : integer;
               r : real;
               b1, b2 : boolean;
               c : char
           END;

    arr8 = ARRAY [range2] OF RECORD
                                 fldi : integer;
                                 fldr : rec7;
                                 flda : ARRAY[range4] OF range2;
                             END;

VAR
    var1 : arr1;  var5 : arr5;
    var2 : arr2;  var6 : arr6;
    var3 : arr3;  var7 : rec7;
    var4 : arr4;  var8 : arr8;

    var9 : RECORD
               b   : boolean;
               rec : RECORD
                         fld1 : arr1;
                         fldb : boolean;
                         fldr : real;
                         fld6 : arr6;
                         flda : ARRAY [enum1, range1] OF arr8;
                     END;
               a : ARRAY [1..5] OF boolean;
           END;

BEGIN
    var2[a] := 3.14;
    var1[var7.i] := var9.rec.flda['e', ten]['q'].fldr;

    IF var9.rec.fldr THEN var2[beta] := seven;

    CASE var5[seven, 'm', d]  OF
        foo:      var3[e] := 12;
        bar, baz: var3[b] := var1.rec.fldb;
    END;

    REPEAT
        var7[3] := a;
    UNTIL var6[3, 'a', c] + var5[4]['f', d];

    var9.rec.flda[b][0, 'm', foo].flda[d] := 'p';
END.
