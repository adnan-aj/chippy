#!/bin/bash

# Speech-to-Text test script - converts recorded flacfile into text
# using IBM Bluemix Watson STT services.
# Adnan adnan@singnet.com.sg
# Was tested by auto-recording using sox record
#   rec -c 1 -r 16000 testfile.flac silence 1 0.1 0.19% 1 1.5 0.3%

echo param 1 = $1

filename=$(basename "$1")
extension="${filename##*.}"
shortname="${filename%.*}"
savename=${shortname}.txt
#echo filename = ${filename}
#echo extension = ${extension}
#echo shortname = ${shortname}
#echo savename = ${savename}

# TTS_USERPASS must be set before using this command

#curl --silent \
curl \
 -X POST -u ${STT_USERPASS} \
 --header "Content-Type: audio/$extension" \
 --header "Transfer-Encoding: chunked" \
 --data-binary @$1 \
 --output ${savename} \
 "https://stream.watsonplatform.net/speech-to-text/api/v1/recognize?continuous=true" \
