PROGRAM WhileTest;

VAR
    number : integer;
    root : real;

BEGIN {Calculate the square root of 2 using Newton's method.}
    number := 2;
    root := number;

    WHILE root*root - number > 0.000001 DO BEGIN
        root := (number/root + root)/2
    END
END.
