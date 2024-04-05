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

appdatadir = ${datarootdir}/appdata
appdata_DATA =
deskpixsdir = ${datarootdir}/pixmaps
deskpixs_DATA =
desktopdir = ${datarootdir}/applications
desktop_DATA =
ICONSDIR = ${datarootdir}/icons
HICOLORDIR = ${datarootdir}/icons/hicolor
hicolor32x32dir = ${HICOLORDIR}/32x32/apps
hicolor64x64dir = ${HICOLORDIR}/64x64/apps
hicolor128x128dir = ${HICOLORDIR}/128x128/apps
hicolor256x256dir = ${HICOLORDIR}/256x256/apps
hicolorscalabledir = ${HICOLORDIR}/scalable/apps
hicolor32x32_DATA =
hicolor64x64_DATA =
hicolor128x128_DATA =
hicolor256x256_DATA =
hicolorscalable_DATA =
metainfodir = ${datarootdir}/metainfo
metainfo_DATA =
mimedir = ${datarootdir}/mime
mimepackagesdir = ${datarootdir}/mime/packages
mimepackages_DATA =
if !WANTXINV3D
appdata_DATA += xinvaders3d.appdata.xml
deskpixs_DATA += 32x32/xinvaders3d.xpm 32x32/xinvaders3d.png
desktop_DATA += xinvaders3d.desktop
hicolor32x32_DATA += 32x32/xinvaders3d.png
hicolor64x64_DATA += 64x64/xinvaders3d.png
hicolor128x128_DATA += 128x128/xinvaders3d.png
hicolor256x256_DATA += 256x256/xinvaders3d.png
hicolorscalable_DATA += svg/xinvaders3d.svgz
metainfo_DATA += xinvaders3d.metainfo.xml
mimepackages_DATA += xinvaders3d.xml
endif
if WANTXINV3D
appdata_DATA += xinv3d.appdata.xml
deskpixs_DATA += ${builddir}/32x32/xinv3d.xpm ${builddir}/32x32/xinv3d.png
desktop_DATA += xinv3d.desktop
hicolor32x32_DATA += ${builddir}/32x32/xinv3d.png
hicolor64x64_DATA += ${builddir}/64x64/xinv3d.png
hicolor128x128_DATA += ${builddir}/128x128/xinv3d.png
hicolor256x256_DATA += ${builddir}/256x256/xinv3d.png
hicolorscalable_DATA += ${builddir}/svg/xinv3d.svgz
metainfo_DATA += xinv3d.metainfo.xml
mimepackages_DATA += xinv3d.xml
endif

EXTRA_DIST = \
	xinvaders3d.appdata.xml.in xinvaders3d.metainfo.xml.in			\
	xinvaders3d.xml.in 32x32/xinvaders3d.xpm 32x32/xinvaders3d.png		\
	64x64/xinvaders3d.png 128x128/xinvaders3d.png 256x256/xinvaders3d.png	\
	svg/xinvaders3d.svgz xinvaders3d.desktop.in

xinv3d.desktop:
	cp ${builddir}/xinvaders3d.desktop ${builddir}/xinv3d.desktop

xinv3d.appdata.xml:
	cp ${builddir}/xinvaders3d.appdata.xml ${builddir}/xinv3d.appdata.xml

xinv3d.metainfo.xml:
	cp ${builddir}/xinvaders3d.metainfo.xml ${builddir}/xinv3d.metainfo.xml

xinv3d.xml:
	cp ${builddir}/xinvaders3d.xml ${builddir}/xinv3d.xml

32x32/xinv3d.xpm:
	mkdir -p ${builddir}/32x32
	cp ${srcdir}/32x32/xinvaders3d.xpm ${builddir}/32x32/xinv3d.xpm

32x32/xinv3d.png:
	mkdir -p ${builddir}/32x32
	cp ${srcdir}/32x32/xinvaders3d.png ${builddir}/32x32/xinv3d.png

64x64/xinv3d.png:
	mkdir -p ${builddir}/64x64
	cp ${srcdir}/64x64/xinvaders3d.png ${builddir}/64x64/xinv3d.png

128x128/xinv3d.png:
	mkdir -p ${builddir}/128x128
	cp ${srcdir}/128x128/xinvaders3d.png ${builddir}/128x128/xinv3d.png

256x256/xinv3d.png:
	mkdir -p ${builddir}/256x256
	cp ${srcdir}/256x256/xinvaders3d.png ${builddir}/256x256/xinv3d.png

svg/xinv3d.svgz:
	mkdir -p ${builddir}/svg
	cp ${srcdir}/svg/xinvaders3d.svgz ${builddir}/svg/xinv3d.svgz

clean-local:
	rm -f ${builddir}/xinv3d.desktop
	rm -f ${builddir}/xinv3d.appdata.xml
	rm -f ${builddir}/xinv3d.metainfo.xml
	rm -f ${builddir}/xinv3d.xml
	rm -f ${builddir}/32x32/xinv3d.png
	rm -f ${builddir}/32x32/xinv3d.xpm
	rm -f ${builddir}/64x64/xinv3d.png
	rm -f ${builddir}/128x128/xinv3d.png
	rm -f ${builddir}/256x256/xinv3d.png
	rm -f ${builddir}/svg/xinv3d.svgz
