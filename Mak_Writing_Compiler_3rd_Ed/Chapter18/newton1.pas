PROGRAM Newton1;

CONST
    epsilon = 1e-6;

TYPE
    positive = 0..32767;

VAR
    number : positive;

FUNCTION root(x : real) : real;
    VAR
        r : real;

    BEGIN
        r := 1;

        r := (x/r + r)/2;
        r := (x/r + r)/2;
        r := (x/r + r)/2;
        r := (x/r + r)/2;
        r := (x/r + r)/2;
        r := (x/r + r)/2;
        r := (x/r + r)/2;
        r := (x/r + r)/2;

        root := r;
    END;

PROCEDURE print(n : integer; root : real);
    BEGIN
        writeln('The square root of ', n:4, ' is ', root:8:4);
    END;

BEGIN
    writeln;
    write('Enter a new number: ');
    read(number);
    print(number, root(number));

    writeln;
    write('Enter a new number: ');
    read(number);
    print(number, root(number));
END.
