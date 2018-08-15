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
RSC=rc.exe
CPP=cl.exe

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
	-@erase ".\Release\Debug.obj"
	-@erase ".\Release\Debugger.obj"
	-@erase ".\Release\Error.obj"
	-@erase ".\Release\Exec.obj"
	-@erase ".\Release\Execexpr.obj"
	-@erase ".\Release\Execrtn.obj"
	-@erase ".\Release\Execstd.obj"
	-@erase ".\Release\Execstmt.obj"
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
	-@erase ".\Release\Tracer.obj"
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
	".\Release\Debug.obj" \
	".\Release\Debugger.obj" \
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
	".\Release\Scanner.obj" \
	".\Release\Symtab.obj" \
	".\Release\Tknnum.obj" \
	".\Release\Tknstrsp.obj" \
	".\Release\Tknword.obj" \
	".\Release\Tracer.obj" \
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
	-@erase ".\Debug\Debug.obj"
	-@erase ".\Debug\Debugger.obj"
	-@erase ".\Debug\Error.obj"
	-@erase ".\Debug\Exec.obj"
	-@erase ".\Debug\Execexpr.obj"
	-@erase ".\Debug\Execrtn.obj"
	-@erase ".\Debug\Execstd.obj"
	-@erase ".\Debug\Execstmt.obj"
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
	-@erase ".\Debug\Tracer.obj"
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
	".\Debug\Debug.obj" \
	".\Debug\Debugger.obj" \
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
	".\Debug\Scanner.obj" \
	".\Debug\Symtab.obj" \
	".\Debug\Tknnum.obj" \
	".\Debug\Tknstrsp.obj" \
	".\Debug\Tknword.obj" \
	".\Debug\Tracer.obj" \
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

SOURCE="\Book1#2\Programs\Prog11-1\Types.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Buffer.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Common.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Debug.cpp"
DEP_CPP_DEBUG=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\common.h"\
	"..\exec.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog11-1\Debugger.cpp"
DEP_CPP_DEBUGG=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\common.h"\
	"..\exec.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

!IF  "$(CFG)" == "msvc4 - Win32 Release"


".\Release\Debugger.obj" : $(SOURCE) $(DEP_CPP_DEBUGG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"


".\Debug\Debugger.obj" : $(SOURCE) $(DEP_CPP_DEBUGG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog11-1\Error.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Exec.cpp"
DEP_CPP_EXEC_=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\exec.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog11-1\Execexpr.cpp"
DEP_CPP_EXECE=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\common.h"\
	"..\exec.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog11-1\Execrtn.cpp"
DEP_CPP_EXECR=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\exec.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog11-1\Execstd.cpp"
DEP_CPP_EXECS=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\exec.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog11-1\Execstmt.cpp"
DEP_CPP_EXECST=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\exec.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog11-1\Icode.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Parsdecl.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Parser.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Parsexpr.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Parsrtn1.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Parsrtn2.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Parsstd.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Parsstmt.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Parstyp1.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Parstyp2.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Scanner.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Symtab.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Tknnum.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Tknstrsp.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Tknword.cpp"
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

SOURCE="\Book1#2\Programs\Prog11-1\Tracer.cpp"
DEP_CPP_TRACE=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\common.h"\
	"..\exec.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

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

SOURCE="\Book1#2\Programs\Prog11-1\Backend.cpp"

!IF  "$(CFG)" == "msvc4 - Win32 Release"

DEP_CPP_BACKE=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	

".\Release\Backend.obj" : $(SOURCE) $(DEP_CPP_BACKE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msvc4 - Win32 Debug"

DEP_CPP_BACKE=\
	"..\backend.h"\
	"..\buffer.h"\
	"..\icode.h"\
	"..\misc.h"\
	"..\scanner.h"\
	"..\symtab.h"\
	"..\token.h"\
	
NODEP_CPP_BACKE=\
	"..\-"\
	"..\pSaveTokenString"\
	

".\Debug\Backend.obj" : $(SOURCE) $(DEP_CPP_BACKE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
