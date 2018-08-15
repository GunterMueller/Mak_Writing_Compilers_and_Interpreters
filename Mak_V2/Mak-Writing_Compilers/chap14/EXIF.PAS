PROGRAM exif (output);

CONST
    one = 1;
    two = 2;

BEGIN
    IF one <> two THEN writeln('true');
    IF one =  two THEN writeln('true');

    IF one <> two THEN writeln('true')
		  ELSE writeln('false');
    IF one =  two THEN writeln('true')
		  ELSE writeln('false');
END.
