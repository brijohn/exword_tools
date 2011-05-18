#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT(exword_tools, 0.1, brijohn@gmail.com)
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([src/common/Exword.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])
AC_CHECK_HEADER([zlib.h], [], [AC_MSG_ERROR([zlib header not found])])
# Checks for libraries.
AC_CHECK_LIB(z, inflateEnd, AC_SUBST([ZLIB_LIBS], [-lz]), [AC_MSG_ERROR([zlib support not available])])

AM_OPTIONS_WXCONFIG

reqwx=2.8.0
AM_PATH_WXCONFIG($reqwx, wxWin=1, wxWin=0, [core, xrc])
if test "$wxWin" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system.

		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
fi

AM_PATH_WXRC(wxrc=1)
if test "$wxrc" != 1; then
        AC_MSG_ERROR([
               The wxrc program was not installed or not found.

               Please check the wxWidgets installation.
        ])
fi

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
LIBS="$LIBS $WX_LIBS"


# Big-endian stuff
AC_SUBST(WORDS_BIGENDIAN)
AC_SUBST(BYTESWAP_HEADER,dummy)
AC_SUBST(HAVE_BYTESWAP_HEADER,0)

AC_C_BIGENDIAN()

if test "$ac_cv_c_bigendian" = yes; then
  WORDS_BIGENDIAN=1
  AC_CHECK_HEADERS(sys/byteswap.h,[BYTESWAP_HEADER=sys/byteswap.h; HAVE_BYTESWAP_HEADER=1])
  AC_CHECK_HEADERS(byteswap.h,    [BYTESWAP_HEADER=byteswap.h;     HAVE_BYTESWAP_HEADER=1])
else
  WORDS_BIGENDIAN=0
fi

# Checks for typedefs, structures, and compiler characteristics.
LIBEXWORD_REQUIRED=0.1
PKG_CHECK_MODULES([EXWORD],[libexword >= $LIBEXWORD_REQUIRED])

AC_SUBST([EXWORD_CFLAGS])
AC_SUBST([EXWORD_LIBS])

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])

AC_CONFIG_FILES([Makefile
		src/Makefile
		src/textloader/Makefile
		src/library/Makefile
		src/libraryinstaller/Makefile
		src/unshield/Makefile])

AC_OUTPUT
