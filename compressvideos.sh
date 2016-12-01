#!/bin/bash

# Compresses all clip videos for for the different platforms Unix, Windows and Android.
# Since different video formats work on different platforms, we need different video files, one for every platform.
# Besides that compressing the clip videos decreases the size of the files which reduces the download time of the game.

find ./clips -type f \( \( -iname "*.avi" -o -iname "*.mkv" \) -and -not -iname "*_compressed*"  \) -print0 | while IFS= read -r -d $'\0' line; do
	echo "$line"
	filename=$(basename "$line")
	extension="${filename##*.}"
	filename="${filename%.*}"
	compressedNameUnix=$(dirname "$line")/"$filename""_compressed_unix.$extension"
	compressedNameWindows=$(dirname "$line")/"$filename""_compressed_windows.$extension"
	compressedNameAndroid=$(dirname "$line")/"$filename""_compressed_android.$extension"
	echo "Unix: $compressedNameUnix"
	echo "Windows: $compressedNameWindows"
	echo "Android $compressedNameAndroid"

	if [ -e "$compressedNameUnix" ] ; then
		rm "$compressedNameUnix"
	fi

	if [ -e "$compressedNameWindows" ] ; then
		rm "$compressedNameWindows"
	fi

	if [ -e "$compressedNameAndroid" ] ; then
		rm "$compressedNameAndroid"
	fi

	# Original resolution: 1920x1080
	# / 2
	#RESOLUTION="960x540" # Keep the ratio of the original videos!
	RESOLUTION="1200x675"

	# Make sure the codec is supported on Android:
	# https://developer.android.com/guide/appendix/media-formats.html
	# Use for sound: aac
	# Use for video codec: h264
	# Use for container: mp4
	# mp3 cant be used with h264 and mp4. It leads to the following error: MPEG4Extractor: MP3 track in MP4/3GPP file is not supported
	# -strict -2 is required to enforce encoding support for aac.
	# Although it ends with .mkv or .avi to use the same filenames as specified in the XML file, it actually uses always mp4 since this is supported by Android.
	ffmpeg -nostdin -i "$line" -s "$RESOLUTION" -b 512k -vcodec h264 -acodec aac -strict -2 -f mp4 "$compressedNameUnix"

	# .wmv for Windows. QtMultimedia uses a different backend on Windows 10 which does not support h264.
	ffmpeg -nostdin -i "$line" -s "$RESOLUTION" -b 512k -vcodec msmpeg4v2 -acodec wmav2 -ac 2 -strict -2 -f avi "$compressedNameWindows"

	# Deinterlaced for Android
	# http://video.stackexchange.com/questions/17396/how-to-deinterlacing-with-ffmpeg
	ffmpeg -nostdin -i "$line" -vf yadif -s "$RESOLUTION" -b 512k -vcodec h264 -acodec aac -strict -2 -f mp4 "$compressedNameAndroid"
done