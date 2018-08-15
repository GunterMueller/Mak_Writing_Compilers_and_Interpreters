# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=msvc4 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to msvc4 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "msvc4 - Win32 Release" && "$(CFG)" != "msvc4 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "msvc4.mak" CFG="msvc4 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "msvc4 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "msvc4 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "msvc4 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\Release\msvc4.exe"

CLEAN : 
	-@erase ".\Release\Backend.obj"
	-@erase ".\Release\Buffer.obj"
	-@erase ".\Release\Common.obj"
	-@erase ".\Release\Compile.obj"
	-@erase ".\Release\Emitasm.obj"
	-@erase ".\Release\Emitcode.obj"
	-@erase ".\Release\Emitexpr.obj"
	-@erase ".\Release\Emitrtn.obj"
	-@erase ".\Release\Emitsrc.obj"
	-@erase ".\Release\Emitstd.obj"
	-@erase ".\Release\Emitstmt.obj"
	-@erase ".\Release\Error.obj"
	-@erase ".\Release\Icode.obj"
	-@erase ".\Release\msvc4.exe"
	-@erase ".\Release\Parsdecl.obj"
	-@erase ".\Release\Parser.obj"
	-@erase ".\Release\Parsexpr.obj"
	-@erase ".\Release\Parsrtn1.obj"
	-@erase ".\Release\Parsrtn2.obj"
	-@erase ".\Release\Parsstd.obj"
	-@erase ".\Release\Parsstmt.obj"
	-@erase ".\Release\Parstyp1.obj"
	-@erase ".\Release\Parstyp2.obj"
	-@erase ".\Release\Scanner.obj"
	-@erase ".\Release\Symtab.obj"
	-@erase ".\Release\Tknnum.obj"
	-@erase ".\Release\Tknstrsp.obj"
	-@erase ".\Release\Tknword.obj"
	-@erase ".\Release\Types.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/msvc4.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/msvc4.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/msvc4.pdb" /machine:I386 /out:"$(OUTDIR)/msvc4.exe" 
LINK32_OBJS= \
	".\Release\Backend.obj" \
	".\Release\Buffer.obj" \
	".\Release\Common.obj" \
	".\Release\Compile.obj" \
	".\Release\Emitasm.obj" \
	".\Release\Emitcode.obj" \
	".\Release\Emitexpr.obj" \
	".\Release\Emitrtn.obj" \
	".\Release\Emitsrc.obj" \
	".\Release\Emitstd.obj" \
	".\Release\Emitstmt.obj" \
	".\Release\Error.obj" \
	".\Release\Icode.obj" \
	".\Release\Parsdecl.obj" \
	".\Release\Parser.obj" \
	".\Release\Parsexpr.obj" \
	".\Release\Parsrtn1.obj" \
	".\Release\Parsrtn2.obj" \
	".\Release\Parsstd.obj" \
	".\Release\Parsstmt.obj" \
	".\Release\Parstyp1.obj" \
	".\Release\Parstyp2.obj" \
	".\Release\Scanner.obj" \
	".\Release\Symtab.obj" \
	".\Release\Tknnum.obj" \
	".\Release\Tknstrsp.obj" \
	".\Release\Tknword.obj" \
	".\Release\Types.obj"

".\Release\msvc4.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\Debug\msvc4.exe"

CLEAN : 
	-@erase ".\Debug\Backend.obj"
	-@erase ".\Debug\Buffer.obj"
	-@erase ".\Debug\Common.obj"
	-@erase ".\Debug\Compile.obj"
	-@erase ".\Debug\Emitasm.obj"
	-@erase ".\Debug\Emitcode.obj"
	-@erase ".\Debug\Emitexpr.obj"
	-@erase ".\Debug\Emitrtn.obj"
	-@erase ".\Debug\Emitsrc.obj"
	-@erase ".\Debug\Emitstd.obj"
	-@erase ".\Debug\Emitstmt.obj"
	-@erase ".\Debug\Error.obj"
	-@erase ".\Debug\Icode.obj"
	-@erase ".\Debug\msvc4.exe"
	-@erase ".\Debug\msvc4.ilk"
	-@erase ".\Debug\msvc4.pdb"
	-@erase ".\Debug\Parsdecl.obj"
	-@erase ".\Debug\Parser.obj"
	-@erase ".\Debug\Parsexpr.obj"
	-@erase ".\Debug\Parsrtn1.obj"
	-@erase ".\Debug\Parsrtn2.obj"
	-@erase ".\Debug\Parsstd.obj"
	-@erase ".\Debug\Parsstmt.obj"
	-@erase ".\Debug\Parstyp1.obj"
	-@erase ".\Debug\Parstyp2.obj"
	-@erase ".\Debug\Scanner.obj"
	-@erase ".\Debug\Symtab.obj"
	-@erase ".\Debug\Tknnum.obj"
	-@erase ".\Debug\Tknstrsp.obj"
	-@erase ".\Debug\Tknword.obj"
	-@erase ".\Debug\Types.obj"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\vc40.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/msvc4.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/msvc4.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/msvc4.pdb" /debug /machine:I386 /out:"$(OUTDIR)/msvc4.exe" 
