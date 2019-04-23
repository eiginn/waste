# Microsoft Developer Studio Project File - Name="PadLockSL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PadLockSL - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PadLockSL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PadLockSL.mak" CFG="PadLockSL - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PadLockSL - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PadLockSL - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /O1 /I "AES\include" /I "$(QTDIR)\include" /I "c:\qt3\qt3\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "QTUI" /D "_WIN32" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib winmm.lib winspool.lib $(QTDIR)\lib\qt-mt301.lib $(QTDIR)\lib\qtmain.lib /nologo /subsystem:windows /machine:IX86 /out:"Release/PadLockSL.exe"

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MDd /W3 /Gm /ZI /Od /I "AES\include" /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "QTUI" /D "_WIN32" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FR /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804
# ADD RSC /l 0x409 /fo"Resource.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib winmm.lib winspool.lib $(QTDIR)\lib\qt-mt301d.lib $(QTDIR)\lib\qtmaind.lib /nologo /subsystem:windows /debug /machine:IX86 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PadLockSL - Win32 Release"
# Name "PadLockSL - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=AES\src\aescrypt.c
# End Source File
# Begin Source File

SOURCE=AES\src\aeskey.c
# End Source File
# Begin Source File

SOURCE=AES\src\aestab.c
# End Source File
# Begin Source File

SOURCE=asyncdns.cpp
# End Source File
# Begin Source File

SOURCE=AES\src\c5p.c
# End Source File
# Begin Source File

SOURCE=common.cpp
# End Source File
# Begin Source File

SOURCE=config.cpp
# End Source File
# Begin Source File

SOURCE=connection.cpp
# End Source File
# Begin Source File

SOURCE=filedb.cpp
# End Source File
# Begin Source File

SOURCE=listen.cpp
# End Source File
# Begin Source File

SOURCE=load_config.cpp
# End Source File
# Begin Source File

SOURCE=m_chat.cpp
# End Source File
# Begin Source File

SOURCE=m_file.cpp
# End Source File
# Begin Source File

SOURCE=m_keydist.cpp
# End Source File
# Begin Source File

SOURCE=m_lcaps.cpp
# End Source File
# Begin Source File

SOURCE=m_ping.cpp
# End Source File
# Begin Source File

SOURCE=m_search.cpp
# End Source File
# Begin Source File

SOURCE=m_upload.cpp
# End Source File
# Begin Source File

SOURCE=rsa\md5c.c
# End Source File
# Begin Source File

SOURCE=mqueue.cpp
# End Source File
# Begin Source File

SOURCE=mqueuelist.cpp
# End Source File
# Begin Source File

SOURCE=netkern.cpp
# End Source File
# Begin Source File

SOURCE=netq.cpp
# End Source File
# Begin Source File

SOURCE=rsa\nn.c
# End Source File
# Begin Source File

SOURCE=prefdlgs.cpp
# End Source File
# Begin Source File

SOURCE=preference.cpp
# End Source File
# Begin Source File

SOURCE=prefinterface.cpp
# End Source File
# Begin Source File

SOURCE=rsa\prime.c
# End Source File
# Begin Source File

SOURCE=qt_srvmain.cpp
# End Source File
# Begin Source File

SOURCE=qulf_dialog.cpp
# End Source File
# Begin Source File

SOURCE=rsa\r_keygen.c
# End Source File
# Begin Source File

SOURCE=rsa\r_random.c
# End Source File
# Begin Source File

SOURCE=.\Resource.rc
# End Source File
# Begin Source File

SOURCE=rsa\rsa.c
# End Source File
# Begin Source File

SOURCE=sha.cpp
# End Source File
# Begin Source File

SOURCE=srchwnd.cpp
# End Source File
# Begin Source File

SOURCE=tag.cpp
# End Source File
# Begin Source File

SOURCE=ui_listview.cpp
# End Source File
# Begin Source File

SOURCE=ui_treeview.cpp
# End Source File
# Begin Source File

SOURCE=util.cpp
# End Source File
# Begin Source File

SOURCE=vaste_chat.cpp
# End Source File
# Begin Source File

SOURCE=vaste_idd_net.cpp
# End Source File
# Begin Source File

SOURCE=vaste_idd_search.cpp
# End Source File
# Begin Source File

SOURCE=vaste_idd_xfers.cpp
# End Source File
# Begin Source File

SOURCE=vaste_kernal.cpp
# End Source File
# Begin Source File

SOURCE=vaste_keygen.cpp
# End Source File
# Begin Source File

SOURCE=vaste_mainwindow.cpp
# End Source File
# Begin Source File

SOURCE=vq_chat.cpp
# End Source File
# Begin Source File

