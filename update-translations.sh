#!/bin/bash

# Copyright (C) 2023 by Robert Tari <robert@tari.in>
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

PROJECT=$(grep -Po "^project *\(\K[^ ]+" "CMakeLists.txt")
VERSION=$(grep -Po "^project *\(.* +VERSION +\K[0-9]+\.[0-9]+\.[0-9]+" "CMakeLists.txt")
COPYRIGHT="Ayatana Indicators developers"
ORGANISATION="AyatanaIndicators"
xgettext --output="po/$PROJECT.pot" --no-wrap --copyright-holder="$COPYRIGHT" --package-name="$PROJECT" --package-version="$VERSION" --default-domain="$PROJECT" --msgid-bugs-address="https://github.com/$ORGANISATION/$PROJECT/issues" --force-po --language=C --keyword=_ src/*.c
xgettext --output="po/$PROJECT.pot" --no-wrap --omit-header --join-existing --default-domain="$PROJECT" --language=Glade "data/$PROJECT.ui"
sed -e "s/_Name=/Name=/" -e  "s/_Comment=/Comment=/" "data/$PROJECT.desktop.in" > "data/$PROJECT.desktop.intl"
xgettext --output="po/$PROJECT.pot" --no-wrap --omit-header --join-existing --default-domain="$PROJECT" --keyword --keyword=Name --keyword=Comment --language=Desktop "data/$PROJECT.desktop.intl"
rm "data/$PROJECT.desktop.intl"
sed --in-place "s/\"Content-Type: text\/plain; charset=CHARSET\\\n\"/\"Content-Type: text\/plain; charset=UTF-8\\\n\"/" "po/$PROJECT.pot"
sed --in-place "s/# SOME DESCRIPTIVE TITLE./# Translation of $PROJECT in LANGUAGE/" "po/$PROJECT.pot"
sed --in-place "s/# Copyright (C) YEAR /# Copyright (C) $(date +%Y) /" "po/$PROJECT.pot"

CATALOGS=$(find . -name '*.po' | sort)

for CAT in $CATALOGS; do
    LOCALE=$(basename ${CAT%.*})
    echo "${LOCALE}" >> "po/LINGUAS"
    msgmerge --no-fuzzy-matching --update --backup=off "$CAT" "po/$PROJECT.pot"
done

rm "po/LINGUAS"
