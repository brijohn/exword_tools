bin_PROGRAMS = TextLoader

TextLoader_SOURCES = TextLoader.c
TextLoader_CFLAGS = \
                    $(WARN_CFLAGS)   \
                    $(AM_CFLAGS)     \
                    $(EXWORD_CFLAGS) \
                    $(GTK_CFLAGS)

TextLoader_LDFLAGS = $(AM_LDFLAGS)
TextLoader_LDADD =   $(EXWORD_LIBS) $(GTK_LIBS)

AM_LDFLAGS  = -export-dynamic
AM_CPPFLAGS = -DPKGDATADIR=\"${pkgdatadir}\" -DLOCALEDIR=\"${LOCALEDIR}\"
