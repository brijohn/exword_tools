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
# Checks for libraries.
# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

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
		src/textloader/Makefile])
AC_OUTPUT
