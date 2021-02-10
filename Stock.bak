# Microsoft Visual C++ Generated NMAKE File, Format Version 20051
# MSVCPRJ: version 2.0.4135
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "stock.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/stock.exe $(OUTDIR)/stock.bsc $(MTL_TLBS)

$(OUTDIR) : 
    if not exist $(OUTDIR) mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /ML /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /ML /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp$(OUTDIR)/"stock.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /d "NDEBUG" /d "_AFXDLL"
# SUBTRACT BASE RSC /l 0x0
# ADD RSC /d "NDEBUG"
# SUBTRACT RSC /l 0x0
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"STOCK.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"stock.bsc" 

$(OUTDIR)/stock.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30.lib mfco30.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows\
 /INCREMENTAL:no /PDB:$(OUTDIR)/"stock.pdb" /MACHINE:I386\
 /OUT:$(OUTDIR)/"stock.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/STOCK.res \
	$(INTDIR)/MEM.OBJ \
	$(INTDIR)/TEXTFILE.OBJ \
	$(INTDIR)/UTILITY.OBJ \
	$(INTDIR)/STOCK.OBJ

$(OUTDIR)/stock.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/stock.exe $(OUTDIR)/stock.bsc $(MTL_TLBS)

$(OUTDIR) : 
    if not exist $(OUTDIR) mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /ML /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /ML /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp$(OUTDIR)/"stock.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"stock.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /d "_DEBUG" /d "_AFXDLL"
# SUBTRACT BASE RSC /l 0x0
# ADD RSC /d "_DEBUG"
# SUBTRACT RSC /l 0x0
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"STOCK.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_SBRS= \
	
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"stock.bsc" 

$(OUTDIR)/stock.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib mfc30d.lib mfco30d.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
LINK32_FLAGS=user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"stock.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"stock.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/STOCK.res \
	$(INTDIR)/MEM.OBJ \
	$(INTDIR)/TEXTFILE.OBJ \
	$(INTDIR)/UTILITY.OBJ \
	$(INTDIR)/STOCK.OBJ

$(OUTDIR)/stock.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\STOCK.RC
DEP_STOCK=\
	.\stock.ico\
	.\ids.h

$(INTDIR)/STOCK.res :  $(SOURCE)  $(DEP_STOCK) $(INTDIR)
   $(RSC) /l 0x409 /fo$(INTDIR)/"STOCK.res" /d "NDEBUG"  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MEM.C
DEP_MEM_C=\
	.\mem.h

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/MEM.OBJ :  $(SOURCE)  $(DEP_MEM_C) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/MEM.OBJ :  $(SOURCE)  $(DEP_MEM_C) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TEXTFILE.C
DEP_TEXTF=\
	.\mem.h\
	.\textfile.h

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/TEXTFILE.OBJ :  $(SOURCE)  $(DEP_TEXTF) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/TEXTFILE.OBJ :  $(SOURCE)  $(DEP_TEXTF) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\UTILITY.C
DEP_UTILI=\
	.\mem.h\
	.\utility.h

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/UTILITY.OBJ :  $(SOURCE)  $(DEP_UTILI) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/UTILITY.OBJ :  $(SOURCE)  $(DEP_UTILI) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\STOCK.C
DEP_STOCK_=\
	.\ids.h\
	.\mem.h\
	.\textfile.h\
	.\utility.h

!IF  "$(CFG)" == "Win32 Release"

$(INTDIR)/STOCK.OBJ :  $(SOURCE)  $(DEP_STOCK_) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

$(INTDIR)/STOCK.OBJ :  $(SOURCE)  $(DEP_STOCK_) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