LINK32_OBJS= \
	".\Debug\Backend.obj" \
	".\Debug\Buffer.obj" \
	".\Debug\Common.obj" \
	".\Debug\Compile.obj" \
	".\Debug\Emitasm.obj" \
	".\Debug\Emitcode.obj" \
	".\Debug\Emitexpr.obj" \
	".\Debug\Emitrtn.obj" \
	".\Debug\Emitsrc.obj" \
	".\Debug\Emitstd.obj" \
	".\Debug\Emitstmt.obj" \
	".\Debug\Error.obj" \
	".\Debug\Icode.obj" \
	".\Debug\Parsdecl.obj" \
	".\Debug\Parser.obj" \
	".\Debug\Parsexpr.obj" \
	".\Debug\Parsrtn1.obj" \
	".\Debug\Parsrtn2.obj" \
	".\Debug\Parsstd.obj" \
	".\Debug\Parsstmt.obj" \
	".\Debug\Parstyp1.obj" \
	".\Debug\Parstyp2.obj" \
	".\Debug\Scanner.obj" \
	".\Debug\Symtab.obj" \
	".\Debug\Tknnum.obj" \
	".\Debug\Tknstrsp.obj" \
	".\Debug\Tknword.obj" \
	".\Debug\Types.obj"

".\Debug\msvc4.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "msvc4 - Win32 Release"
# Name "msvc4 - Win32 Debug"

