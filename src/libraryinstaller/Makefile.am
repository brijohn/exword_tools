BUILT_SOURCES = Resources.h

bin_PROGRAMS = ExwordLibraryInstaller

ExwordLibraryInstaller_SOURCES = LibraryInstaller.cpp Resources.cpp \
                                 InstallWizard.cpp UnpackThread.cpp

ExwordLibraryInstaller_LDADD = @LIBS@ ../unshield/libunshield.a $(ZLIB_LIBS)
ExwordLibraryInstaller_CXXFLAGS = @CXXFLAGS@

AM_CPPFLAGS = -I../unshield

Resources.h:
	$(WXRC) -c -e LibraryInstaller.xrc -o Resources.cpp

clean-local:
	-rm -rf Resources.cpp Resources.h

