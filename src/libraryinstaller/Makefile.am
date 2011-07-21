BUILT_SOURCES = Resources.h

bin_PROGRAMS = ExwordLibraryInstaller

ExwordLibraryInstaller_SOURCES = LibraryInstaller.cpp Resources.cpp \
                                 InstallWizard.cpp UnpackThread.cpp \
                                 ../common/ThreadBase.cpp

ExwordLibraryInstaller_LDADD = @LIBS@ ../unshield/libunshield.a $(ZLIB_LIBS)

ExwordLibraryInstaller_CXXFLAGS = -I../common    \
                                  -I../unshield  \
                                  $(WARN_CFLAGS) \
                                  $(AM_CFLAGS)   \
                                  @CXXFLAGS@

if HAVE_WINDRES
BUILT_SOURCES += ExwordTools.$(OBJEXT)
ExwordLibraryInstaller_LDADD   += ExwordTools.$(OBJEXT)

ExwordTools.$(OBJEXT):
	$(WX_RESCOMP) ../resources/windows/ExwordTools.rc -o $@
endif

Resources.h:
	$(WXRC) -c -e ../resources/xrc/LibraryInstaller.xrc -o Resources.cpp

clean-local:
	-rm -rf Resources.cpp Resources.h

