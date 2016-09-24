
cairo-hello.c --
From http://cairographics.org/FAQ/#GettingStarted

Compiled with
g++ -o hello-cairo `pkg-config --libs --cflags cairo` cairo-hello.c

Produces file "hello.png"



cairo-text-mjpg.cpp --

From http://stackoverflow.com/questions/11917124/opencv-how-to-use-other-font-than-hershey-with-cvputtext-like-arial

Compiled with
 g++ -o cairo-text-mjpg cairo-text-mjpg.cpp `pkg-config --cflags --libs opencv cairo`

 Produces mjpeg stream at /tmp/mjpg3/out.mjpg
 Streamed out by mjpg-streamer
 

cairo-time-mjpg.cpp --
Same as above cairo-text-mjpg.cpp, except changes the text to tell the
time.

g++ -o cam-cairo-text-mjpg2 cam-cairo-text-mjpg2.cpp `pkg-config --cflags --libs --static opencv cairo`
