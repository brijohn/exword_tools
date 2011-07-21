BUILT_SOURCES = Resources.h

bin_PROGRAMS = ExwordLibrary

ExwordLibrary_SOURCES = Library.cpp LibraryFrame.cpp \
                        ../common/ExwordDevice.cpp ../common/Dictionary.cpp \
                        ../common/ThreadBase.cpp InstallThread.cpp \
                        ../common/Model.cpp Users.cpp DictionaryListCtrl.cpp \
                        ../common/ProgressDialog.cpp Resources.cpp

ExwordLibrary_CXXFLAGS = -I../common      \
                         $(WARN_CFLAGS)   \
                         $(AM_CFLAGS)     \
                         $(EXWORD_CFLAGS) \
                         @CXXFLAGS@

ExwordLibrary_LDFLAGS = $(AM_LDFLAGS)
ExwordLibrary_LDADD =   @LIBS@ $(EXWORD_LIBS)

if HAVE_WINDRES
BUILT_SOURCES += ExwordTools.$(OBJEXT)
ExwordLibrary_LDADD   += ExwordTools.$(OBJEXT)

ExwordTools.$(OBJEXT):
	$(WX_RESCOMP) -DLIBRARY_RC ../resources/windows/ExwordTools.rc -o $@
endif

Resources.h:
	$(WXRC) -c -e ../resources/xrc/Library.xrc -o Resources.cpp

clean-local:
	-rm -rf Resources.cpp Resources.h
