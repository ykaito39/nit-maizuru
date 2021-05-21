/*********************************************************************************
 * Ver0.3 
 * 新機能:	動くよ!!!
 * 			あと時間をCVのWindowに出力できるようにしたよ!!!
 *			色認識のしきい値をトラックバーで変更できるようにしたよ!!
 *********************************************************************************/
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define CAP_W 640
#define CAP_H 480

using namespace cv;
using namespace std;

//色認識のしきい値
int val_color_s_min_now = 100;
int val_color_s_max_now = 125;
int circle_size = 200;

int main(int argc, char *argv[]){
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
	
		Mat frame_plus = frame.clone(); //これにいろいろ追加していく

		//ノイズ処理==========================================
		Mat blur_img;
		GaussianBlur(frame, blur_img, Size(3, 3), 2);

		//cursorの描画----------------------------
		line(frame_plus, Point(0, CAP_H/2), Point(CAP_W, CAP_H/2), Scalar(0, 200, 0), 2, 4);
		line(frame_plus, Point(CAP_W/2, 0), Point(CAP_W/2, CAP_H), Scalar(0, 200, 0), 2, 4);
		circle(frame_plus, Point(CAP_W/2, CAP_H/2), circle_size, Scalar(0, 200, 0), 2, 4);
		
		//青のみ抽出する======================================
		Mat hsv_img;
		cvtColor(blur_img, hsv_img, CV_BGR2HSV);			//hsvに変換
		Mat color_zone_img = Mat(Size(CAP_W, CAP_H), CV_8UC1); 	//1channel,8bit??のblue_img
		uchar hue, sat;								//h,s,v
			//hue:色相
			//sat:彩度
			//val:明度
		for(int y = 0; y < CAP_H; y++){
			for(int x = 0; x < CAP_W; x++){
				//hsvを分離--------------------------------------
				hue = hsv_img.at<Vec3b>(y,x)[0];
				sat = hsv_img.at<Vec3b>(y,x)[1];
				//blueのみ抽出,nichika---------------------------
				if( (hue > val_color_s_min_now && hue < val_color_s_max_now) && sat > 100)
					color_zone_img.at<uchar>(y,x) = 0;
				else
					color_zone_img.at<uchar>(y,x) = 255;
			}
		}

		//yoshida & daigou Method======================================
		int offset_x = (CAP_W/2),
			offset_y = (CAP_H/2);
		int tmp, tmp1;	//temp:minuts  tempo:hourに使う
		//長針判定-----------------------------------------------------
		for (int r = 160; r > 100; r--){
			for(double i=CV_PI/180; i <= 2*CV_PI; i+=CV_PI/180){
				int x = ((double)r*cos(i+CV_PI/2) + (double)offset_x);
				int y = ((double)r*(-sin(i+CV_PI/2)) + (double)offset_y);
				tmp = (int)(30*(2*CV_PI-i)/CV_PI);
				if(color_zone_img.at<uchar>(y,x) == 0){
					circle(frame_plus, Point(x, y), 5, Scalar(200, 200, 0), 3, 1);
					r = 0; //break
					break;
				}
			}
		}
		//短針判定-----------------------------------------------------
		for (int r = 120; r > 50; r--){
			//tannshin
			for(double i=CV_PI/180; i <= 2*CV_PI; i+=CV_PI/180){
				int x = ((double)r*cos(i+CV_PI/2) + (double)offset_x);
				int y = ((double)r*(-sin(i+CV_PI/2)) + (double)offset_y);
				tmp1 = (int)(30*(2*CV_PI-i)/CV_PI);
				if ((abs(tmp - tmp1) >= 4) && (color_zone_img.at<uchar>(y,x) == 0)){
					circle(frame_plus, Point(x, y), 5, Scalar(0, 0, 200), 3, 1);
					r = 0; //break
					break;
				}
			}
		}
					
		//etc proc=======================================================
		//screen time----------------------
		char daigou[5000];
		sprintf(daigou, "「おにぃちゃん?もう%d:%dだよ?? はやく起きてよね!」\t\tCV:大郷拓也", tmp1/5, tmp);
		cout << daigou << endl;
		//cout << "「おにぃちゃん?もう" << tmp1/5 << ":" << tmp << "だよ?? はやく起きてよね!」\t\tCV:大郷拓也" << endl;
		char time[500];
		rectangle(frame_plus, Point(560, 455), Point(640, 490), Scalar(255, 255, 255), -1, CV_AA);
		sprintf(time, "%d:%0d", tmp1/5, tmp);
		putText(frame_plus,time, Point(560, 475), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(10, 10, 10), 1, CV_AA);
		//rectangle(frame_plus, Point(0, 455), Point(640, 490), Scalar(255, 255, 255), -1, CV_AA);
		//putText(frame_plus,time, Point(0, 475), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(10, 10, 10), 1, CV_AA);
		//resize---------------------------
		Mat re_color_zone_img;
		Mat re_shot;
		resize(color_zone_img, re_color_zone_img, Size(), 0.6, 0.6);
		resize(frame_plus, re_shot, Size(), 0.6, 0.6);

		//iroiro=========================================================
		//key read-------------------------
		char key = waitKey(1);
		if (key == 'q')			//終了
			break;
		else if(key == 's'){	//screenshot	
			namedWindow("shot");
			imshow("shot", re_shot);
		}
		//screen---------------------------
		namedWindow ("frame");
		imshow("frame", frame_plus);
			createTrackbar("circle_size", "frame", &circle_size, 400);

		namedWindow("color_zone");
		imshow("color_zone", re_color_zone_img);
			//trackbar-------------------------
			createTrackbar("color_min", "color_zone", &val_color_s_min_now, 255);
			createTrackbar("color_max", "color_zone", &val_color_s_max_now, 255);

//		namedWindow("blur");
//		imshow("blur", blur_img);
	}
	return 0;
}
