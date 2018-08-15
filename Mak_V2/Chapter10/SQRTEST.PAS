PROGRAM sqrtest(output);

VAR
    x, s : real;

BEGIN
    x := 3.464102;
    s := sqr(x);
    writeln(x:12:6, s:12:6, x*x:12:6);
END.