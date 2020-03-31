# Microsoft Developer Studio Generated NMAKE File, Based on webs.dsp
!IF "$(CFG)" == ""
# CFG=webs - Win32 Debug
CFG=webs - Win32 Release
!ENDIF 

!IF "$(CFG)" != "webs - Win32 Release" && "$(CFG)" != "webs - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "webs.mak" CFG="webs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "webs - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "webs - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF "$(INCLUDE)" == ""
GOINC="\Program Files\Microsoft Visual Studio\VC98\include"
INCLUDE=%include%;$(GOINC)
!ENDIF

# User Management switch
UMSW=/D USER_MANAGEMENT_SUPPORT

# Digest Access switch
DASW=/D DIGEST_ACCESS_SUPPORT

# Secure Socket Layer (SSL) switch
# SSLSW=/D WEBS_SSL_SUPPORT

# SSL directories
!IF "$(SSLSW)" != ""
SSLINCDIR=/RSASSL/library/include
SSLLIBDIR="/RSASSL/library/lib/nocrtdll"
SSLLIBS=sslc32.lib
!ELSE
SSLINCDIR=.
!ENDIF

!IF  "$(CFG)" == "webs - Win32 Release"

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=.
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\webs.exe"

!ELSE 

ALL : "$(OUTDIR)\webs.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\asp.obj"
	-@erase "$(INTDIR)\balloc.obj"
	-@erase "$(INTDIR)\base64.obj"
	-@erase "$(INTDIR)\cgi.obj"
	-@erase "$(INTDIR)\default.obj"
	-@erase "$(INTDIR)\ejlex.obj"
	-@erase "$(INTDIR)\ejparse.obj"
	-@erase "$(INTDIR)\emfdb.obj"
	-@erase "$(INTDIR)\form.obj"
	-@erase "$(INTDIR)\h.obj"
	-@erase "$(INTDIR)\handler.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\md5c.obj"
	-@erase "$(INTDIR)\mime.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\page.obj"
	-@erase "$(INTDIR)\ringq.obj"
	-@erase "$(INTDIR)\rom.obj"
	-@erase "$(INTDIR)\security.obj"
	-@erase "$(INTDIR)\sock.obj"
	-@erase "$(INTDIR)\sockGen.obj"
	-@erase "$(INTDIR)\sym.obj"
	-@erase "$(INTDIR)\uemf.obj"
	-@erase "$(INTDIR)\um.obj"
	-@erase "$(INTDIR)\umui.obj"
	-@erase "$(INTDIR)\url.obj"
	-@erase "$(INTDIR)\value.obj"
	-@erase "$(INTDIR)\webrom.obj"
	-@erase "$(INTDIR)\webs.obj"
	-@erase "$(INTDIR)\websda.obj"
	-@erase "$(INTDIR)\websuemf.obj"
	-@erase "$(OUTDIR)\webs.exe"
	-@erase "$(OUTDIR)\webs.map"
	-@erase "$(OUTDIR)\webs.pdb"
!IF "$(SSLSW)" != ""
	-@erase "$(INTDIR)\websSSL.obj" 
!ENDIF 


CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /ML /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" \
/I$(SSLINCDIR) $(SSLSW) $(DASW) $(UMSW) \
 /D "WIN" /D "WEBS" /D "UEMF" /I$(GOINC) /Fp"$(INTDIR)\webs.pch" /YX /FD /c 
CPP_OBJS=.
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\webs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
!IF "$(SSLSW)" == ""
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\webs.pdb" /map:"$(INTDIR)\webs.map" \
 /machine:I386 /out:"$(OUTDIR)\webs.exe" 
!ELSE
LINK32_FLAGS=$(SSLLIBS) wsock32.lib kernel32.lib user32.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\webs.pdb" /map:"$(INTDIR)\webs.map" /libpath:$(SSLLIBDIR) \
 /machine:I386 /out:"$(OUTDIR)\webs.exe" /nodefaultlib:"libcmt.lib" 
