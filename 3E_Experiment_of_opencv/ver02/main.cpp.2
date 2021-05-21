#include <iostream>
#include <math.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define CAP_W 640
#define CAP_H 480

using namespace cv;
using namespace std;


int main(int argc, char * argv[]){
	//capture Open================================================
	VideoCapture cap(0);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, CAP_W);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAP_H);
	if(!cap.isOpened())
		return -1;

	//Capture=====================================================
	while(1){
		Mat frame;		//Capture frame data
		cap >> frame;	//frameにcapを読み取る。
	
		Mat frame_plus = frame.clone(); //frameを複製

		//cursorの描画
		line(frame_plus, Point(0, 240), Point(640, 240), Scalar(0, 200, 0), 2, 4);
		line(frame_plus, Point(320, 0), Point(320, 480), Scalar(0, 200, 0), 2, 4);
		circle(frame_plus, Point(320, 240), 200, Scalar(0, 200, 0), 2, 4);
		

		namedWindow ("frame", CV_WINDOW_AUTOSIZE);
		imshow("frame", frame_plus);

		//key read================================================
		char key = waitKey(1);
		if (key == 'q')			//終了
			break;
		else if(key == 's'){	//screenshot
			Mat frame_shot = frame;
			Mat blur_img;
			Mat hsv_img;
			
			//ノイズ処理==========================================
			GaussianBlur(frame_shot, blur_img, Size(19,19), 100);

			//青のみ抽出する======================================
			cvtColor(blur_img, hsv_img, CV_BGR2HSV);			//hsvに変換
			Mat blue_img = Mat(Size(CAP_W, CAP_H), CV_8UC1); 	//1channel,8bit??のblue_img
			uchar hue, sat;								//h,s,v
				//hue:色相
				//sat:彩度
				//val:明度
			for(int y = 0; y < CAP_H; y++){
				for(int x = 0; x < CAP_W; x++){
					//hsvを分離------------------------------
					hue = hsv_img.at<Vec3b>(y,x)[0];
					sat = hsv_img.at<Vec3b>(y,x)[1];
					//val = hsv_img.at<Vec3b>(y,x)[2];
					//blueのみ抽出,nichika---------------------------
					if( (hue > 99 && hue < 115) && sat > 100)
						blue_img.at<uchar>(y,x) = 0;
					else
						blue_img.at<uchar>(y,x) = 255;
				}
			}
			namedWindow("shot", CV_WINDOW_AUTOSIZE);
			imshow("shot", blue_img);

			//大郷メソッド========================================
			//cyousinn
			double r = 120.0;
			int offset_x = (CAP_W/2),
				offset_y = (CAP_H/2);
			for(double i=CV_PI/180; i <= 2*CV_PI; i+=CV_PI/180){
				//cout << i << endl;
				int x = ((double)r*cos(i+CV_PI/2) + (double)offset_x);
				int y = ((double)r*(-sin(i+CV_PI/2)) + (double)offset_y);
				//debug
				if(blue_img.at<uchar>(y,x) == 0){
					//cout << "rotate: " << i << "度は青色です。" << endl;
					cout << "i: " << i  << "  " << (int)(30*(2*CV_PI-i)/CV_PI) << "minuts" << endl;
					circle(frame_plus, Point(x, y), 5, Scalar(0, 0, 200), 1, 1);
					namedWindow("debug");
					imshow("debug", frame_plus);
					break;
				}
			}
			//tannshin
			r = 50.0;
			for(double i=CV_PI/180; i <= 2*CV_PI; i+=CV_PI/180){
				//cout << i << endl;
				int x = ((double)r*cos(i+CV_PI/2) + (double)offset_x);
				int y = ((double)r*(-sin(i+CV_PI/2)) + (double)offset_y);
				//debug
				if(blue_img.at<uchar>(y,x) == 0){
					//cout << "rotate: " << i << "度は青色です。" << endl;
					cout << "i: " << i  << "  " << (int)(30/4*(2*CV_PI-i)/CV_PI) << "hour" << endl;
					circle(frame_plus, Point(x, y), 5, Scalar(0, 0, 200), 1, 1);
					namedWindow("debug");
					imshow("debug", frame_plus);
					break;
				}
			}


			//screen==============================================
		//	namedWindow("shot", CV_WINDOW_AUTOSIZE);
		//	imshow("shot", blue_img);
		}
	}
	
	return 0;
}
