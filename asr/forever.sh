#!/bin/bash

# Loop recording script - work in progress
# for using stt.sh script to test IBM Bluemix Watson STT service.
# Adnan adnan@singnet.com.sg
# expanded from auto-recording using sox record
#   rec -c 1 -r 16000 testfile.flac silence 1 0.1 0.19% 1 1.5 0.3%

# References
# http://ymkimit.blogspot.sg/2014/07/recording-sound-detecting-silence.html
# http://digitalcardboard.com/blog/2009/08/25/the-sox-of-silence/


# The above-periods value is used to indicate if audio should be trimmed at
# the beginning of the audio. A value of zero indicates no silence should be
# trimmed from the beginning. When specifying an non-zero above-periods, it
# trims audio up until it finds non-silence. Normally, when trimming silence
# from beginning of audio the above-periods will be 1 but it can be increased
# to higher values to trim all audio up to a specific count of non-silence
# periods.
above_period=1
# When above-periods is non-zero, you must also specify a duration and
# threshold. Duration indications the amount of time that non-silence must
# be detected before it stops trimming audio. By increasing the duration,
# burst of noise can be treated as silence and trimmed off
start_duration=0.1
start_threshold=0.19% # for my quiet office-workshop

# When optionally trimming silence from the end of the audio, you specify a
# below-periods count. In this case, below-period means to remove all audio
# after silence is detected. Normally, this will be a value 1 of but it can
# be increased to skip over periods of silence that are wanted. For example,
# if you have a song with 2 seconds of silence in the middle and 2 second at
# the end, you could set below-period to a value of 2 to skip over the 
# silence in the middle of the audio.
below_period=1
# For below-periods, duration specifies a period of silence that must exist
# before audio is not copied any more. By specifying a higher duration,
# silence that is wanted can be left in the audio. 
end_duration=1.5
end_threshold=0.3% # for my quiet office-workshop

while  true
do
    /usr/bin/rec -c 1 -r 16000 phrase.flac silence \
	$above_period $start_duration $start_threshold \
	$below_period $end_duration $end_threshold

    DATE=$(date +%Y%m%d%H%M%S)
    mv phrase.flac ${DATE}-event.flac
done