!ENDIF
LINK32_OBJS= \
	"$(INTDIR)\asp.obj" \
	"$(INTDIR)\balloc.obj" \
	"$(INTDIR)\base64.obj" \
	"$(INTDIR)\cgi.obj" \
	"$(INTDIR)\default.obj" \
	"$(INTDIR)\ejlex.obj" \
	"$(INTDIR)\ejparse.obj" \
	"$(INTDIR)\emfdb.obj" \
	"$(INTDIR)\form.obj" \
	"$(INTDIR)\h.obj" \
	"$(INTDIR)\handler.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\md5c.obj" \
	"$(INTDIR)\mime.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\page.obj" \
	"$(INTDIR)\ringq.obj" \
	"$(INTDIR)\rom.obj" \
	"$(INTDIR)\security.obj" \
	"$(INTDIR)\sock.obj" \
	"$(INTDIR)\sockGen.obj" \
	"$(INTDIR)\sym.obj" \
	"$(INTDIR)\uemf.obj" \
	"$(INTDIR)\um.obj" \
	"$(INTDIR)\umui.obj" \
	"$(INTDIR)\url.obj" \
	"$(INTDIR)\value.obj" \
	"$(INTDIR)\webrom.obj" \
	"$(INTDIR)\webs.obj" \
	"$(INTDIR)\websda.obj" \
	"$(INTDIR)\websuemf.obj" \
!IF "$(SSLSW)" != ""
	"$(INTDIR)\websSSL.obj" 
!ENDIF 

"$(OUTDIR)\webs.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "webs - Win32 Debug"

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=.
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\webs.exe"

!ELSE 

ALL : "$(OUTDIR)\webs.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\asp.obj"
	-@erase "$(INTDIR)\balloc.obj"
	-@erase "$(INTDIR)\base64.obj"
	-@erase "$(INTDIR)\cgi.obj"
	-@erase "$(INTDIR)\default.obj"
	-@erase "$(INTDIR)\ejlex.obj"
	-@erase "$(INTDIR)\ejparse.obj"
	-@erase "$(INTDIR)\emfdb.obj"
	-@erase "$(INTDIR)\form.obj"
	-@erase "$(INTDIR)\h.obj"
	-@erase "$(INTDIR)\handler.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\md5c.obj"
	-@erase "$(INTDIR)\mime.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\page.obj"
	-@erase "$(INTDIR)\ringq.obj"
	-@erase "$(INTDIR)\rom.obj"
	-@erase "$(INTDIR)\security.obj"
	-@erase "$(INTDIR)\sock.obj"
	-@erase "$(INTDIR)\sockGen.obj"
	-@erase "$(INTDIR)\sym.obj"
	-@erase "$(INTDIR)\uemf.obj"
	-@erase "$(INTDIR)\um.obj"
	-@erase "$(INTDIR)\umui.obj"
	-@erase "$(INTDIR)\url.obj"
	-@erase "$(INTDIR)\value.obj"
	-@erase "$(INTDIR)\webrom.obj"
	-@erase "$(INTDIR)\webs.obj"
	-@erase "$(INTDIR)\websda.obj"
	-@erase "$(INTDIR)\websuemf.obj"
	-@erase "$(OUTDIR)\webs.exe"
	-@erase "$(OUTDIR)\webs.map"
	-@erase "$(OUTDIR)\webs.pdb"
!IF "$(SSLSW)" != ""
	-@erase "$(INTDIR)\websSSL.obj" 
!ENDIF 

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MLd /W3 /Gm /Zi /D "WIN32" /D "_DEBUG" /D "_WINDOWS" \
 $(SSLSW) $(DASW) $(UMSW)  \
/D "WIN" /D "WEBS" /D "UEMF" /I$(SSLINCDIR) /I$(GOINC) /FD /c 
CPP_OBJS=.
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\webs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
!IF "$(SSLSW)" == ""
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib advapi32.lib \
/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\webs.pdb" \
 /map:"$(INTDIR)\webs.map" /debug /machine:I386 /out:"$(OUTDIR)\webs.exe"\
 /pdbtype:sept /LIBPATH:"\Program Files\DevStudio\VC\lib" 