SOURCE=xfers.cpp
# End Source File
# Begin Source File

SOURCE=xferwnd.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=AES\include\aes.h
# End Source File
# Begin Source File

SOURCE=AES\include\aesopt.h
# End Source File
# Begin Source File

SOURCE=asyncdns.h
# End Source File
# Begin Source File

SOURCE=AES\include\C5P.h
# End Source File
# Begin Source File

SOURCE=common.h
# End Source File
# Begin Source File

SOURCE=connection.h
# End Source File
# Begin Source File

SOURCE=filedb.h
# End Source File
# Begin Source File

SOURCE=rsa\global.h
# End Source File
# Begin Source File

SOURCE=itemlist.h
# End Source File
# Begin Source File

SOURCE=itemstack.h
# End Source File
# Begin Source File

SOURCE=m_chat.h
# End Source File
# Begin Source File

SOURCE=m_file.h
# End Source File
# Begin Source File

SOURCE=m_keydist.h
# End Source File
# Begin Source File

SOURCE=m_lcaps.h
# End Source File
# Begin Source File

SOURCE=m_ping.h
# End Source File
# Begin Source File

SOURCE=m_search.h
# End Source File
# Begin Source File

SOURCE=m_upload.h
# End Source File
# Begin Source File

SOURCE=rsa\md5.h
# End Source File
# Begin Source File

SOURCE=mqueue.h
# End Source File
# Begin Source File

SOURCE=mqueuelist.h
# End Source File
# Begin Source File

SOURCE=netkern.h
# End Source File
# Begin Source File

SOURCE=netq.h
# End Source File
# Begin Source File

SOURCE=rsa\nn.h
# End Source File
# Begin Source File

SOURCE=platform.h
# End Source File
# Begin Source File

SOURCE=.\prefdlgs.h

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\prefdlgs.h
InputName=prefdlgs

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\prefdlgs.h
InputName=prefdlgs

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\preference.h

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\preference.h
InputName=preference

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\preference.h
InputName=preference

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=prefinterface.h
# End Source File
# Begin Source File

SOURCE=rsa\prime.h
# End Source File
# Begin Source File

SOURCE=.\qulf_dialog.h

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\qulf_dialog.h
InputName=qulf_dialog

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\qulf_dialog.h
InputName=qulf_dialog

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=rsa\r_random.h
# End Source File
# Begin Source File

SOURCE=rsa\rsa.h
# End Source File
# Begin Source File

SOURCE=rsa\rsaref.h
# End Source File
# Begin Source File

SOURCE=sha.h
# End Source File
# Begin Source File

SOURCE=shbuf.h
# End Source File
# Begin Source File

SOURCE=sockets.h
# End Source File
# Begin Source File

SOURCE=srchwnd.h
# End Source File
# Begin Source File

SOURCE=tag.h
# End Source File
# Begin Source File

SOURCE=ui_listview.h
# End Source File
# Begin Source File

SOURCE=ui_treeview.h
# End Source File
# Begin Source File

SOURCE=util.h
# End Source File
# Begin Source File

SOURCE=.\vaste_chat.h

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\vaste_chat.h
InputName=vaste_chat

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\vaste_chat.h
InputName=vaste_chat

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vaste_idd_net.h

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\vaste_idd_net.h
InputName=vaste_idd_net

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\vaste_idd_net.h
InputName=vaste_idd_net

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vaste_idd_search.h

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\vaste_idd_search.h
InputName=vaste_idd_search

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\vaste_idd_search.h
InputName=vaste_idd_search

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vaste_idd_xfers.h

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\vaste_idd_xfers.h
InputName=vaste_idd_xfers

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\vaste_idd_xfers.h
InputName=vaste_idd_xfers

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vaste_mainwindow.h

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\vaste_mainwindow.h
InputName=vaste_mainwindow

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\vaste_mainwindow.h
InputName=vaste_mainwindow

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=vq_chat.h
# End Source File
# Begin Source File

SOURCE=xfers.h
# End Source File
# Begin Source File

SOURCE=xferwnd.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ico\viasecim.ico
# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=idd_about.cpp
# End Source File
# Begin Source File

SOURCE=idd_about.h
# End Source File
# Begin Source File

SOURCE=idd_chatchan.cpp
# End Source File
# Begin Source File

SOURCE=idd_chatchan.h
# End Source File
# Begin Source File

SOURCE=idd_chatpriv.cpp
# End Source File
# Begin Source File

SOURCE=idd_chatpriv.h
# End Source File
# Begin Source File

SOURCE=idd_chatroomcreate.cpp
# End Source File
# Begin Source File

SOURCE=idd_chatroomcreate.h
# End Source File
# Begin Source File

