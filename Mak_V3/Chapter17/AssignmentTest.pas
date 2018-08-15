PROGRAM AssignmentTest;

VAR
    ratio, fahrenheit, centigrade : real;
    freezing : boolean;

BEGIN
    ratio := 5.0/9.0;

    fahrenheit := 72;
    centigrade := (fahrenheit - 32)*ratio;

    centigrade := 25;
    fahrenheit := 32 + centigrade/ratio;

    freezing := fahrenheit < 32
END.
