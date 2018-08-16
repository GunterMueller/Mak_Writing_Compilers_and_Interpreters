PROGRAM RoutinesTest(input, output);

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

PROCEDURE proc(j, k : integer; VAR x, y, z : real; VAR v : arr;
               VAR p : boolean; ch : char);

    VAR
        i : real;

    BEGIN
        i := 7*k;
        x := 3.14;
    END;

FUNCTION forwarded(m : integer; VAR t : real) : real; forward;

FUNCTION func(VAR x : real; i : real; n : integer) : real;

    VAR
        z : real;
        p, q : boolean;

    PROCEDURE nested(VAR n, m : integer);

        CONST
            ten = 10;

        TYPE
            subrange = five..ten;

        VAR
            a, b, c : integer;
            s : subrange;

        PROCEDURE deeply;

            VAR
                w : real;

            BEGIN
                w := i;
                nested(a, m);
                w := forwarded(b, w);
            END;

        BEGIN {nested}
            s := m;
            deeply;
            a := s;
        END {nested};

    BEGIN {func}
        p := true;
        q := false;
        x := i*z - func(v, -3.15159, five) + n/m;
        func := x;
    END {func};

FUNCTION forwarded;

    VAR
        n : integer;

    BEGIN
        forwarded := n*y - t;
    END;

BEGIN {RoutinesTest}
    e := beta;
    proc(i, -7 + m, a[m], v, y, a, t, 'r');
END {RoutinesTest}.
