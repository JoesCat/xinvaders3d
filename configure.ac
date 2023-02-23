#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Copyright (C) 2022 by Joe Da Silva

AC_PREREQ(2.61)

m4_define([xinvaders3d_version], [1.5.0])
AC_INIT([xinvaders3d],[xinvaders3d_version],[https://github.com/JoesCat/xinvaders3d/issues],
	[xinvaders3d],[https://github.com/JoesCat/xinvaders3d])
AC_CONFIG_SRCDIR([aliens.c])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_SYSTEM
AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AM_INIT_AUTOMAKE([foreign -Wall])
AC_USE_SYSTEM_EXTENSIONS
AC_DEFINE(MOTIF, HAVE_MOTIF)
AC_SUBST([XINVADERS3D_VERSION],[xinvaders3d_version])
AC_CHECK_TOOL([STRIP],[strip],[:])
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_MKDIR_P
AC_PROG_MAKE_SET

#-------------------------------------------
# Enable silent build rules by default, this
# requires atleast Automake-1.11. Disable by
# either passing --disable-silent-rules to
# configure or passing V=1 to make
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])],[AC_SUBST([AM_DEFAULT_VERBOSITY],[1])])

#-------------------------------------------
case $host in
   *-freebsd* | *-openbsd*)
	AC_DEFINE([GAME_LINUX_X11],1,[Build for X11])
	;;
   *-apple-darwin*)
	AC_DEFINE([GAME_LINUX_X11],1,[Build for X11])
	;;
   *mingw*)
	AC_DEFINE([GAME_WIN32],1,[Build for WIN_32])
	;;
   *cygwin*)
	AC_DEFINE([GAME_DOS_DJGPP],1,[Build for DOS])
	;;
   *) AC_DEFINE([GAME_LINUX_X11],1,[Build for X11])
esac

#-------------------------------------------
# Required development include files (and associated libraries)
AC_MSG_CHECKING([Check for X11 and required developer header files])
AC_CHECK_HEADER([X11/Xlib.h],[],[AC_MSG_FAILURE([ERROR: Missing X11/Xlib.h, Please install Developer version of libx11-devel or lib64x11-devel],[1])])

# Enable debug modes
AC_ARG_ENABLE([debuggame],
  [AS_HELP_STRING([--enable-debuggame],
    [Enable Game debugging printf mode on commandline @<:@default=no@:>@])],
  [],[enable_debuggame=no])
makeDebug=no
if test "x$enable_debuggame" = xyes || test "x$enable_debuggame" = xtrue ; then
  makeDebuggame=yes
  AC_DEFINE([GAME_DEBUG],1,[Define if debugging game using printf.])
fi

AC_ARG_ENABLE([debugkeyboard],
  [AS_HELP_STRING([--enable-debugkeyboard],
    [Enable Keyboard debugging printf mode on commandline @<:@default=no@:>@])],
  [],[enable_debugkeyboard=no])
makeDebugkeyboard=no
if test "x$enable_debugkeyboard" = xyes || test "x$enable_debugkeyboard" = xtrue ; then
  makeDebugkeyboard=yes
  AC_DEFINE([GAME_DEBUG_KEYBOARD],1,[Define if debugging Keyboard using printf.])
fi

#-------------------------------------------
# Check for and add usable compiler warnings
WCFLAGS=""
AC_LANG_PUSH([C])
AX_CHECK_COMPILE_FLAG([-Wall],[WCFLAGS="$WCFLAGS -Wall"])
AX_CHECK_COMPILE_FLAG([-Wextra],[WCFLAGS="$WCFLAGS -Wextra"])
AX_CHECK_COMPILE_FLAG([-Wcast-align],[WCFLAGS="$WCFLAGS -Wcast-align"])
AX_CHECK_COMPILE_FLAG([-Wbad-function-cast],[WCFLAGS="$WCFLAGS -Wbad-function-cast"])
AX_CHECK_COMPILE_FLAG([-Wc++-compat],[WCFLAGS="$WCFLAGS -Wc++-compat"])
AX_CHECK_COMPILE_FLAG([-Wmissing-prototypes],[WCFLAGS="$WCFLAGS -Wmissing-prototypes"])
AX_CHECK_COMPILE_FLAG([-Wunused],[WCFLAGS="$WCFLAGS -Wunused"])
AX_CHECK_COMPILE_FLAG([-Wdeprecated-declarations],[WCFLAGS="$WCFLAGS -Wdeprecated-declarations"])
AC_LANG_POP
AC_SUBST([WCFLAGS])

#-------------------------------------------
AC_CONFIG_FILES([Makefile xinvaders3d.pc desktop/Makefile packaging/xinvaders3d.spec])
AC_OUTPUT
AC_MSG_NOTICE([

Configuration:

  Source code location	${srcdir}
  Build code location	${builddir}
  Destination prefix	${prefix}
  Host Operating System	${host}
  Compiler		${CC}
  Config CFLAGS		"${CFLAGS}"
  Config WFLAGS		"${WCFLAGS}"
  Config LDFLAGS	"${LDFLAGS}"

])
