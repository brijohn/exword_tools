BUILT_SOURCES = Resources.h

bin_PROGRAMS = ExwordLibrary

ExwordLibrary_SOURCES = Library.cpp LibraryFrame.cpp \
                        ../common/Exword.cpp ../common/Dictionary.cpp \
                        ../common/TransferThread.cpp InstallThread.cpp \
                        ../common/Model.cpp Users.cpp DictionaryListCtrl.cpp \
                        ../common/ProgressDialog.cpp Resources.cpp

ExwordLibrary_CXXFLAGS = -I../common \
                         $(WARN_CFLAGS)   \
                         $(AM_CFLAGS)     \
                         $(EXWORD_CFLAGS) \
                         @CXXFLAGS@

ExwordLibrary_LDFLAGS = $(AM_LDFLAGS)
ExwordLibrary_LDADD =   @LIBS@ $(EXWORD_LIBS)

Resources.h:
	$(WXRC) -c -e Library.xrc -o Resources.cpp

clean-local:
	-rm -rf Resources.cpp Resources.h
