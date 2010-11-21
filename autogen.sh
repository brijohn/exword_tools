# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="exword_tools"
REQUIRED_AUTOMAKE_VERSION=1.9

(test -f $srcdir/configure.ac \
  && test -d $srcdir/src) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level exword_tools directory"
    exit 1
}

(cd $srcdir;
    autoreconf --install --symlink &&
    intltoolize --copy --force --automake &&
    ./configure $@
)


