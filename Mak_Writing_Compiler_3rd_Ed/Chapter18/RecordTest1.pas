PROGRAM RecordTest1;

TYPE
    String2  = ARRAY [0..1 ] OF char;
    String5  = ARRAY [0..4 ] OF char;
    String8  = ARRAY [0..7 ] OF char;
    String16 = ARRAY [0..15] OF char;

    AddressRec = RECORD
                     street : String16;
                     city   : String16;
                     state  : String2;
                     zip    : String5;
                 END;

    PersonRec =  RECORD
                     firstName : String16;
                     lastName  : String16;
                     age       : integer;
                     address   : AddressRec;
                     phones    : ARRAY [0..1] OF String8;
                 END;

VAR
    john : PersonRec;
    mary : PersonRec;

PROCEDURE print1(VAR pers : PersonRec);

    VAR
        age  : integer;
        addr : AddressRec;
        phs  : ARRAY [0..1] OF String8;

    BEGIN
        age := pers.age;

        addr.street := pers.address.street;
        addr.city   := pers.address.city;
        addr.state  := pers.address.state;
        addr.zip    := pers.address.zip;

        phs[0] := pers.phones[0];
        phs[1] := pers.phones[1];

        writeln;
        writeln('First name: ', pers.firstName);
        writeln(' Last name: ', pers.lastName);
        writeln('       Age: ', age);
        writeln('    Street: ', addr.street);
        writeln('      City: ', addr.city);
        writeln('     State: ', addr.state);
        writeln('       ZIP: ', addr.zip);
        writeln('  Phone #1: ', phs[0]);
        writeln('  Phone #2: ', phs[1]);
    END;

PROCEDURE print2(pers : PersonRec);

    VAR
        age  : integer;
        addr : AddressRec;
        phs  : ARRAY [0..1] OF String8;

    BEGIN
        writeln;
        writeln('First name: ', pers.firstName);
        writeln(' Last name: ', pers.lastName);
        writeln('       Age: ', pers.age);
        writeln('    Street: ', pers.address.street);
        writeln('      City: ', pers.address.city);
        writeln('     State: ', pers.address.state);
        writeln('       ZIP: ', pers.address.zip);
        writeln('  Phone #1: ', pers.phones[0]);
        writeln('  Phone #2: ', pers.phones[1]);
    END;

BEGIN
    john.firstName := 'John';
    john.lastName  := 'Doe';
    john.age := 24;
    john.address.street := '1680 25th Street';
    john.address.city   := 'San Pablo';
    john.address.state  := 'CALIFORNIA';
    john.address.zip    := '94806';
    john.phones[0]      := '111-1111';
    john.phones[1]      := '222-2222';

    mary.firstName := 'Mary';
    mary.lastName  := 'Jane';
    mary.age := 22;
    mary.address.street := '4899 Bela Drive ';
    mary.address.city   := 'San Jose';
    mary.address.state  := 'CALIFORNIA';
    mary.address.zip    := '95129';
    mary.phones[0]      := '333-3333';
    mary.phones[1]      := '444-4444';
    print1(john);
    print2(mary);
END.
