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
!MESSAGE NMAKE /f "Msvc4.mak" CFG="msvc4 - Win32 Debug"
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
# PROP Target_Last_Scanned "msvc4 - Win32 Debug"
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

ALL : ".\Release\Msvc4.exe"

CLEAN : 
	-@erase ".\Release\Buffer.obj"
	-@erase ".\Release\Common.obj"
	-@erase ".\Release\Error.obj"
	-@erase ".\Release\Exec.obj"
	-@erase ".\Release\Execexpr.obj"
	-@erase ".\Release\Execrtn.obj"
	-@erase ".\Release\Execstd.obj"
	-@erase ".\Release\Execstmt.obj"
	-@erase ".\Release\Icode.obj"
	-@erase ".\Release\Msvc4.exe"
	-@erase ".\Release\Parsdecl.obj"
	-@erase ".\Release\Parser.obj"
	-@erase ".\Release\Parsexpr.obj"
	-@erase ".\Release\Parsrtn1.obj"
	-@erase ".\Release\Parsrtn2.obj"
	-@erase ".\Release\Parsstd.obj"
	-@erase ".\Release\Parsstmt.obj"
	-@erase ".\Release\Parstyp1.obj"
	-@erase ".\Release\Parstyp2.obj"
	-@erase ".\Release\Run.obj"
	-@erase ".\Release\Scanner.obj"
	-@erase ".\Release\Symtab.obj"
	-@erase ".\Release\Tknnum.obj"
	-@erase ".\Release\Tknstrsp.obj"
	-@erase ".\Release\Tknword.obj"
	-@erase ".\Release\Tracer.obj"
	-@erase ".\Release\Types.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/Msvc4.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Msvc4.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/Msvc4.pdb" /machine:I386 /out:"$(OUTDIR)/Msvc4.exe" 
LINK32_OBJS= \
	".\Release\Buffer.obj" \
	".\Release\Common.obj" \
	".\Release\Error.obj" \
	".\Release\Exec.obj" \
	".\Release\Execexpr.obj" \
	".\Release\Execrtn.obj" \
	".\Release\Execstd.obj" \
	".\Release\Execstmt.obj" \
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
	".\Release\Run.obj" \
	".\Release\Scanner.obj" \
	".\Release\Symtab.obj" \
	".\Release\Tknnum.obj" \
	".\Release\Tknstrsp.obj" \
	".\Release\Tknword.obj" \
	".\Release\Tracer.obj" \
	".\Release\Types.obj"

".\Release\Msvc4.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : ".\Debug\Msvc4.exe"

CLEAN : 
	-@erase ".\Debug\Buffer.obj"
	-@erase ".\Debug\Common.obj"
	-@erase ".\Debug\Error.obj"
	-@erase ".\Debug\Exec.obj"
	-@erase ".\Debug\Execexpr.obj"
	-@erase ".\Debug\Execrtn.obj"
	-@erase ".\Debug\Execstd.obj"
	-@erase ".\Debug\Execstmt.obj"
	-@erase ".\Debug\Icode.obj"
	-@erase ".\Debug\Msvc4.exe"
	-@erase ".\Debug\Msvc4.ilk"
	-@erase ".\Debug\Msvc4.pdb"
	-@erase ".\Debug\Parsdecl.obj"
	-@erase ".\Debug\Parser.obj"
	-@erase ".\Debug\Parsexpr.obj"
	-@erase ".\Debug\Parsrtn1.obj"
	-@erase ".\Debug\Parsrtn2.obj"
	-@erase ".\Debug\Parsstd.obj"
	-@erase ".\Debug\Parsstmt.obj"
	-@erase ".\Debug\Parstyp1.obj"
	-@erase ".\Debug\Parstyp2.obj"
	-@erase ".\Debug\Run.obj"
	-@erase ".\Debug\Scanner.obj"
	-@erase ".\Debug\Symtab.obj"
	-@erase ".\Debug\Tknnum.obj"
	-@erase ".\Debug\Tknstrsp.obj"
	-@erase ".\Debug\Tknword.obj"
	-@erase ".\Debug\Tracer.obj"
	-@erase ".\Debug\Types.obj"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\vc40.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/Msvc4.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Msvc4.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/Msvc4.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Msvc4.exe" 
