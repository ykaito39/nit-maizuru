#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    Mat img, gray;
    img=imread("img.png");
	if(img.empty())
		return -1;

    GaussianBlur( img, gray, Size(9, 9), 2, 2 );
    cvtColor(gray, gray, CV_BGR2GRAY);
    vector<Vec3f> circles;
    HoughCircles(gray, circles, CV_HOUGH_GRADIENT,
                 2, gray.rows/2, 200, 100 );
    for( size_t i = 0; i < circles.size(); i++ )
    {
         Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
         int radius = cvRound(circles[i][2]);
         // 円の中心を描画
         circle( img, center, 3, Scalar(0,255,0), -1, 8, 0 );
         // 円を描画
         circle( img, center, radius, Scalar(0,0,255), 3, 8, 0 );
    }
    namedWindow( "circles" );
    imshow( "circles", img );
	waitKey(0);
    return 0;
}
