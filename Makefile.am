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

bin_PROGRAMS =
noinst_PROGRAMS =
man_MANS =
if !WANTXINV3D
bin_PROGRAMS += xinvaders3d
noinst_PROGRAMS += xinv3d
man_MANS += xinvaders3d.6
endif
if WANTXINV3D
bin_PROGRAMS += xinv3d
noinst_PROGRAMS += xinvaders3d
man_MANS += xinv3d.6
endif
invaderCode = aliens.c camera.c effects.c game.c object.c mat4x4.c player.c vec4x1.c main-x11.c
noinst_HEADERS = camera.h defines.h externs.h game.h gtext.h mat4x4.h object.h system.h vec4x1.h

xinvaders3d_CPPFLAGS = ${CFLAGS} ${WCFLAGS}
xinvaders3d_LDFLAGS = ${LDFLAGS}
xinvaders3d_LDADD = -lX11 -lm
xinvaders3d_SOURCES = ${invaderCode}
xinvaders3d.$(OBJEXT): ${noinst_HEADERS}

xinv3d_CPPFLAGS = ${CFLAGS} ${WCFLAGS}
xinv3d_LDFLAGS = ${LDFLAGS}
xinv3d_LDADD = -lX11 -lm
xinv3d_SOURCES = ${invaderCode}
xinv3d.$(OBJEXT): ${noinst_HEADERS}

DISTCLEANFILES = xinv3d.6
DISTCHECK_CONFIGURE_FLAGS = --enable-debuggame --enable-debugkeyboard --disable-silent-rules --enable-as-xinv3d

EXTRA_DIST = \
	AUTHORS Changelog main-dos.c main-w.c Makefile.dos Makefile.lin	 \
	README.md README.dos README.win xinvaders3d.jpg xinvaders3d.6.in \
	packaging/xinvaders3d.spec.in packaging/xinvaders3d.spec	 \
	m4/ax_check_compile_flag.m4 ${noinst_HEADERS}
nodist_EXTRA_DATA = .git .github xinvaders3d.6 xinv3d.6

strip:
	${STRIP} ${builddir}/xinvaders3d
	${STRIP} ${builddir}/xinv3d

xinv3d.6:
	cp ${builddir}/xinvaders3d.6 ${builddir}/xinv3d.6

clean-local:
	rm -f ${builddir}/xinv3d.6

