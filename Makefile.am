SUBDIRS = src po

INTLTOOL_FILES = intltool-extract.in \
                 intltool-merge.in \
                 intltool-update.in

EXTRA_DIST = TextLoader.glade
pkgdata_DATA = TextLoader.glade

DISTCLEANFILES = intltool-extract.in \
                 intltool-merge.in \
                 intltool-update.in \
                 po/Makefile.in.in \
                 src/Makefile.in \
                 Makefile.in \
                 aclocal.m4 \
                 config.h.in \
                 configure \
                 compile \
                 INSTALL \
                 install-sh \
                 depcomp \
                 COPYING \
                 missing

distclean-local:
	-rm -rf autom4te.cache
