#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

int main( int argc, char** argv )
{
  // Load the image
  Mat imgOriginal = imread( "MyPic.JPG", 1 );

  //show the original image
  const char* pzOriginalImage = "Original Image";
  namedWindow( pzOriginalImage, CV_WINDOW_AUTOSIZE );
  imshow( pzOriginalImage, imgOriginal );

  const char* pzRotatedImage = "Rotated Image";
  namedWindow( pzRotatedImage, CV_WINDOW_AUTOSIZE );

  int iAngle = 180;
  createTrackbar("Angle", pzRotatedImage, &iAngle, 360);

  int iImageHieght = imgOriginal.rows / 2;
  int iImageWidth = imgOriginal.cols / 2;

  while (true)
    {
      Mat matRotation = getRotationMatrix2D( Point(iImageWidth, iImageHieght), (iAngle - 180), 1 );
 
      // Rotate the image
      Mat imgRotated;
      warpAffine( imgOriginal, imgRotated, matRotation, imgOriginal.size() );

      imshow( pzRotatedImage, imgRotated );

      int iRet = waitKey(30);
      if ( iRet == 27 )
	{
	  break;
	}
    }

  return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
