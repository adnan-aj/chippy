// testing file editing through samba from adnan-lt windows

////////////////////////////////////////////////////////////////////////
//
//  From
// 
// Compiled with
// $ g++ -o mjpg-camview mjpg-camview.cpp `pkg-config --cflags --libs opencv`
// 
// 
// 
// 
// 
///////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstring>

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int argc, char*argv[])
{
    string outFile ("/tmp/mjpg3/xxx.mjpg");
    
    int resizeFactor = 1;
    bool doBGS = false;
    bool writeOut = true;
    bool displayWindows = false;
    
    int imgSizeX = resizeFactor * 320;
    int imgSizeY = resizeFactor * 240;
    bool isOutputColored = true;
    
    if (argv[1])
	outFile = argv[1];
    
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    
    // fps counter begin
    time_t start, end;
    int counter = 0;
    double sec;
    double fps;
    // fps counter end
    
    Mat edges;
    
    if (displayWindows)
	namedWindow("Cam View", 1);

    for(;;)
    {
        Mat frame;

	if (counter == 0) {
	    time(&start);
	}
	
        cap >> frame; // get a new frame from camera
//	cvtColor(frame, edges, CV_BGR2GRAY);
//	GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
//	Canny(edges, edges, 0, 30, 3);
//	imshow("edges", frame);
	
	if (writeOut) {
	    VideoWriter outStream(outFile, CV_FOURCC('M','J','P','G'), 2, 
				  Size(imgSizeX, imgSizeY), isOutputColored);
	    if (outStream.isOpened()){
		outStream.write(frame);
	    } else {
		cout << "ERROR: Can't write to " << outFile << endl;
		return -1;
	    }
        }

	// fps counter begin
	time(&end);
	counter++;
	sec = difftime(end, start);
	fps = counter/sec;
	cout.precision(2);
	cout<<fixed<<fps<<" fps\n";
	if (counter == (INT_MAX - 1000)) counter = 0;
	// fps counter end

        //if (waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