LINK32_OBJS= \
	".\Debug\Buffer.obj" \
	".\Debug\Common.obj" \
	".\Debug\Error.obj" \
	".\Debug\Exec.obj" \
	".\Debug\Execexpr.obj" \
	".\Debug\Execrtn.obj" \
	".\Debug\Execstd.obj" \
	".\Debug\Execstmt.obj" \
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
	".\Debug\Run.obj" \
	".\Debug\Scanner.obj" \
	".\Debug\Symtab.obj" \
	".\Debug\Tknnum.obj" \
	".\Debug\Tknstrsp.obj" \
	".\Debug\Tknword.obj" \
	".\Debug\Tracer.obj" \
	".\Debug\Types.obj"

".\Debug\Msvc4.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE="\Book1#2\Programs\Prog10-1\Types.cpp"
DEP_CPP_TYPES=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Common.cpp"
DEP_CPP_COMMO=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Error.cpp"
DEP_CPP_ERROR=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Exec.cpp"
DEP_CPP_EXEC_=\
	"..\..\..\Programs\Prog10-1\backend.h"\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\exec.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Exec.obj" : $(SOURCE) $(DEP_CPP_EXEC_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Exec.obj" : $(SOURCE) $(DEP_CPP_EXEC_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog10-1\Execexpr.cpp"
DEP_CPP_EXECE=\
	"..\..\..\Programs\Prog10-1\backend.h"\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\exec.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Execexpr.obj" : $(SOURCE) $(DEP_CPP_EXECE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Execexpr.obj" : $(SOURCE) $(DEP_CPP_EXECE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog10-1\Execrtn.cpp"
DEP_CPP_EXECR=\
	"..\..\..\Programs\Prog10-1\backend.h"\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\exec.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Execrtn.obj" : $(SOURCE) $(DEP_CPP_EXECR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Execrtn.obj" : $(SOURCE) $(DEP_CPP_EXECR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog10-1\Execstd.cpp"
DEP_CPP_EXECS=\
	"..\..\..\Programs\Prog10-1\backend.h"\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\exec.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Execstd.obj" : $(SOURCE) $(DEP_CPP_EXECS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Execstd.obj" : $(SOURCE) $(DEP_CPP_EXECS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog10-1\Execstmt.cpp"
DEP_CPP_EXECST=\
	"..\..\..\Programs\Prog10-1\backend.h"\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\exec.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Execstmt.obj" : $(SOURCE) $(DEP_CPP_EXECST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Execstmt.obj" : $(SOURCE) $(DEP_CPP_EXECST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog10-1\Icode.cpp"
DEP_CPP_ICODE=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Parsdecl.cpp"
DEP_CPP_PARSD=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\parser.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Parser.cpp"
DEP_CPP_PARSE=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\parser.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Parsexpr.cpp"
DEP_CPP_PARSEX=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\parser.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Parsrtn1.cpp"
DEP_CPP_PARSR=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\parser.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Parsrtn2.cpp"
DEP_CPP_PARSRT=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\parser.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Parsstd.cpp"
DEP_CPP_PARSS=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\parser.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Parsstmt.cpp"
DEP_CPP_PARSST=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\parser.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Parstyp1.cpp"
DEP_CPP_PARST=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\parser.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Parstyp2.cpp"
DEP_CPP_PARSTY=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\parser.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Run.cpp"
DEP_CPP_RUN_C=\
	"..\..\..\Programs\Prog10-1\backend.h"\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\exec.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\parser.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Run.obj" : $(SOURCE) $(DEP_CPP_RUN_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Run.obj" : $(SOURCE) $(DEP_CPP_RUN_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog10-1\Scanner.cpp"
DEP_CPP_SCANN=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Symtab.cpp"
DEP_CPP_SYMTA=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Tknnum.cpp"
DEP_CPP_TKNNU=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Tknstrsp.cpp"
DEP_CPP_TKNST=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Tknword.cpp"
DEP_CPP_TKNWO=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog10-1\Tracer.cpp"
DEP_CPP_TRACE=\
	"..\..\..\Programs\Prog10-1\backend.h"\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\exec.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Tracer.obj" : $(SOURCE) $(DEP_CPP_TRACE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Tracer.obj" : $(SOURCE) $(DEP_CPP_TRACE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog10-1\Buffer.cpp"
DEP_CPP_BUFFE=\
	"..\..\..\Programs\Prog10-1\buffer.h"\
	"..\..\..\Programs\Prog10-1\common.h"\
	"..\..\..\Programs\Prog10-1\icode.h"\
	"..\..\..\Programs\Prog10-1\misc.h"\
	"..\..\..\Programs\Prog10-1\scanner.h"\
	"..\..\..\Programs\Prog10-1\symtab.h"\
	"..\..\..\Programs\Prog10-1\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
