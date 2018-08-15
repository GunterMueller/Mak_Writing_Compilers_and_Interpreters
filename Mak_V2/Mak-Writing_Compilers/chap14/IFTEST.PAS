PROGRAM iftest (output);

BEGIN
    IF true  THEN writeln('true');
    IF false THEN writeln('true');

    IF true  THEN writeln('true')
	     ELSE writeln('false');
    IF false THEN writeln('true')
	     ELSE writeln('false');
END.

