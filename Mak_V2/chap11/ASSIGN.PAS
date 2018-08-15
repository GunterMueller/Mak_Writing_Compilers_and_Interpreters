PROGRAM assign (output);

CONST
    ten = 10;
    pi  = 3.14159;

TYPE
    subrange = 5..ten;
    enum = (zero, one, two, three, four, five);
    arr  = ARRAY [enum] OF real;
    rec  = RECORD
	       i : integer;
	       z : RECORD
		       x : real;
		       a : arr;
		   END;
	   END;
    arec = ARRAY [12..15] OF rec;

VAR
    i, j, k : subrange;
    e1, e2  : enum;
    x, y, z : real;
    p, q    : boolean;
    ch      : char;
    r1, r2  : rec;
    a1, a2  : arec;
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

    r1.i := 7;
    r1.z.x := 3.14;
    r1.z.a[two] := +2.2;
    i := r1.i;
    x := r1.z.x;
    x := r1.z.a[two];

    a1[14].i := 7;
    a1[14].z.x := 3.14;
    a1[14].z.a[two] := +2.2;
    i := a1[14].i;
    x := a1[14].z.x;
    x := a1[14].z.a[two];

    ch := 'x';
    string1 := 'Hello, you';
    string2 := string1;
    p := string1 = string2;
    string1[ten] := ch;
    ch := string1[1];
    p := string1 = string2;
    p := string1 > string2;

    r2 := r1;
    i := r2.i;
    x := r2.z.x;
    x := r2.z.a[two];

    a2 := a1;
    i := a2[14].i;
    x := a2[14].z.x;
    x := a2[14].z.a[two];
END.
