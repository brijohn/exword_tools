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

Resources.h:
	$(WXRC) -c -e ../resources/xrc/LibraryInstaller.xrc -o Resources.cpp

clean-local:
	-rm -rf Resources.cpp Resources.h

