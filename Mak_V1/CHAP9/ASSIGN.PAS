PROGRAM assign (output);

CONST
    ten = 10;
    pi  = 3.14159;

TYPE
    subrange = 5..ten;
    enum = (zero, one, two, three, four, five);
    arr = ARRAY [enum] OF real;
    rec = RECORD
	      i : integer;
	      z : RECORD
		      x  : real;
		      a1 : arr;
		  END;
	  END;
    arc = ARRAY [12..15] OF rec;

VAR
    i, j, k : subrange;
    e1, e2  : enum;
    x, y, z : real;
    p, q    : boolean;
    ch      : char;
    r       : rec;
    a       : arc;
    string1, string2 : ARRAY [1..ten] OF char;

BEGIN
    i := 7;
    j := ten DIV 2;
    k := 4*(i - j);
    e1 := three;
    e2 := e1;
    x := pi/7.2;
    y := x + 3;
    z := x - ten + y;
    p := true;
    q := NOT (x = y) AND p;

    r.i := 7;
    r.z.x := 3.14;
    r.z.a1[two] := +2.2;
    i := r.i;
    x := r.z.x;
    x := r.z.a1[two];

    a[14].i := 7;
    a[14].z.x := 3.14;
    a[14].z.a1[two] := +2.2;
    i := a[14].i;
    x := a[14].z.x;
    x := a[14].z.a1[two];

    ch := 'x';
    string1 := 'Hello, you';
    string2 := string1;
    p := string1 = string2;
    string1[ten] := ch;
    ch := string1[1];
    p := string1 = string2;
    p := string1 > string2;
END.


