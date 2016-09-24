////////////////////////////////////////////////////////////////////////
//
//  From http://stackoverflow.com/questions/11917124/
// opencv-how-to-use-other-font-than-hershey-with-cvputtext-like-arial
// 
// Compiled with
// $ g++ -o cairo-text-mjpg cairo-text-mjpg.cpp 
//       `pkg-config --cflags --libs opencv cairo`
// 
// putTextCairo modified to fix memory leak.
// VideoWriter still has memory leak.
// 
///////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <cairo/cairo.h>

#include <time.h>

using namespace cv;
using namespace std;

// Reduce memory leak by using statics, by not re-allocating
cairo_surface_t* surfaceLocal = NULL;
cairo_t* cairoLocal = NULL;
cv::Mat cairoTargetLocal;

void putTextCairo(cv::Mat &targetImage,
		  std::string const& text,
		  cv::Point2d centerPoint,
		  std::string const& fontFace,
		  double fontSize,
		  cv::Scalar textColor,
		  bool fontItalic,
		  bool fontBold)
{
    // Create Cairo
    if (surfaceLocal == NULL)
	surfaceLocal = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
						  targetImage.cols,
						  targetImage.rows);
    
    if (cairoLocal == NULL)
	cairoLocal = cairo_create(surfaceLocal);

    
    // Wrap Cairo with a Mat
    if (cairoTargetLocal.empty()) {
	cairoTargetLocal.create(cairo_image_surface_get_height(surfaceLocal),
				   cairo_image_surface_get_width(surfaceLocal),
				   CV_8UC4);
	cairoTargetLocal.data = cairo_image_surface_get_data(surfaceLocal);
	// cairo_image_surface_get_stride(surfaceLocal));
    }
    
    // Put image onto Cairo
    cv::cvtColor(targetImage, cairoTargetLocal, cv::COLOR_BGR2BGRA);
    
    // Set font and write text
    cairo_select_font_face(cairoLocal,
			   fontFace.c_str(),
			   fontItalic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
			   fontBold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
    
    cairo_set_font_size(cairoLocal, fontSize);
    cairo_set_source_rgb(cairoLocal, textColor[2], textColor[1], textColor[0]);
    
    cairo_text_extents_t extents;
    cairo_text_extents(cairoLocal, text.c_str(), &extents);
    
    cairo_move_to(cairoLocal,
		  centerPoint.x - extents.width/2 - extents.x_bearing,
		  centerPoint.y - extents.height/2- extents.y_bearing);
    cairo_show_text(cairoLocal, text.c_str());
    
    // Copy the data to the output image
    cv::cvtColor(cairoTargetLocal, targetImage, cv::COLOR_BGRA2BGR);
    
    // Don't destroy anything...
    // cairo_destroy(cairo);
    // cairo_surface_destroy(surface);
}


int main(int argc, char*argv[])
{
    string outFile ("/tmp/mjpg1/out.mjpg");
    int mjpgSizeX = 320;
    int mjpgSizeY = 240;
    bool mjpgIsColored = true;
    bool updateWindow = false;
    
    time_t current_time;
    struct tm * time_info;
    char timeString[40];

    if (argv[1])
	outFile = argv[1];
    
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

	// create a new 320x240 image
    //Mat canvas(Size(320,240),CV_8UC3);
    //Mat testROI(canvas, Rect(10,10,100,100));
	Mat canvas;
    
    if (updateWindow)
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

	//canvas = Scalar(0, 0, 0);
	//testROI = Scalar(192, 64, 0);
	cap >> canvas;
	rectangle(canvas, Point(100, 100), Point(120, 120), Scalar(0, 0, 255));

	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%a %h %e %H:%M:%S", time_info);

	putText(canvas, "Hello", Point(30,30), 
		FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250),
		1, CV_AA);
	
	putTextCairo(canvas, timeString, Point(150,150), 
		     "helvetica", 20, Scalar(0,255,0), false, false);
	
	if (updateWindow)
	    imshow("DrawView", canvas);

	VideoWriter outStream(outFile, CV_FOURCC('M','J','P','G'), 2,
			      Size(mjpgSizeX, mjpgSizeY), mjpgIsColored);
	if (outStream.isOpened()){
	    outStream.write(canvas);
	    outStream.release();
	} else {
	    cout << "ERROR: Can't write to " << outFile << "!\n";
	    return -1;
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

        if (waitKey(30) >= 0) break;
    }

    return 0;
}
