#!/bin/sh
# Sound file generator script for mac os x
#
#    Copyright (C) 2015 Jochen Kielkopf
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
SCRIPT_NAME="OpenTX Sound File Generator - Mac OSX"
SCRIPT_VERSION="1.0"
SCRIPT_DATE="2015/09/28"
SCRIPT_AUTHER="Jochen Kielkopf"
SCRIPT_AUTHER_CONTACT="https://github.com/Clooney82/MavLink_FrSkySPort"
# -------------------------------------------------------------------------------------------------
# Mac OS X voices for using with the ‘say’ command
# Here is a list of the other voices you can also use:
#
# # English (US) Female Voices
# say -v Agnes "Agnes, hello world"
# say -v Allison "Allison, hello world"
# say -v Ava "Ava, hello world"
# say -v Kathy "Kathy, hello world"
# say -v Princess "Princess, hello world"
# say -v Samantha "Samantha, hello world"  # Siri's Voice
# say -v Susan "Susan, hello world"
# say -v Vicki "Vicki, hello world"
# say -v Victoria "Victoria, hello world"
#
# # English (US) Male Voices
# say -v Alex "Alex, hello world"
# say -v Bruce "Bruce, hello world"
# say -v Fred "Fred, hello world"
# say -v Junior "Junior, hello world"
# say -v Ralph "Ralph, hello world"
# say -v Tom "Tom, hello world"
#
# # English (UK) Female Voices
# say -v Kate "Kate, hello world"
# say -v Serena "Serena, hello world"
#
# # English (UK) Male Voices
# say -v Daniel "Daniel, hello world"
# say -v Oliver "Oliver, hello world"
#
# # German Female Voices
# say -v Anna "Anna, Hallo Welt." # Siri's Voice in Germany
# say -v Petra "Petra, Hallo Welt."
# say -v Steffi "Steffi, Hallo Welt."
#
# # German Male voices
# say -v Markus "Markus, Hallo Welt."
# say -v Yannick "Yannick, Hallo Welt."
#
# Portugese (Portugal) voices
# say -v Catarina "Catarina, Olá Mundo"
# say -v Joana "Joana, hello world"
#
# # Novelty Voices (US)
# say -v Albert "Albert, hello world"
# say -v "Bad News" "Bad News, hello world"
# say -v Bahh "Bahh, hello world"
# say -v Bells "Bells, hello world"
# say -v Boing "Boing, hello world"
# say -v Bubbles "Bubbles, hello world"
# say -v Cellos "Cellos, hello world"
# say -v Deranged "Deranged, hello world"
# say -v "Good News" "Good News, hello world"
# say -v Hysterical "Hysterical, hello world"
# say -v "Pipe Organ" "Pipe Organ, hello world"
# say -v Trinoids "Trinoids, hello world"
# say -v Whisper "Whisper, hello world"
# say -v Zarvox "Zarvox, hello world"
# =================================================================================================
# Notes:
#
# =================================================================================================
SOURCE_DIRECTORY="$(dirname $0)"
#SOURCE_FILE="$(ls $SOURCE_DIRECTORY/OpenTX*.csv)"  # "$(dirname $0)/OpenTX 2.x - Voice and Sound Pack Generator - Definitions - MAIN.csv"  # "$(dirname $0)/SOUNDS/sounds.csv"  # CSV File with the phrases & file names.
SOURCE_FILE="$1"
DESTINATION_DIRECTORY_SOUNDS="$SOURCE_DIRECTORY/SOUNDS"
VOICE="Samantha"
#VOICE="Anna"
LANG="en"		# supported languages: en, de, pt
#LANG="de"
#LANG="pt"
Hz="32000"  # sample rate to encode the audio at, noted in Hz. 32000Hz = 32KHz. Higher is higher quality (16000, 22000, 32000).

# =================================================================================================
# Functions
# =================================================================================================
checkFolder() { [ -d "$@" ] || mkdir -p "$@"; }

# =================================================================================================
# Main Program #
# =================================================================================================
echo ""
echo "$SCRIPT_NAME"
echo "================================================================================"
echo "Version:        $SCRIPT_VERSION"
echo "Date:           $SCRIPT_DATE"
echo "Written By:     $SCRIPT_AUTHER"
echo "                $SCRIPT_AUTHER_CONTACT"
echo "Description:    Creates openTX compatible soundfiles with macs osx's built in"
echo "                say command based on cvs file."
echo "================================================================================"
if [ -z "$1" ]; then
	echo "General Usage: sh ./opentx_voice_and_sound_pack_generator_script_mac-osx.sh INPUTFILE.csv"
	exit 9
else
	echo "Source File:           $SOURCE_FILE"
	echo "Destination Directory: $DESTINATION_DIRECTORY_SOUNDS"
fi
echo "================================================================================"


