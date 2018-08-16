PROGRAM WolfIsland (input, output);

{   Wolf Island is a simulation of a 9 x 9 island of wolves and rabbits.
    The wolves eat rabbits, and the rabbits eat grass.  Their initial
    locations are:

			. . . . . . . . .
			. W . . . . . W .
			. . . . . . . . .
			. . . r r r . . .
			. . . r r r . . .
			. . . r r r . . .
			. . . . . . . . .
			. W . . . . . W .
			. . . . . . . . .

    A wolf or rabbit can move up, down, left, or right into an adjacent
    location.  Diagonal moves are not allowed.

    Time is measured in discrete time units.  Wolves reproduce every
    12 time units, and rabbits every 5 units.  An animal reproduces
    by splitting into two.

    Each wolf starts out with 6 food units and loses 1 every time unit.
    A wolf gains 6 food units by eating a rabbit.  It starves to death
    if it reaches 0 food units.  Since there's always enough grass to
    eat, rabbits don't worry about food units.

    The order of events from time unit T-1 to T is:

    (1)	For each wolf:

	Lose a food unit.  Die if 0 food units and remove.

	Eat a rabbit if there is one in an adjacent location by moving
	into the rabbit's location.  Increase the wolf's food units
	by 6 and remove the rabbit.

	Otherwise, randomly choose to move into an adjacent empty
	location, or stay put.

	If wolf reproduction time (T = 12,24,36,...), split and leave
	behind an offspring in the previous location.  Each split wolf
	has half (DIV 2) the food units.  If there was no move, the
	baby was stillborn, but the food units are still halved.

    (2)	For each rabbit:

	Randomly choose to move into an adjacent empty, or stay put.

	If rabbit reproduction time (T = 5,10,15,...), split and leave
	behind an offspring in the previous location.  If there was no
	move, the baby was stillborn.

	The simulation ends when all the wolves are dead or all the
	rabbits are eaten.

	The island is printed at times T = 0,1,2,3,4,5,6,7,8,9,10,
	15,20,25,30,...,80.  A message is printed whenever a wolf is
	born or dies, and whenever a rabbit is born or is eaten.
}

