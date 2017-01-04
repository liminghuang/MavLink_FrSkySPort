#!/bin/bash

# make_textids.sh part of MavLink_FrSkySPort
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

VERSION=`cat VERSION.txt`
version=`cat VERSION.txt | sed 's/V/v/'`
ac_version=`cat VERSION.txt | sed 's/V/v/' | sed 's/\./_/'`
SOURCE_FILE="ardupilot_texts_$VERSION.csv"
TARGET_FILE="Avg_statustext_$version.ino"

SOURCE_DIRECTORY="$(dirname $0)"
DESTINATION_DIRECTORY_SOUNDS="$SOURCE_DIRECTORY/SOUNDS"
VOICE="Samantha"
#VOICE="Ava"
LANG="en"		# supported languages: en
Hz="32000"  # sample rate to encode the audio at, noted in Hz. 32000Hz = 32KHz. Higher is higher quality (16000, 22000, 32000).
directory="$LANG/$VOICE/TELEM"
# =================================================================================================
# Functions
# =================================================================================================
checkFolder() { [ -d "$@" ] || mkdir -p "$@"; }

say -v "$VOICE" --data-format=LEI16@32000 "Creating sounds used by MavLink to F.r.Sky Telemetry Converter."

sed "s/AC_VERSION/$ac_version/" $SOURCE_DIRECTORY/TEMPLATE/statustext_begin.txt | sed "s/VERSION/$version/" > $TARGET_FILE

last_source=""
IFS=";"  # File Delimiter
i=0
while read sourcefile severity statustext variances; do
  i=$[i+1]
  if [ "$sourcefile" != "$last_source" ] ; then
    echo "" >> $TARGET_FILE
    echo "  //$sourcefile" >> $TARGET_FILE
    last_source="$sourcefile"
  fi
  if [ "$variances" == "" ]; then
    echo "  else if(text == \"$statustext\")                                                            textId = $i;" >> $TARGET_FILE
  elif [ "$variances" == "endsWith" ]; then
    echo "  else if(text.endsWith(\"$statustext\"))                                                   textId = $i;" >> $TARGET_FILE
  elif [ "$variances" == "startsWith" ]; then
    echo "  else if(text.startsWith(\"$statustext\"))                                                   textId = $i;" >> $TARGET_FILE
  else
    echo "  else if(text == \"$statustext\")                                                            textId = $i;" >> $TARGET_FILE
  fi

  #say -v "Samantha" "$statustext"
  file_name="MSG$i"
  # Check and Create Folder
  checkFolder "$DESTINATION_DIRECTORY_SOUNDS/$directory"

  # Create Voice File
  say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/$file_name.wav" -v "$VOICE" --data-format=LEI16@32000 "$statustext"
  #say -v "$VOICE" --data-format=LEI16@32000 "$statustext"
done < "$SOURCE_FILE"
cat $SOURCE_DIRECTORY/TEMPLATE/statustext_end.txt >> $TARGET_FILE

MAV_SEVERITY=("EMERGENCY!" "ALERT!" "CRITICAL!" "ERROR!" "WARNING!" "NOTICE!" "INFO!")
count=${#MAV_SEVERITY[@]}
for (( i=0; i<${count}; i++ ));
do
  file_name="SEV$i"
  #echo $file_name
  say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/$file_name.wav" -v "$VOICE" --data-format=LEI16@32000 "${MAV_SEVERITY[$i]}"
  #say -v "$VOICE" --data-format=LEI16@32000 "${MAV_SEVERITY[$i]}"
done

FLIGHT_MODES=("Stabilize" "Acro" "Altitude Hold" "Auto" "Guided" "Loiter" "Return to launch" "Circle" "Invalid Mode" "Land" "Optical Loiter" "Drift" "Invalid Mode" "Sport" "Flip Mode" "Auto Tune" "Position Hold" "Brake")
count=${#FLIGHT_MODES[@]}
#echo $count
for (( i=0; i<${count}; i++ ));
do
  file_name="AVFM${i}A"
  #echo $file_name
  if [ ${FLIGHT_MODES[$i]} == "Invalid Mode" ]; then
    say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/$file_name.wav" -v "$VOICE" --data-format=LEI16@32000 "Invalid Flight Mode!"
    #say -v "$VOICE" --data-format=LEI16@32000 "Invalid Flight Mode!"
  else
    say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/$file_name.wav" -v "$VOICE" --data-format=LEI16@32000 "${FLIGHT_MODES[$i]},active"
    #say -v "$VOICE" --data-format=LEI16@32000 "${FLIGHT_MODES[$i]},active"
  fi
done

TEXT="System,armed!"
say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/SARM.wav" -v "$VOICE" --data-format=LEI16@32000 "$TEXT"
#say -v "$VOICE" --data-format=LEI16@32000 "$TEXT"

TEXT="System,disarmed!"
say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/SDISAR.wav" -v "$VOICE" --data-format=LEI16@32000 "$TEXT"
#say -v "$VOICE" --data-format=LEI16@32000 "$TEXT"

TEXT="Point"
say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/POINT.wav" -v "$VOICE" --data-format=LEI16@32000 "$TEXT"
#say -v "$VOICE" --data-format=LEI16@32000 "$TEXT"

TEXT="Cell minimum at"
say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/CELLMIN.wav" -v "$VOICE" --data-format=LEI16@32000 "$TEXT"
#say -v "$VOICE" --data-format=LEI16@32000 "$TEXT"

TEXT="Critical!,Cell minimum at"
say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/CRICM.wav" -v "$VOICE" --data-format=LEI16@32000 "$TEXT"
#say -v "$VOICE" --data-format=LEI16@32000 "$TEXT"

TEXT="Warning!,Cell minimum at"
say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/WARNCM.wav" -v "$VOICE" --data-format=LEI16@32000 "$TEXT"
#say -v "$VOICE" --data-format=LEI16@32000 "$TEXT"

TEXT="Warning!,Achieved Main Service Interval."
say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/ServMain.wav" -v "$VOICE" --data-format=LEI16@32000 "$TEXT"
#say -v "$VOICE" --data-format=LEI16@32000 "$TEXT"

TEXT="Attention!,Achieved Service Interval one."
say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/ServOne.wav" -v "$VOICE" --data-format=LEI16@32000 "$TEXT"
#say -v "$VOICE" --data-format=LEI16@32000 "$TEXT"

TEXT="Attention!,Achieved Service Interval two."
say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/ServTwo.wav" -v "$VOICE" --data-format=LEI16@32000 "$TEXT"
#say -v "$VOICE" --data-format=LEI16@32000 "$TEXT"

TEXT="Attention!,Achieved Service Interval three."
say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/ServThre.wav" -v "$VOICE" --data-format=LEI16@32000 "$TEXT"
#say -v "$VOICE" --data-format=LEI16@32000 "$TEXT"

cp -pv $SOURCE_DIRECTORY/TEMPLATE/*wav $DESTINATION_DIRECTORY_SOUNDS/$directory

rm VERSION.txt
