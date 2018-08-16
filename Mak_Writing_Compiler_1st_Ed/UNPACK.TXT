echo off

echo ***
echo *** Unpacking source files
echo ***

pkunzip -d -o sources.zip
del sources.zip
del pkunzip.exe

echo ***
echo *** Copying duplicate files
echo ***

echo chap13\code.h to chap14
copy chap13\code.h chap14

echo chap2\common.h to chap3-8,13,14
for %%d in (chap3 chap4 chap5 chap6 chap7 chap8 chap13 chap14) do copy chap2\common.h %%d

echo chap9\common.h to chap10,11
for %%d in (chap10 chap11) do copy chap9\common.h %%d

echo chap2\error.h to chap3-8,13,14
for %%d in (chap3 chap4 chap5 chap6 chap7 chap8 chap13 chap14) do copy chap2\error.h %%d

echo chap9\error.h to chap10,11
for %%d in (chap10 chap11) do copy chap9\error.h %%d

echo chap10\exec.h to chap11
copy chap10\exec.h chap11

echo chap6\parser.h to chap7-11,13,14
for %%d in (chap7 chap8 chap9 chap10 chap11 chap13 chap14) do copy chap6\parser.h %%d

echo chap2\scanner.h to chap3,4
for %%d in (chap3 chap4) do copy chap2\scanner.h %%d

echo chap5\scanner.h to chap6-11,13,14
for %%d in (chap6 chap7 chap8 chap9 chap10 chap11 chap13 chap14) do copy chap5\scanner.h %%d

echo chap3\symtab.h to chap4,5
for %%d in (chap4 chap5) do copy chap3\symtab.h %%d

echo chap7\symtab.h to chap8-11, 13,14
for %%d in (chap8 chap9 chap10 chap11 chap13 chap14) do copy chap7\symtab.h %%d

echo chap9\routine.c to chap10
copy chap9\routine.c chap10

echo chap13\routine.c to chap14
copy chap13\routine.c chap14

echo chap7\standard.c to chap8-11
for %%d in (chap8 chap9 chap10 chap11) do copy chap7\standard.c %%d

echo chap7\decl.c to chap8
copy chap7\decl.c chap8

echo chap9\decl.c to chap10,11
for %%d in (chap10 chap11) do copy chap9\decl.c %%d

echo chap13\decl.c to chap14
copy chap13\decl.c chap14

echo chap10\stmt.c to chap11
copy chap10\stmt.c chap11

echo chap8\expr.c to chap9-11
for %%d in (chap9 chap10 chap11) do copy chap8\expr.c %%d

echo chap13\expr.c to chap14
copy chap13\expr.c chap14

echo chap3\symtab.c to chap4,5
for %%d in (chap4 chap5) do copy chap3\symtab.c %%d

echo chap7\symtab.c to chap8-14
for %%d in (chap8 chap9 chap10 chap11 chap13 chap14) do copy chap7\symtab.c %%d

echo chap2\scanner.c to chap3,4
for %%d in (chap3 chap4) do copy chap2\scanner.c %%d

echo chap5\scanner.c to chap6,7
for %%d in (chap6 chap7) do copy chap5\scanner.c %%d

echo chap8\scanner.c to chap9-11
for %%d in (chap9 chap10 chap11) do copy chap8\scanner.c %%d

echo chap13\scanner.c to chap14
copy chap13\scanner.c chap14

echo chap2\error.c to chap3-8,13,14
for %%d in (chap3 chap4 chap5 chap6 chap7 chap8 chap13 chap14) do copy chap2\error.c %%d

echo chap9\error.c to chap10
copy chap9\error.c chap10

echo chap13\emitasm.c to chap14
copy chap13\emitasm.c chap14

echo chap13\emitcode.c to chap14
copy chap13\emitcode.c chap14

echo chap10\executil.c to chap11
copy chap10\executil.c chap11

echo chap10\execstd.c to chap11
copy chap10\execstd.c chap11

echo chap10\execstmt.c to chap11
copy chap10\execstmt.c chap11

echo chap9\execexpr.c to chap10,11
for %%d in (chap10 chap11) do copy chap9\execexpr.c %%d

echo chap1\newton.pas to chap3,10,11,14
for %%d in (chap10 chap11 chap14) do copy chap1\newton.pas %%d

echo chap2\hello.pas to chap3
copy chap2\hello.pas chap3

echo chap8\translat.pas to chap10,14
for %%d in (chap10 chap14) do copy chap8\translat.pas %%d

echo chap10\sieve.pas to chap14
copy chap10\sieve.pas chap14

echo chap10\translat.in to chap14
copy chap10\translat.in chap14

echo chap10\wolf.pas to chap14
copy chap10\wolf.pas chap14

echo chap10\wolf.in to chap14
copy chap10\wolf.in chap14

echo chap10\xref.pas to chap14
copy chap10\xref.pas chap14