if [ -f "$SOURCE_FILE" ]; then				# Check Soure File
	#echo "" >> "$SOURCE_FILE"						# Add blank newline to source file so the script reads everything before comeing to END OF FILE.
	if [ "$LANG" == "en" ]; then
		TEXT="Hello, my name is $VOICE, I will be the voice of your sound pack. Building Sound Pack now."
		echo "$TEXT"
		say -v "$VOICE" "$TEXT"
		i=0
	elif [ "$LANG" == "de" ]; then
		TEXT="Hallo, mein Name ist $VOICE, Ich werde die Stimme Deines Sound Paket sein. Erstelle Sound Paket jetzt."
		echo "$TEXT"
		say -v "$VOICE" "$TEXT"
		i=1
	elif [ "$LANG" == "pt" ]; then
		TEXT="Olá, meu nome é $VOICE, eu vou ser a voz de seu pacote de som. Edifício pacote de som agora."
		echo "$TEXT"
		say -v "$VOICE" "$TEXT"
		i=2
	else
		LANG="en"
		i=0
		TEXT="Hello, my name is $VOICE, I will be the voice of your sound pack. Your Language is not supported. Building English Sound Pack now."
		echo "$TEXT"
		say -v "$VOICE" "$TEXT"
	fi
echo $LANG
	echo "================================================================================"
	checkFolder "$DESTINATION_DIRECTORY_SOUNDS"							# Check Destination Folder

	# Read and proccess CSV file
	IFS=";"  # File Delimiter
	while read directory file_name text_to_say_en text_to_say_de text_to_say_pt; do
		directory="$LANG/$VOICE$directory"
		text_to_say[0]="$text_to_say_en"
		text_to_say[1]="$text_to_say_de"
		text_to_say[2]="$text_to_say_pt"

		# Check that varable contain data
		if [ "$directory" != "Directory" ] && [ -n "$directory" ] && [ -n "$file_name" ] && [ -n "${text_to_say[$i]}" ] && [ "${text_to_say[$i]}" != "NOT_AVAILABLE" ]; then
			echo "$DESTINATION_DIRECTORY_SOUNDS/$directory/$file_name.wav\t|\t${text_to_say[$i]}"

			# Check and Create Folder
			checkFolder "$DESTINATION_DIRECTORY_SOUNDS/$directory"

			# Create Voice File
			say -o "$DESTINATION_DIRECTORY_SOUNDS/$directory/$file_name.wav" -v "$VOICE" --data-format=LEI16@32000 "${text_to_say[$i]}"
		else
			echo "[ $directory ] $file_name ... ${text_to_say[$i]}"
		fi
	done < "$SOURCE_FILE"
	say -o "$DESTINATION_DIRECTORY_SOUNDS/$LANG/tada.wav" -v "$VOICE" --data-format=LEI16@32000 "tada!!!"
	echo "================================================================================"
	echo "Coping Sound Files Info [$SOURCE_FILE] -> [$DESTINATION_DIRECTORY_SOUNDS/sounds.csv]"
	cp "$SOURCE_FILE" "$DESTINATION_DIRECTORY_SOUNDS/sounds.csv"
	echo "================================================================================"
	afinfo "$DESTINATION_DIRECTORY_SOUNDS/$LANG/tada.wav"
	rm $DESTINATION_DIRECTORY_SOUNDS/$LANG/tada.wav

	dirs=`ls -1d $DESTINATION_DIRECTORY_SOUNDS/$LANG/$VOICE* | wc -l`
	if [ $dirs -gt 1 ]; then
		del=`ls -1d $DESTINATION_DIRECTORY_SOUNDS/$LANG/$VOICE* | tail -1`
		rm -rf $del
	fi
	echo "================================================================================"
	if [ "$LANG" == "en" ]; then
		TEXT="Build Complete, Audio Files Ready!"
		echo "$TEXT"
		say -v "$VOICE" "$TEXT"
	elif [ "$LANG" == "de" ]; then
		TEXT="Erstellung abgeschlossen, Audio Dateien liegen bereit!"
		echo "$TEXT"
		say -v "$VOICE" "$TEXT"
	elif [ "$LANG" == "pt" ]; then
			TEXT="Construir completo, arquivos de áudio Pronto!"
			echo "$TEXT"
			say -v "$VOICE" "$TEXT"
	else
		TEXT="Hello, my name is $VOICE, I will be the voice of your sound pack. Building Sound Pack now."
		echo "$TEXT"
		say -v "$VOICE" "$TEXT"
	fi
else
	if [ "$LANG" == "en" ]; then
		TEXT="Hello, my name is $VOICE, I would create your sound pack but I can't find the source file needed to create it. Please check your configuration in the script, and the title of your source file."
		echo "$TEXT"
		say -v "$VOICE" "$TEXT"
	elif [ "$LANG" == "de" ]; then
		TEXT="Hallo, mein Name ist $VOICE, Ich würde gerne Deine Sound Paket erstellen, aber ich finde die Quelldatei für die Erstellung nicht. Bitte prüfe Deine Konfiguration im Skript und den Name Deiner Quelldatei."
		echo "$TEXT"
		say -v "$VOICE" "$TEXT"
	elif [ "$LANG" == "pt" ]; then
			TEXT="Olá, meu nome é de R $ VOICE, gostaria de criar seu pacote de som, mas não consigo encontrar o arquivo de fonte necessário para criá-lo. Por favor, verifique a configuração no script, eo título de seu arquivo de origem."
			echo "$TEXT"
			say -v "$VOICE" "$TEXT"
	else
		TEXT="Hello, my name is $VOICE, I will be the voice of your sound pack. Building Sound Pack now."
		echo "$TEXT"
		say -v "$VOICE" "$TEXT"
	fi
fi
