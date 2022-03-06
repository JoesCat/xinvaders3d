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

# Where .desktop files go.
desktopdir = ${datarootdir}/applications
desktop_DATA = xinv3d.desktop

# Where appdata and metainfo xml files go
appdatadir = ${datarootdir}/appdata
appdata_DATA = xinv3d.appdata.xml
metainfodir = ${datarootdir}/metainfo
metainfo_DATA = xinv3d.metainfo.xml

# Where MIME data goes (Support for .xinv3d data files, for instance).
mimedir = ${datarootdir}/mime
mimepackagesdir = ${datarootdir}/mime/packages
mimepackages_DATA = xinv3d.xml

#update-mime-database ${mimedir}

ICONSDIR = ${datarootdir}/icons
HICOLORDIR = ${datarootdir}/icons/hicolor
hicolor32x32dir = $(HICOLORDIR)/32x32/apps
hicolor64x64dir = $(HICOLORDIR)/64x64/apps
hicolor128x128dir = $(HICOLORDIR)/128x128/apps
hicolor256x256dir = $(HICOLORDIR)/256x256/apps

hicolor32x32_DATA = 32x32/xinv3d.png
hicolor64x64_DATA = 64x64/xinv3d.png
hicolor128x128_DATA = 128x128/xinv3d.png
hicolor256x256_DATA = 256x256/xinv3d.png

deskpixsdir = ${datarootdir}/pixmaps
deskpixs_DATA = 32x32/xinv3d.xpm 32x32/xinv3d.png

EXTRA_DIST = xinv3d.desktop xinv3d.appdata.xml xinv3d.metainfo.xml xinv3d.xml \
	32x32/xinv3d.xpm 32x32/xinv3d.png 64x64/xinv3d.png 128x128/xinv3d.png \
	256x256/xinv3d.png
