#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT(exword_tools, 0.1, brijohn@gmail.com)
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([src/TextLoader.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
# Checks for libraries.

#setup intltool
IT_PROG_INTLTOOL([0.35.0])
GETTEXT_PACKAGE=exword_tools
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
                   [The domain to use with gettext])
AM_GLIB_GNU_GETTEXT

LOCALEDIR=[${datadir}/locale]
AC_SUBST(LOCALEDIR)


# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
LIBEXWORD_REQUIRED=0.1
GTK_REQUIRED=2.16
PKG_CHECK_MODULES([EXWORD],[libexword >= $LIBEXWORD_REQUIRED])
PKG_CHECK_MODULES([GTK],[gtk+-2.0 >= $GTK_REQUIRED])

AC_SUBST([EXWORD_CFLAGS])
AC_SUBST([EXWORD_LIBS])

AC_SUBST([GTK_CFLAGS])
AC_SUBST([GTK_LIBS])

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])

AC_CONFIG_FILES([Makefile
		src/Makefile
		po/Makefile.in])
AC_OUTPUT