SOURCE=idd_help.cpp
# End Source File
# Begin Source File

SOURCE=idd_help.h
# End Source File
# Begin Source File

SOURCE=idd_net.cpp
# End Source File
# Begin Source File

SOURCE=idd_net.h
# End Source File
# Begin Source File

SOURCE=idd_recv.cpp
# End Source File
# Begin Source File

SOURCE=idd_recv.h
# End Source File
# Begin Source File

SOURCE=idd_recvq.cpp
# End Source File
# Begin Source File

SOURCE=idd_recvq.h
# End Source File
# Begin Source File

SOURCE=idd_search.cpp
# End Source File
# Begin Source File

SOURCE=idd_search.h
# End Source File
# Begin Source File

SOURCE=idd_send.cpp
# End Source File
# Begin Source File

SOURCE=idd_send.h
# End Source File
# Begin Source File

SOURCE=idd_xfers.cpp
# End Source File
# Begin Source File

SOURCE=idd_xfers.h
# End Source File
# Begin Source File

SOURCE=main_window.cpp
# End Source File
# Begin Source File

SOURCE=main_window.h
# End Source File
# Begin Source File

SOURCE=moc_idd_about.cpp
# End Source File
# Begin Source File

SOURCE=moc_idd_chatchan.cpp
# End Source File
# Begin Source File

SOURCE=moc_idd_chatpriv.cpp
# End Source File
# Begin Source File

SOURCE=moc_idd_chatroomcreate.cpp
# End Source File
# Begin Source File

SOURCE=moc_idd_help.cpp
# End Source File
# Begin Source File

SOURCE=moc_idd_net.cpp
# End Source File
# Begin Source File

SOURCE=moc_idd_recv.cpp
# End Source File
# Begin Source File

SOURCE=moc_idd_recvq.cpp
# End Source File
# Begin Source File

SOURCE=moc_idd_search.cpp
# End Source File
# Begin Source File

SOURCE=moc_idd_send.cpp
# End Source File
# Begin Source File

SOURCE=moc_idd_xfers.cpp
# End Source File
# Begin Source File

SOURCE=moc_main_window.cpp
# End Source File
# Begin Source File

SOURCE=moc_prefdlgs.cpp
# End Source File
# Begin Source File

SOURCE=moc_preference.cpp
# End Source File
# Begin Source File

SOURCE=moc_qulf_dialog.cpp
# End Source File
# Begin Source File

SOURCE=moc_vaste_chat.cpp
# End Source File
# Begin Source File

SOURCE=moc_vaste_idd_net.cpp
# End Source File
# Begin Source File

SOURCE=moc_vaste_idd_search.cpp
# End Source File
# Begin Source File

SOURCE=moc_vaste_idd_xfers.cpp
# End Source File
# Begin Source File

SOURCE=moc_vaste_mainwindow.cpp
# End Source File
# End Group
# Begin Group "Forms"

# PROP Default_Filter "ui"
# Begin Source File

SOURCE=.\idd_about.ui

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_about.ui
InputName=idd_about

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_about.ui
InputName=idd_about

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\idd_chatchan.ui

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_chatchan.ui
InputName=idd_chatchan

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_chatchan.ui
InputName=idd_chatchan

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\idd_chatpriv.ui

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_chatpriv.ui
InputName=idd_chatpriv

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_chatpriv.ui
InputName=idd_chatpriv

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\idd_chatroomcreate.ui

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_chatroomcreate.ui
InputName=idd_chatroomcreate

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_chatroomcreate.ui
InputName=idd_chatroomcreate

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\idd_help.ui

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_help.ui
InputName=idd_help

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_help.ui
InputName=idd_help

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\idd_net.ui

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_net.ui
InputName=idd_net

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_net.ui
InputName=idd_net

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\idd_recv.ui

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_recv.ui
InputName=idd_recv

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_recv.ui
InputName=idd_recv

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\idd_recvq.ui

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_recvq.ui
InputName=idd_recvq

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_recvq.ui
InputName=idd_recvq

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\idd_search.ui

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_search.ui
InputName=idd_search

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_search.ui
InputName=idd_search

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\idd_send.ui

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_send.ui
InputName=idd_send

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_send.ui
InputName=idd_send

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\idd_xfers.ui

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_xfers.ui
InputName=idd_xfers

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\idd_xfers.ui
InputName=idd_xfers

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\main_window.ui

!IF  "$(CFG)" == "PadLockSL - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=.\main_window.ui
InputName=main_window

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PadLockSL - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\main_window.ui
InputName=main_window

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(ProjDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath)  -impl $(InputName).h -o $(ProjDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(ProjDir)\$(InputName).h -o $(ProjDir)\moc_$(InputName).cpp \
	

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
