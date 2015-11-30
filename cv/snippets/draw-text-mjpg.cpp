////////////////////////////////////////////////////////////////////////
//
// Compiling with
// 
//  g++ -o draw-text-mjpg draw-text-mjpg.cpp 
//    `pkg-config --cflags --libs opencv` fonts/fonts.a
// 
///////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstring>

#include "opencv2/opencv.hpp"
#include "fonts/linux/font.h"

using namespace cv;
using namespace std;

#define DECLARE_TIMING(s)  int64 timeStart_##s; double timeDiff_##s; double timeTally_##s = 0; int countTally_##s = 0
#define START_TIMING(s)    timeStart_##s = cvGetTickCount()
#define STOP_TIMING(s)     timeDiff_##s = (double)(cvGetTickCount() - timeStart_##s); timeTally_##s += timeDiff_##s; countTally_##s++
#define GET_TIMING(s)      (double)(timeDiff_##s / (cvGetTickFrequency()*1000.0))
#define GET_AVERAGE_TIMING(s)   (double)(countTally_##s ? timeTally_##s/ ((double)countTally_##s * cvGetTickFrequency()*1000.0) : 0)
#define CLEAR_AVERAGE_TIMING(s) timeTally_##s = 0; countTally_##s = 0

DECLARE_TIMING(myTimer);

void putTextLinux( Mat& targetImage, const string& text, Point org,
		   int fontFace, double fontScale, Scalar color,
		   int thickness=1, int lineType=8,
		   bool bottomLeftOrigin=false );

int main(int argc, char*argv[])
{
    string outFile ("/tmp/mjpg3/out.mjpg");
    int mjpgSizeX = 320;
    int mjpgSizeY = 240;
    bool mjpgIsColored = true;
    bool updateWindow = true;
    
    string testString = "Hello Console Font";

    if (argv[1])
	outFile = argv[1];
    
    // create a new 320x240 image
    Mat canvas(Size(320,240),CV_8UC3);
    Mat testROI(canvas, Rect(10,10,100,100));

    int i, j;
    Vec3b forecolor(255, 0, 128);
    for (i = 90; i < 98; i++)
	for (j = 90; j < 105; j++) {
	        canvas.at<Vec3b>(i, j) = forecolor;;
	    #if 0
	        canvas.at<Vec3b>(i, j)[0] = 255;
	        canvas.at<Vec3b>(i, j)[1] = 255;
	        canvas.at<Vec3b>(i, j)[2] = 255;
	    #endif
	}

    namedWindow("DrawView", 1);

    // fps counter begin
    time_t start, end;
    int counter = 0;
    double sec;
    double fps;
    // fps counter end
    
        
    for(;;)
    {
	if (counter == 0) {
	    time(&start);
	}
	
	canvas = Scalar(0, 0, 0);
	testROI = Scalar(192, 64, 0);
	rectangle(canvas, Point(100, 100), Point(120, 120), Scalar(0, 0, 255));

	putTextLinux(canvas, "FPS = images.", cvPoint(30,30), 
		     FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250),
		     1, CV_AA);
	
	if (updateWindow)
	    imshow("DrawView", canvas);

	VideoWriter outStream(outFile, CV_FOURCC('M','J','P','G'), 2,
			      Size(mjpgSizeX, mjpgSizeY), mjpgIsColored);
	if (outStream.isOpened()){
	    outStream.write(canvas);
	} else {
	    cout<<"ERROR: Can't write to "<<outFile<<"!\n";
	    return -1;
	}
	
	// fps counter begin
	time(&end);
	counter++;
	sec = difftime(end, start);
	fps = counter/sec;
	cout.precision(2);
	cout << fixed << fps << " fps\n";
	if (counter == (INT_MAX - 1000)) counter = 0;
	// fps counter end

        if (waitKey(30) >= 0) break;
    }

    return 0;
}

extern const struct font_desc *fonts[];
int fontn = 3;
int cur_x = 50;
int cur_y = 50;
int SSD1306_LCDWIDTH = 320;
int SSD1306_LCDHEIGHT = 240;

void putTextLinux( Mat& targetImage, const string& text, Point org,
		   int fontFace, double fontScale, Scalar color,
		   int thickness, int lineType,
		   bool bottomLeftOrigin )
#if 0
void putTextFontn(cv::Mat &targetImage,
		  std::string const& text,
		  cv::Point2d centerPoint,
		  std::string const& fontFace,
		  double fontSize,
		  cv::Scalar textColor,
		  bool fontItalic = false,
		  bool fontBold = false)
#endif
{
    int x, y, height, width;
    unsigned char *p;
    const struct font_desc *font;
    Vec3b foreColor(255, 255, 128);
  
    int c = 'A';
    
    font = fonts[fontn];
    width = font->width;
    height = font->height;
    p = (unsigned char *) font->data;

    if (width <= 8)
	p += (c * height); // font height
    else
	p += (c * height * 2); // font height
    
    // font height
    for (y = 0; y < height; y++) {
	// font width
	for (x = 0; x < width; x++) {
	    if (width <= 8) {
#if 0
		drawPixel(cur_x + x, cur_y + y,
		      ((1 << (8 - 1 - x)) & *(p + y)) ? WHITE : 0);
#else
		if ((1 << (8 - 1 - x)) & *(p + y))
		    targetImage.at<Vec3b>(x, y) = foreColor;
#endif
		
	    }
	    else {
		if (x < 8) {
		    #if 0
		    drawPixel(cur_x + x, cur_y + y,
			      ((1 << (8 - 1 - x)) & *(p + (y * 2))) ? WHITE : 0);
#else
		    
#endif
		} 
		else {
#if 0
		    drawPixel(cur_x + x, cur_y + y,
			      ((1 << (16 - x - 1)) & *(p + (y * 2) + 1)) ? WHITE : 0);
#else
		    
#endif
		    
		}
	    }
	}
    }
    
    // update pixel cursor
    cur_x += width;
    if (cur_x > SSD1306_LCDWIDTH) {
	cur_x = 0;
	cur_y += height;
    }
    if (cur_y > SSD1306_LCDHEIGHT) {
	cur_y = 0;
    }
#if 0
    if (update)
	display();
#endif
}
