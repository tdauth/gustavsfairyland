#!/bin/bash

find ./clips -type f \( \( -iname "*.avi" -o -iname "*.mkv" \) -and -not -iname "*_compressed*"  \) -print0 | while IFS= read -r -d $'\0' line; do
	echo "$line"
	filename=$(basename "$line")
	extension="${filename##*.}"
	filename="${filename%.*}"
	compressedName=$(dirname "$line")/"$filename""_compressed.$extension"
	echo "$compressedName"
	if [ -e "$compressedName" ] ; then
		rm "$compressedName"
	fi
	# Make sure the codec is supported on Android:
	# https://developer.android.com/guide/appendix/media-formats.html
	# Use for sound: aac
	# Use for video codec: h264
	# Use for container: mp4
	# mp3 cant be used with h264 and mp4. It leads to the following error: MPEG4Extractor: MP3 track in MP4/3GPP file is not supported
	# -strict -2 is required to enforce encoding support for aac.
	# Although it ends with .mkv or .avi to use the same filenames as specified in the XML file, it actually uses always mp4 since this is supported by Android.
	#ffmpeg -nostdin -i "$line" -s 1280x720 -b 512k -vcodec h264 -acodec aac -strict -2 -f mp4 "$compressedName"
	# Deinterlaced for Android
	# http://video.stackexchange.com/questions/17396/how-to-deinterlacing-with-ffmpeg
	ffmpeg -nostdin -i "$line" -vf yadif -s 1280x720 -b 512k -vcodec h264 -acodec aac -strict -2 -f mp4 "$compressedName"
done