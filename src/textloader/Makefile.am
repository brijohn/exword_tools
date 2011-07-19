BUILT_SOURCES = Resources.h

bin_PROGRAMS = ExwordTextLoader

ExwordTextLoader_SOURCES = TextLoader.cpp TextLoaderFrame.cpp \
                           ../common/ExwordDevice.cpp ../common/Dictionary.cpp \
                           ../common/ThreadBase.cpp UploadThread.cpp \
                           ../common/ProgressDialog.cpp ../common/Model.cpp \
                           Resources.cpp

ExwordTextLoader_CXXFLAGS = -I../common \
                            $(WARN_CFLAGS)   \
                            $(AM_CFLAGS)     \
                            $(EXWORD_CFLAGS) \
                            @CXXFLAGS@

ExwordTextLoader_LDFLAGS = $(AM_LDFLAGS)
ExwordTextLoader_LDADD =   @LIBS@ $(EXWORD_LIBS)

Resources.h:
	$(WXRC) -c -e TextLoader.xrc -o Resources.cpp

clean-local:
	-rm -rf Resources.cpp Resources.h
