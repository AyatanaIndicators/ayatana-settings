#!/bin/bash

set -x

# Copyright (C) 2017 by Mike Gabriel <mike.gabriel@das-netzwerkteam.de>
# Copyright (C) 2020 by Robert Tari <robert@tari.in>
#
# This package is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 3 of the License.
#
# This package is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>

APPNAME=`grep "APPNAME =" ayatanasettings/appdata.py | sed -r -e "s/.*'(.*)'.*/\1/g"`
APPVERSION=`grep "APPVERSION =" ayatanasettings/appdata.py | sed -r -e "s/.*'(.*)'.*/\1/g"`

# .glade file
xgettext -o "po/${APPNAME}.pot" \
         --no-wrap --copyright-holder "Robert Tari" \
         --package-name "${APPNAME}" \
         --package-version "${APPVERSION}" \
         -d "${APPNAME}" \
         --msgid-bugs-address "https://github.com/AyatanaIndicators/${APPNAME}/issues" \
         -L Glade \
         "data/${APPNAME}/${APPNAME}.glade"

# .py files
xgettext -o "po/${APPNAME}.pot" \
         --no-wrap \
         --omit-header \
         -j \
         -d "${APPNAME}" \
         -L Python ayatanasettings/*.py

# main executable
xgettext -o "po/${APPNAME}.pot" \
         --no-wrap \
         --omit-header \
         -j \
         -d "${APPNAME}" \
         -L Python \
         "${APPNAME}"

# .desktop file
xgettext -o "po/${APPNAME}.pot" \
         --no-wrap \
         --omit-header \
         -j \
         -d "${APPNAME}" \
         -kName -kComment -kGenericName -k \
         "data/applications/${APPNAME}.desktop.in"

sed -i '/"Content-Type: text\/plain; charset=CHARSET\\n"/c\"Content-Type: text\/plain; charset=UTF-8\\n"' "po/${APPNAME}.pot"