!ELSE
LINK32_FLAGS=$(SSLLIBS) wsock32.lib kernel32.lib user32.lib advapi32.lib \
/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\webs.pdb" /libpath:$(SSLLIBDIR) \
 /map:"$(INTDIR)\webs.map" /debug /machine:I386 /out:"$(OUTDIR)\webs.exe"\
 /pdbtype:sept /LIBPATH:"\Program Files\DevStudio\VC\lib" /nodefaultlib:"libcmt.lib" 
!ENDIF
LINK32_OBJS= \
	"$(INTDIR)\asp.obj" \
	"$(INTDIR)\balloc.obj" \
	"$(INTDIR)\base64.obj" \
	"$(INTDIR)\cgi.obj" \
	"$(INTDIR)\default.obj" \
	"$(INTDIR)\ejlex.obj" \
	"$(INTDIR)\ejparse.obj" \
	"$(INTDIR)\emfdb.obj" \
	"$(INTDIR)\form.obj" \
	"$(INTDIR)\h.obj" \
	"$(INTDIR)\handler.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\md5c.obj" \
	"$(INTDIR)\mime.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\page.obj" \
	"$(INTDIR)\ringq.obj" \
	"$(INTDIR)\rom.obj" \
	"$(INTDIR)\security.obj" \
	"$(INTDIR)\sock.obj" \
	"$(INTDIR)\sockGen.obj" \
	"$(INTDIR)\sym.obj" \
	"$(INTDIR)\uemf.obj" \
	"$(INTDIR)\url.obj" \
	"$(INTDIR)\um.obj" \
	"$(INTDIR)\umui.obj" \
	"$(INTDIR)\value.obj" \
	"$(INTDIR)\webrom.obj" \
	"$(INTDIR)\webs.obj" \
	"$(INTDIR)\websda.obj" \
	"$(INTDIR)\websuemf.obj" \
!IF "$(SSLSW)" != ""
	"$(INTDIR)\websSSL.obj"
!ENDIF 