!IF  "$(CFG)" == "msvc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Types.cpp"
DEP_CPP_TYPES=\
	"..\buffer.h"\
	"..\misc.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Types.obj" : $(SOURCE) $(DEP_CPP_TYPES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Types.obj" : $(SOURCE) $(DEP_CPP_TYPES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Buffer.cpp"
DEP_CPP_BUFFE=\
	"..\buffer.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Common.cpp"
DEP_CPP_COMMO=\
	"..\buffer.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Compile.cpp"
DEP_CPP_COMPI=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\codegen.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Compile.obj" : $(SOURCE) $(DEP_CPP_COMPI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Compile.obj" : $(SOURCE) $(DEP_CPP_COMPI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Emitasm.cpp"
DEP_CPP_EMITA=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\codegen.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Emitasm.obj" : $(SOURCE) $(DEP_CPP_EMITA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Emitasm.obj" : $(SOURCE) $(DEP_CPP_EMITA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Emitcode.cpp"
DEP_CPP_EMITC=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\codegen.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Emitcode.obj" : $(SOURCE) $(DEP_CPP_EMITC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Emitcode.obj" : $(SOURCE) $(DEP_CPP_EMITC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Emitexpr.cpp"
DEP_CPP_EMITE=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\codegen.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Emitexpr.obj" : $(SOURCE) $(DEP_CPP_EMITE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Emitexpr.obj" : $(SOURCE) $(DEP_CPP_EMITE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Emitrtn.cpp"
DEP_CPP_EMITR=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\codegen.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Emitrtn.obj" : $(SOURCE) $(DEP_CPP_EMITR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Emitrtn.obj" : $(SOURCE) $(DEP_CPP_EMITR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Emitsrc.cpp"
DEP_CPP_EMITS=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\codegen.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Emitsrc.obj" : $(SOURCE) $(DEP_CPP_EMITS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Emitsrc.obj" : $(SOURCE) $(DEP_CPP_EMITS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Emitstd.cpp"
DEP_CPP_EMITST=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\codegen.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Emitstd.obj" : $(SOURCE) $(DEP_CPP_EMITST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Emitstd.obj" : $(SOURCE) $(DEP_CPP_EMITST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Emitstmt.cpp"
DEP_CPP_EMITSTM=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\codegen.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Emitstmt.obj" : $(SOURCE) $(DEP_CPP_EMITSTM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Emitstmt.obj" : $(SOURCE) $(DEP_CPP_EMITSTM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Error.cpp"
DEP_CPP_ERROR=\
	"..\buffer.h"\
	"..\misc.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Error.obj" : $(SOURCE) $(DEP_CPP_ERROR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Error.obj" : $(SOURCE) $(DEP_CPP_ERROR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Icode.cpp"
DEP_CPP_ICODE=\
	"..\buffer.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Icode.obj" : $(SOURCE) $(DEP_CPP_ICODE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Icode.obj" : $(SOURCE) $(DEP_CPP_ICODE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Parsdecl.cpp"
DEP_CPP_PARSD=\
	"..\buffer.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Parsdecl.obj" : $(SOURCE) $(DEP_CPP_PARSD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Parsdecl.obj" : $(SOURCE) $(DEP_CPP_PARSD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Parser.cpp"
DEP_CPP_PARSE=\
	"..\buffer.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Parser.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Parser.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Parsexpr.cpp"
DEP_CPP_PARSEX=\
	"..\buffer.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Parsexpr.obj" : $(SOURCE) $(DEP_CPP_PARSEX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Parsexpr.obj" : $(SOURCE) $(DEP_CPP_PARSEX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Parsrtn1.cpp"
DEP_CPP_PARSR=\
	"..\buffer.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Parsrtn1.obj" : $(SOURCE) $(DEP_CPP_PARSR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Parsrtn1.obj" : $(SOURCE) $(DEP_CPP_PARSR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Parsrtn2.cpp"
DEP_CPP_PARSRT=\
	"..\buffer.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Parsrtn2.obj" : $(SOURCE) $(DEP_CPP_PARSRT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Parsrtn2.obj" : $(SOURCE) $(DEP_CPP_PARSRT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Parsstd.cpp"
DEP_CPP_PARSS=\
	"..\buffer.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Parsstd.obj" : $(SOURCE) $(DEP_CPP_PARSS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Parsstd.obj" : $(SOURCE) $(DEP_CPP_PARSS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Parsstmt.cpp"
DEP_CPP_PARSST=\
	"..\buffer.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Parsstmt.obj" : $(SOURCE) $(DEP_CPP_PARSST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Parsstmt.obj" : $(SOURCE) $(DEP_CPP_PARSST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Parstyp1.cpp"
DEP_CPP_PARST=\
	"..\buffer.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Parstyp1.obj" : $(SOURCE) $(DEP_CPP_PARST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Parstyp1.obj" : $(SOURCE) $(DEP_CPP_PARST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Parstyp2.cpp"
DEP_CPP_PARSTY=\
	"..\buffer.h"\
	"..\common.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Parstyp2.obj" : $(SOURCE) $(DEP_CPP_PARSTY) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Parstyp2.obj" : $(SOURCE) $(DEP_CPP_PARSTY) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Scanner.cpp"
DEP_CPP_SCANN=\
	"..\buffer.h"\
	"..\misc.h"\
	"..\scanner.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Scanner.obj" : $(SOURCE) $(DEP_CPP_SCANN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Scanner.obj" : $(SOURCE) $(DEP_CPP_SCANN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Symtab.cpp"
DEP_CPP_SYMTA=\
	"..\buffer.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Symtab.obj" : $(SOURCE) $(DEP_CPP_SYMTA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Symtab.obj" : $(SOURCE) $(DEP_CPP_SYMTA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Tknnum.cpp"
DEP_CPP_TKNNU=\
	"..\buffer.h"\
	"..\misc.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Tknnum.obj" : $(SOURCE) $(DEP_CPP_TKNNU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Tknnum.obj" : $(SOURCE) $(DEP_CPP_TKNNU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Tknstrsp.cpp"
DEP_CPP_TKNST=\
	"..\buffer.h"\
	"..\misc.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Tknstrsp.obj" : $(SOURCE) $(DEP_CPP_TKNST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Tknstrsp.obj" : $(SOURCE) $(DEP_CPP_TKNST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Tknword.cpp"
DEP_CPP_TKNWO=\
	"..\buffer.h"\
	"..\misc.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Tknword.obj" : $(SOURCE) $(DEP_CPP_TKNWO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Tknword.obj" : $(SOURCE) $(DEP_CPP_TKNWO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog14-1\Backend.cpp"
DEP_CPP_BACKE=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Backend.obj" : $(SOURCE) $(DEP_CPP_BACKE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Backend.obj" : $(SOURCE) $(DEP_CPP_BACKE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
