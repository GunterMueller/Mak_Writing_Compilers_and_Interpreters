PROGRAM hanoi (input, output);

TYPE
    pole   = (left, middle, right);
    number = 1..10;

VAR
    disks : number;

PROCEDURE move (n : number; source, aux, dest : pole);

    PROCEDURE printmove;

	PROCEDURE printpole (p : pole);

	    BEGIN
		CASE p OF
		    left   : write('left  ');
		    middle : write('middle');
		    right  : write('right ');
		END;
	    END;

	BEGIN
	    write('Move a disk from ');  printpole(source);
	    write(' to ');               printpole(dest);
	    writeln;
	END;

    BEGIN
        IF n = 1 THEN printmove
        ELSE BEGIN
            move(n-1, source, dest, aux);
            printmove;
            move(n-1, aux, source, dest);
        END;
    END;

BEGIN
    REPEAT
        writeln;
        write('Number of disks (1-10, 1 to stop)? ');
    	read(disks);
        IF (disks > 1) AND (disks <= 10) THEN BEGIN
            writeln;
            writeln('For ', disks:0, ' disks, the required moves are:');
            writeln;
            move(disks, left, middle, right);
        END
        ELSE IF disks <> 1 THEN BEGIN
            writeln('*** Invalid number of disks.');
        END
    UNTIL disks = 1;
END.
