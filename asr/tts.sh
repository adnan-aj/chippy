#!/bin/bash

# Text-to-Speech test script - convert text into wavfile
# using IBM Bluemix Watson TTS services.
# Adnan adnan@singnet.com.sg


#echo param 1 = $1
#echo param 2 = $2
filename=$(basename "$1")
extension="${filename##*.}"
shortname="${filename%.*}"
datetxt=$(date +%Y%m%d%H%M%S)
#echo data = $datetxt
#echo extension = $extension
#echo basename = $shortname
#echo savename = $shortname.txt

# TTS_USERPASS must be set before using this command

curl --silent \
 -X POST -u ${TTS_USERPASS} \
 --header "Content-Type: application/json" \
 --header "Accept: audio/wav" \
 --data "{\"text\":\"$2\"}" \
 --output ${shortname}.wav \
 "https://stream.watsonplatform.net/text-to-speech/api/v1/synthesize?voice=en-US_AllisonVoice" \
