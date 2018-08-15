PROGRAM RoutineErrors (input, output);

CONST
    five = 5;

TYPE
    enum = (alpha, beta, gamma);
    arr = ARRAY [1..five] OF real;

VAR
    e, k : enum;
    i, m : integer;
    a : arr;
    v, y : real;
    t : boolean;

PROCEDURE proc(j : integer; VAR x, y, z : real;
               VAR p : (alpha, beta, gamma); ch : char);

    VAR
        i : integer;

    BEGIN
        i := 7*k;
        x := func(x);
    END;

FUNCTION forwarded(m : integer; VAR t : real) : 1..10; forward;

FUNCTION func(VAR x : real; i : real; n : integer) : arr;

    VAR
        z : real;
        p, q : boolean;

    PROCEDURE nested(VAR n, m : integer);

        VAR
            a, b, c : integer;

        BEGIN {nested}
            p := forwarded(9, 2.0);
        END {nested};

    BEGIN {func}
        x := i*z - func(v, -3.15159, five) + n/m;
        func := x;
    END {func};

FUNCTION forwarded(b : boolean) : real;

    VAR
        n : integer;

    BEGIN
        forwarded := true;
    END;

BEGIN {RoutinesTest}
    proc(i, -7 + m, a[m], v, y, a, t, 'r');
END {RoutinesTest}.
