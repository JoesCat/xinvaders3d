# Makefile.am - build XINVADERS 3D - 3d Shoot'em up
# Copyright (C) 2022 Jose Da Silva
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

SUBDIRS = . desktop

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

bin_PROGRAMS = xinv3d
xinv3d_CPPFLAGS = ${CFLAGS} ${WCFLAGS}
xinv3d_LDFLAGS = ${LDFLAGS}
xinv3d_LDADD = -lX11 -lm
xinv3d_SOURCES = aliens.c camera.c effects.c game.c object.c mat4x4.c player.c vec4x1.c main-x11.c
noinst_HEADERS = camera.h defines.h externs.h game.h gtext.h mat4x4.h object.h system.h vec4x1.h
xinv3d.$(OBJEXT): ${noinst_HEADERS}

man_MANS = xinv3d.6

pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA = xinv3d.pc

EXTRA_DIST = AUTHORS Changelog main-dos.c main-w.c Makefile.dos Makefile.lin README.md README.dos README.win xinvaders3d.jpg xinv3d.6 ${noinst_HEADERS}
