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
    i := 7;			writeln(i);
    j := ten DIV 2;		writeln(j);
    k := 4*(i - j);             writeln(k);

    x := pi/7.2;		writeln(x);
    y := x + 3;			writeln(y);
    z := x - ten + y;		writeln(z);
    p := true;			writeln(p);
    q := NOT (x = y) AND p;	writeln(q);

    r.i := 7;			writeln(r.i);
    r.z.x := 3.14;              writeln(r.z.x);
    r.z.a1[two] := +2.2;        writeln(r.z.a1[two]);
    i := r.i;                   writeln(i);
    x := r.z.x;                 writeln(x);
    x := r.z.a1[two];           writeln(x);

    a[14].i := 7;               writeln(a[14].i);
    a[14].z.x := 3.14;          writeln(a[14].z.x);
    a[14].z.a1[two] := +2.2;    writeln(a[14].z.a1[two]);
    i := a[14].i;               writeln(i);
    x := a[14].z.x;             writeln(x);
    x := a[14].z.a1[two];       writeln(x);

    ch := 'x';                  writeln(ch);
    string1 := 'Hello, you';    writeln(string1);
    string2 := string1;         writeln(string2);
    p := string1 = string2;     writeln(p);
    string1[ten] := ch;         writeln(string1);
    ch := string1[1];           writeln(ch);
    p := string1 = string2;     writeln(p);
    p := string1 > string2;     writeln(p);
END.


