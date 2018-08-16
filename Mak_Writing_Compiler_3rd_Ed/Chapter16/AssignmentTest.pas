PROGRAM AssignmentTest;

VAR
    tempF, tempC : 0..200;
    ratio, fahrenheit, centigrade : real;
    freezing : boolean;

BEGIN
    tempF := 72;
    tempC := 25;
    ratio := 5.0/9.0;

    fahrenheit := tempF;
    centigrade := (fahrenheit - 32)*ratio;

    centigrade := tempC;
    fahrenheit := 32 + centigrade/ratio;

    freezing := fahrenheit < 32
END.