CONST
    size            = 9;	{size of island}
    max             = 10;       {size plus border}
    wolfreprotime   = 12;       {wolf reproduction period}
    rabbitreprotime = 5;        {rabbit reproduction period}
    rabbitfoodunits = 6;        {rabbit food unit worth to wolf}
    initfoodunits   = 6;        {wolf's initial food units}
    maxprinttimes   = 50;       {max. no. times to print island}

TYPE
    posint   = 0..32767;
    index    = 0..max;          {index range of island matrix}

    contents = (wolf, rabbit, newwolf, newrabbit, empty, border);
	{Contents each each island location.  Each time a wolf or
	 rabbit moves, newwolf or newrabbit is initially placed in
	 the new location.  This prevents a wolf or rabbit from
	 being processed again in its new location during the same
	 time period.}

VAR
    island     : ARRAY [index, index] OF contents;
				{Wolf Island with border}
    foodunits  : ARRAY [1..size, 1..size] OF posint;
				{wolves' food unit matrix}
    printtimes : ARRAY [1..maxprinttimes] OF posint;
				{times to print island}

    numwolves, numrabbits : posint;	{no. of wolves and rabbits}
    numprinttimes         : posint;     {no. of print times}
    t      	          : posint;     {time}
    xpt  : 1..maxprinttimes;     	{print times index}
    seed : posint;               	{random number seed}

    rowoffset : ARRAY [0..4] OF -1..+1;
    coloffset : ARRAY [0..4] OF -1..+1;
	{Row and column offsets.  When added to the current row and
	 column of a wolf's or rabbit's location, gives the row and
	 column of the same or an adjacent location.}


PROCEDURE Initialize;

    {Initialize all arrays.}

    VAR
	i        : posint;
	row, col : index;

    BEGIN

	{Initialize the island and wolf food matrices.}
	FOR i := 0 TO max DO BEGIN
	    island[0,   i] := border;
	    island[max, i] := border;
	    island[i, 0]   := border;
	    island[i, max] := border;
	END;
	FOR row := 1 TO size DO BEGIN
	    FOR col := 1 TO size DO BEGIN
		island[row, col]    := empty;
		foodunits[row, col] := 0;
	    END;
	END;

	{Place wolves on the island.}
	read(numwolves);
	FOR i := 1 TO numwolves DO BEGIN
	    read(row, col);
	    island[row, col]    := wolf;
	    foodunits[row, col] := initfoodunits;
	END;

	{Place rabbits on the island.}
	read(numrabbits);
	FOR i := 1 TO numrabbits DO BEGIN
	    read(row, col);
	    island[row, col] := rabbit;
	END;

	{Read print times.}
	read(numprinttimes);
	FOR i := 1 TO numprinttimes DO BEGIN
	    read(printtimes[i]);
	END;

	{Initialize the row and column offsets for moves.}
	rowoffset[0] :=  0;  coloffset[0] :=  0;    {stay put}
	rowoffset[1] := -1;  coloffset[1] :=  0;    {up}
	rowoffset[2] :=  0;  coloffset[2] := -1;    {left}
	rowoffset[3] :=  0;  coloffset[3] := +1;    {right}
	rowoffset[4] := +1;  coloffset[4] :=  0;    {down}
    END {Initialize};


FUNCTION random (limit : posint) : posint;

    {Return a random integer from 0..limit-1.}

    CONST
	multiplier = 21;
	increment  = 77;
	divisor    = 1024;

    BEGIN
	seed   := (seed*multiplier + increment) MOD divisor;
	random := (seed*limit) DIV divisor;
    END {random};


PROCEDURE NewLocation (creature : contents;
		       oldrow, oldcol : index;
		       VAR newrow, newcol : index);

    {Find a new location for the creature currently at
     island[oldrow, oldcol].}


    VAR
	adj  : 0..4;	    {adjacent locations index}
	what : contents;    {contents of location}
	done : boolean;

    BEGIN
	done := false;

	{A wolf first tries to eat a rabbit.
	 Check adjacent locations.}
	IF creature = wolf THEN BEGIN
	    adj := 0;
	    REPEAT
		adj := adj + 1;
		newrow := oldrow + rowoffset[adj];
		newcol := oldcol + coloffset[adj];
		what   := island[newrow, newcol];
		done   := what = rabbit;
	    UNTIL done OR (adj = 4);
	END;

	{Move randomly into an adjacent location or stay put.}
	IF NOT done THEN BEGIN
	    REPEAT
		adj := random(5);
		newrow := oldrow + rowoffset[adj];
		newcol := oldcol + coloffset[adj];
		what   := island[newrow, newcol];
	    UNTIL    (what = empty)
		  OR ((newrow = oldrow) AND (newcol = oldcol));
	END;
    END {NewLocation};


PROCEDURE ProcessWolf (oldrow, oldcol : index);

    {Process the wolf located at island[oldrow, oldcol].}

    VAR
	newrow, newcol : index;	    {new row and column}
	moved : boolean;            {true iff wolf moved}

    BEGIN

	{Lose a food unit.}
	foodunits[oldrow, oldcol] := foodunits[oldrow, oldcol] - 1;

	IF foodunits[oldrow, oldcol] = 0 THEN BEGIN

	    {Die of starvation.}
	    island[oldrow, oldcol] := empty;
	    numwolves := numwolves - 1;
	    writeln('t =', t:4, ' : Wolf died at ',
		    '[', oldrow:1, ', ', oldcol:1, ']');
	END
	ELSE BEGIN

	    {Move to adjacent location, or stay put.}
	    NewLocation(wolf, oldrow, oldcol, newrow, newcol);
	    moved := (newrow <> oldrow) OR (newcol <> oldcol);

	    IF moved THEN BEGIN

		{If there's a rabbit there, eat it.}
		IF island[newrow, newcol] = rabbit THEN BEGIN
		    foodunits[oldrow, oldcol] :=
			foodunits[oldrow, oldcol] + rabbitfoodunits;
		    numrabbits := numrabbits - 1;
		    writeln('t =', t:4, ' : Rabbit eaten at ',
			    '[', newrow:1, ', ', newcol:1, ']');
		END;

		{Set new (or same) location.}
		island[newrow, newcol] := newwolf;
		island[oldrow, oldcol] := empty;
		foodunits[newrow, newcol] := foodunits[oldrow, oldcol];
		foodunits[oldrow, oldcol] := 0;
	    END;

	    {Wolf reproduction time?}
	    IF     ((t MOD wolfreprotime) = 0)
	       AND (foodunits[newrow, newcol] > 1) THEN BEGIN
		foodunits[newrow, newcol] :=
		    foodunits[newrow, newcol] DIV 2;

		{If moved, then leave behind an offspring.}
		IF moved THEN BEGIN
		    island[oldrow, oldcol] := newwolf;
		    foodunits[oldrow, oldcol] :=
			foodunits[newrow, newcol];
		    numwolves := numwolves + 1;
		    writeln('t =', t:4, ' : Wolf born at ',
			    '[', oldrow:1, ', ', oldcol:1, ']');
		END;
	    END;
	END;
    END {ProcessWolf};


PROCEDURE ProcessRabbit (oldrow, oldcol : index);

    {Process the rabbit located at island[oldrow, oldcol].}

    VAR
	newrow, newcol : index;	    {new row and column}
	moved : boolean;            {true iff rabbit moved}

    BEGIN

	{Move to adjacent location, or stay put.}
	NewLocation(rabbit, oldrow, oldcol, newrow, newcol);
	moved := (newrow <> oldrow) OR (newcol <> oldcol);
	IF moved THEN BEGIN
	    island[newrow, newcol] := newrabbit;
	    island[oldrow, oldcol] := empty;
	END;

	{Rabbit reproduction time?}
	IF (t MOD rabbitreprotime) = 0 THEN BEGIN

	    {If moved, then leave behind an offspring.}
	    IF moved THEN BEGIN
		island[oldrow, oldcol] := newrabbit;
		numrabbits := numrabbits + 1;
		writeln('t =', t:4, ' : Rabbit born at ',
			'[', oldrow:1, ', ', oldcol:1, ']');
	    END;
	END;
    END {ProcessRabbit};


PROCEDURE EventsOccur;

    {Perform the events that occur for each time unit.}

    VAR
	row, col : index;

    BEGIN

	{Scan for wolves and process each one in turn.}
	FOR row := 1 TO size DO BEGIN
	    FOR col := 1 TO size DO BEGIN
		IF island[row, col] = wolf THEN BEGIN
		    ProcessWolf(row, col);
		END;
	    END;
	END;


	{Scan for rabbits and process each one in turn.}
	FOR row := 1 TO size DO BEGIN
	    FOR col := 1 TO size DO BEGIN
		IF island[row, col] = rabbit THEN BEGIN
		    ProcessRabbit(row, col);
		END;
	    END;
	END;
    END {EventsOccur};


PROCEDURE PrintIsland;

    {Print the island.}

    VAR
	row, col : index;
	cnts     : contents;

    BEGIN
	writeln;
	writeln('t =', t:4, ' : Wolf Island');
	writeln;

	FOR row := 1 TO size DO BEGIN
	    write(' ':10);
	    FOR col := 1 TO size DO BEGIN
		cnts := island[row, col];
		IF      cnts = empty  THEN write('. ')
		ELSE IF cnts = wolf   THEN write('W ')
		ELSE IF cnts = rabbit THEN write('r ')
	    END;
	    writeln;
	END;
    END {PrintIsland};


PROCEDURE ResetIsland;

    {Reset the island by setting each newwolf to wolf
     and each newrabbit to rabbit.}

    VAR
	row, col : index;

    BEGIN
	FOR row := 1 TO size DO BEGIN
	    FOR col := 1 TO size DO BEGIN
		IF island[row, col] = newwolf THEN BEGIN
		    island[row, col] := wolf;
		END
		ELSE IF island[row, col] = newrabbit THEN BEGIN
		    island[row, col] := rabbit;
		END;
	    END;
	END;
    END {ResetIsland};


BEGIN {WolfIsland}

    Initialize;

    t   := 0;
    xpt := 1;
    read(seed);

    PrintIsland;

    {Loop once per time period.}
    REPEAT
	writeln;

	t := t + 1;
	EventsOccur;
	ResetIsland;

	{Time to print the island?}
	IF t = printtimes[xpt] THEN BEGIN
	    PrintIsland;
	    xpt := xpt + 1;
	END;
    UNTIL (numwolves = 0) OR (numrabbits = 0)
	  OR (xpt > numprinttimes);

    PrintIsland;

END {WolfIsland}.