"$(OUTDIR)\webs.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "webs - Win32 Release" || "$(CFG)" == "webs - Win32 Debug"
SOURCE=..\asp.c
DEP_CPP_ASP_C=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ASP_C=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\asp.obj" : $(SOURCE) $(DEP_CPP_ASP_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\balloc.c
DEP_CPP_BALLO=\
	"..\uemf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_BALLO=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\balloc.obj" : $(SOURCE) $(DEP_CPP_BALLO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\base64.c
DEP_CPP_BASE6=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_BASE6=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\base64.obj" : $(SOURCE) $(DEP_CPP_BASE6) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\cgi.c
DEP_CPP_CGI=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CGI=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\cgi.obj" : $(SOURCE) $(DEP_CPP_CGI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\default.c
DEP_CPP_DEFAU=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DEFAU=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\default.obj" : $(SOURCE) $(DEP_CPP_DEFAU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\ejlex.c
DEP_CPP_EJLEX=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_EJLEX=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\ejlex.obj" : $(SOURCE) $(DEP_CPP_EJLEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\ejparse.c
DEP_CPP_EJPAR=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_EJPAR=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\ejparse.obj" : $(SOURCE) $(DEP_CPP_EJPAR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)

SOURCE=..\emfdb.c
DEP_CPP_EMFDB=\
	"..\emfdb.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_EMFDB=\
	"..\emfdb.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\

	
"$(INTDIR)\emfdb.obj" : $(SOURCE) $(DEP_CPP_CGI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)

SOURCE=..\form.c
DEP_CPP_FORM_=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_FORM_=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\form.obj" : $(SOURCE) $(DEP_CPP_FORM_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\h.c
DEP_CPP_H_Ce=\
	"..\uemf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_H_Ce=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\h.obj" : $(SOURCE) $(DEP_CPP_H_Ce) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\handler.c
DEP_CPP_HANDL=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HANDL=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\handler.obj" : $(SOURCE) $(DEP_CPP_HANDL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\main.c
DEP_CPP_MAIN_=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MAIN_=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"

SOURCE=..\md5c.c
DEP_CPP_MD5C=\
	"..\md5.h"\
	
NODEP_CPP_MD5C=\
	"..\md5.h"\

	
"$(INTDIR)\md5c.obj" : $(SOURCE) $(DEP_CPP_CGI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)

SOURCE=..\mime.c
DEP_CPP_MIME_=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MIME_=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\mime.obj" : $(SOURCE) $(DEP_CPP_MIME_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\misc.c
DEP_CPP_MISC_=\
	"..\uemf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_MISC_=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\misc.obj" : $(SOURCE) $(DEP_CPP_MISC_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\page.c
DEP_CPP_PAGE_=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PAGE_=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\page.obj" : $(SOURCE) $(DEP_CPP_PAGE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\ringq.c
DEP_CPP_RINGQ=\
	"..\uemf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_RINGQ=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\ringq.obj" : $(SOURCE) $(DEP_CPP_RINGQ) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\rom.c
DEP_CPP_ROM_C=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ROM_C=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\rom.obj" : $(SOURCE) $(DEP_CPP_ROM_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\security.c
DEP_CPP_SECUR=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SECUR=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\security.obj" : $(SOURCE) $(DEP_CPP_SECUR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\sock.c
DEP_CPP_SOCK=\
	"..\uemf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SOCK=\
	"..\arpa\inet.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\sock.obj" : $(SOURCE) $(DEP_CPP_SOCK) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\sockGen.c
DEP_CPP_SOCKG=\
	"..\uemf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SOCKG=\
	"..\arpa\inet.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\sockGen.obj" : $(SOURCE) $(DEP_CPP_SOCKG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\sym.c
DEP_CPP_SYM_C=\
	"..\uemf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SYM_C=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\sym.obj" : $(SOURCE) $(DEP_CPP_SYM_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\uemf.c
DEP_CPP_UEMF_=\
	"..\uemf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_UEMF_=\
	"..\arpa\inet.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\uemf.obj" : $(SOURCE) $(DEP_CPP_UEMF_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\um.c
DEP_CPP_UM=\
	"..\emfdb.h"\
	"..\um.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_UM=\
	"..\emfdb.h"\
	"..\um.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\

	
"$(INTDIR)\um.obj" : $(SOURCE) $(DEP_CPP_CGI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\umui.c
DEP_CPP_UMUI=\
	"..\emfdb.h"\
	"..\um.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_UMUI=\
	"..\emfdb.h"\
	"..\um.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\

	
"$(INTDIR)\umui.obj" : $(SOURCE) $(DEP_CPP_UMUI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)

SOURCE=..\url.c
DEP_CPP_URL_C=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_URL_C=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\url.obj" : $(SOURCE) $(DEP_CPP_URL_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\value.c
DEP_CPP_VALUE=\
	"..\uemf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_VALUE=\
	"..\arpa\inet.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\value.obj" : $(SOURCE) $(DEP_CPP_VALUE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\webrom.c

"$(INTDIR)\webrom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\webs.c
DEP_CPP_WEBS_=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_WEBS_=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\webs.obj" : $(SOURCE) $(DEP_CPP_WEBS_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)

SOURCE=..\websda.c
DEP_CPP_WEBSDA_=\
	"..\websda.h"\
	"..\md5.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_WEBSDA_=\
	"..\websda.h"\
	"..\md5.h"\
	

"$(INTDIR)\websda.obj" : $(SOURCE) $(DEP_CPP_WEBS_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)

SOURCE=..\websSSL.c
DEP_CPP_WEBSSSL_=\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_WEBS_=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\websSSL.obj" : $(SOURCE) $(DEP_CPP_WEBS_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\websuemf.c
DEP_CPP_WEBSUE=\
	"..\ej.h"\
	"..\ejIntrn.h"\
	"..\uemf.h"\
	"..\webs.h"\
	"..\wsIntrn.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_WEBSUE=\
	"..\arpa\inet.h"\
	"..\basic\basic.h"\
	"..\emf\emf.h"\
	"..\netdb.h"\
	"..\netinet\in.h"\
	"..\sys\select.h"\
	"..\sys\socket.h"\
	

"$(INTDIR)\websuemf.obj" : $(SOURCE) $(DEP_CPP_WEBSUE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

