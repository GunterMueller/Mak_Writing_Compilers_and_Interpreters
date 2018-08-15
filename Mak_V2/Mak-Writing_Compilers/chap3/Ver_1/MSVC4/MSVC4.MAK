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

ALL : "$(OUTDIR)\msvc4.exe"

CLEAN : 
	-@erase "$(INTDIR)\Buffer.obj"
	-@erase "$(INTDIR)\Common.obj"
	-@erase "$(INTDIR)\Error.obj"
	-@erase "$(INTDIR)\Parser.obj"
	-@erase "$(INTDIR)\Scanner.obj"
	-@erase "$(INTDIR)\Token.obj"
	-@erase "$(INTDIR)\Tokeniz1.obj"
	-@erase "$(OUTDIR)\msvc4.exe"

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
	"$(INTDIR)\Buffer.obj" \
	"$(INTDIR)\Common.obj" \
	"$(INTDIR)\Error.obj" \
	"$(INTDIR)\Parser.obj" \
	"$(INTDIR)\Scanner.obj" \
	"$(INTDIR)\Token.obj" \
	"$(INTDIR)\Tokeniz1.obj"

"$(OUTDIR)\msvc4.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "$(OUTDIR)\msvc4.exe"

CLEAN : 
	-@erase "$(INTDIR)\Buffer.obj"
	-@erase "$(INTDIR)\Common.obj"
	-@erase "$(INTDIR)\Error.obj"
	-@erase "$(INTDIR)\Parser.obj"
	-@erase "$(INTDIR)\Scanner.obj"
	-@erase "$(INTDIR)\Token.obj"
	-@erase "$(INTDIR)\Tokeniz1.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\msvc4.exe"
	-@erase "$(OUTDIR)\msvc4.ilk"
	-@erase "$(OUTDIR)\msvc4.pdb"

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
	"$(INTDIR)\Buffer.obj" \
	"$(INTDIR)\Common.obj" \
	"$(INTDIR)\Error.obj" \
	"$(INTDIR)\Parser.obj" \
	"$(INTDIR)\Scanner.obj" \
	"$(INTDIR)\Token.obj" \
	"$(INTDIR)\Tokeniz1.obj"

"$(OUTDIR)\msvc4.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

SOURCE="\Book1#2\Programs\Prog3-1\Tokeniz1.cpp"
DEP_CPP_TOKEN=\
	"..\buffer.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\token.h"\
	

"$(INTDIR)\Tokeniz1.obj" : $(SOURCE) $(DEP_CPP_TOKEN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog3-1\Common.cpp"
DEP_CPP_COMMO=\
	"..\common.h"\
	"..\misc.h"\
	

"$(INTDIR)\Common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog3-1\Error.cpp"

"$(INTDIR)\Error.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog3-1\Parser.cpp"
DEP_CPP_PARSE=\
	"..\buffer.h"\
	"..\common.h"\
	"..\misc.h"\
	"..\parser.h"\
	"..\scanner.h"\
	"..\token.h"\
	

"$(INTDIR)\Parser.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog3-1\Scanner.cpp"
DEP_CPP_SCANN=\
	"..\buffer.h"\
	"..\misc.h"\
	"..\scanner.h"\
	"..\token.h"\
	

"$(INTDIR)\Scanner.obj" : $(SOURCE) $(DEP_CPP_SCANN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog3-1\Token.cpp"
DEP_CPP_TOKEN_=\
	"..\buffer.h"\
	"..\misc.h"\
	"..\token.h"\
	

"$(INTDIR)\Token.obj" : $(SOURCE) $(DEP_CPP_TOKEN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE="\Book1#2\Programs\Prog3-1\Buffer.cpp"
DEP_CPP_BUFFE=\
	"..\buffer.h"\
	"..\common.h"\
	"..\misc.h"\
	

"$(INTDIR)\Buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
