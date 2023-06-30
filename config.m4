dnl config.m4 for extension lua

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use 'with':

dnl PHP_ARG_WITH([lua],
dnl   [for lua support],
dnl   [AS_HELP_STRING([--with-lua],
dnl     [Include lua support])])

dnl Otherwise use 'enable':

PHP_ARG_ENABLE([lua],
  [whether to enable lua support],
  [AS_HELP_STRING([--enable-lua],
    [Enable lua support])],
  [no])

if test "$PHP_LUA" != "no"; then
  dnl Write more examples of tests here...

  dnl Remove this code block if the library does not support pkg-config.
  PKG_CHECK_MODULES([LUA], [lua])
  PHP_EVAL_INCLINE($LUA_CFLAGS)
  PHP_EVAL_LIBLINE($LUA_LIBS, LUA_SHARED_LIBADD)


  dnl If you need to check for a particular library version using PKG_CHECK_MODULES,
  dnl you can use comparison operators. For example:
  dnl PKG_CHECK_MODULES([LIBFOO], [foo >= 1.2.3])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo < 3.4])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo = 1.2.3])

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-lua -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/lua.h"  # you most likely want to change this
  dnl if test -r $PHP_LUA/$SEARCH_FOR; then # path given as parameter
  dnl   LUA_DIR=$PHP_LUA
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for lua files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       LUA_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$LUA_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the lua distribution])
  dnl fi

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-lua -> add include path
  dnl PHP_ADD_INCLUDE($LUA_DIR/include)

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-lua -> check for lib and symbol presence
  dnl LIBNAME=LUA # you may want to change this
  dnl LIBSYMBOL=LUA # you most likely want to change this

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   AC_DEFINE(HAVE_LUA_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your lua library.])
  dnl ], [
  dnl   $LIBFOO_LIBS
  dnl ])

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are not using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LUA_DIR/$PHP_LIBDIR, LUA_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_LUA_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your lua library.])
  dnl ],[
  dnl   -L$LUA_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(LUA_SHARED_LIBADD)

  dnl In case of no dependencies
  AC_DEFINE(HAVE_LUA, 1, [ Have lua support ])

  PHP_NEW_EXTENSION(lua, lua.c, $ext_shared)
fi
