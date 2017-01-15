#!/bin/bash

# get_statustext.sh part of MavLink_FrSkySPort
# https://github.com/Clooney82/MavLink_FrSkySPort
#
# Copyright (C) 2015 Jochen Kielkopf
#    https://github.com/Clooney82/MavLink_FrSkySPort
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses>.


ARDUPILOT_DIR="PATH_TO_LOCAL_ARDUPILOT_REPO"

if [ ! -f $ARDUPILOT_DIR/ArduCopter/Copter.h ]; then
  echo "CHECK Ardupilot directory"
  exit 9
fi
grep THISFIRMWARE $ARDUPILOT_DIR/ArduCopter/Copter.h | sed 's/"//g' | sed 's/^.* V/V/' > VERSION.txt
VERSION=`cat VERSION.txt`

TARGET_DIR=`pwd`
if [ $(dirname $0) != "." ]; then
  echo "execute script this way: ./get_statustext.sh"
  rm VERSION.txt
  exit 9
fi

TARGET_FILE="ardupilot_texts_${VERSION}"
TARGET_FILE_TYPE="csv"

TEXTS_FILE="$TARGET_DIR/$TARGET_FILE.$TARGET_FILE_TYPE"

SEARCH_STRING="(send_statustext|send_status|gcs_send_text|SEVERITY)"

cd ${ARDUPILOT_DIR}
egrep -i ${SEARCH_STRING} * > ${TEXTS_FILE} 2>/dev/null
egrep -i ${SEARCH_STRING} */* >> ${TEXTS_FILE} 2>/dev/null
egrep -i ${SEARCH_STRING} */*/* >> ${TEXTS_FILE} 2>/dev/null
egrep -i ${SEARCH_STRING} */*/*/* >> ${TEXTS_FILE} 2>/dev/null
egrep -i ${SEARCH_STRING} */*/*/*/* >> ${TEXTS_FILE} 2>/dev/null
egrep -i ${SEARCH_STRING} */*/*/*/*/* >> ${TEXTS_FILE} 2>/dev/null
egrep -i ${SEARCH_STRING} */*/*/*/*/*/* >> ${TEXTS_FILE} 2>/dev/null
egrep -i ${SEARCH_STRING} */*/*/*/*/*/*/* >> ${TEXTS_FILE} 2>/dev/null
egrep -i ${SEARCH_STRING} */*/*/*/*/*/*/*/* >> ${TEXTS_FILE} 2>/dev/null
egrep -i ${SEARCH_STRING} */*/*/*/*/*/*/*/*/* >> ${TEXTS_FILE} 2>/dev/null
egrep -i ${SEARCH_STRING} */*/*/*/*/*/*/*/*/*/* >> ${TEXTS_FILE} 2>/dev/null

cd ${TARGET_DIR}
sed -i .bak.after_grep -f sed.file ${TEXTS_FILE}
cp -p ${TEXTS_FILE} ${TEXTS_FILE}.bak.after_sed

cat ${TEXTS_FILE}.bak.after_sed |sort -t";" -k3 -u | sort -t";" -k1 > ${TEXTS_FILE}
sed -i .bak.after_sort -f sed.file2 ${TEXTS_FILE}
sed -i .bak.after_delete_empty_lines '/^$/d' ${TEXTS_FILE}

rm ${TEXTS_FILE}.bak.after_grep ${TEXTS_FILE}.bak.after_sed ${TEXTS_FILE}.bak.after_sort ${TEXTS_FILE}.bak.after_delete_empty_lines
