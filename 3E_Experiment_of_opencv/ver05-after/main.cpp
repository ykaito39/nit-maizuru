/*********************************************************************************
 * H27年度3E 電気情報工学実験2Bのやつ
 * Ver0.5
 * 開発環境:	gcc 4.8.4
 * 				OpenCV 3.0 たぶんQtで動いてる
 * 				vim 7.4
 *
 * 実装履歴:	2015/12/16	dメソッドの実装
 * 							ついでにyメソッドの実装
 * 				2015/12/17	時間をチャプチャ画面に出力できるようにしたよ!
 *							色認識のしきい値をトラックバーで変更できるようにしたよ!!
 *				2015/12/18	ver0.4に移行
 *							CV:ddd(console)を実装
 *				2015/12/19	円の中心を自動取得できるようになったよ!!!
 *							ver0.5に移行
 *
 * 実装予定: 	水平検出
 *
 *********************************************************************************/
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

//キャプチャ画像のサイズ
#define CAP_W 640
#define CAP_H 480

#define PI CV_PI

//色認識のしきい値
int val_color_h_min_now = 90;	//色相の下限
int val_color_h_max_now = 125;	//色相の上限
int val_color_s_now = 176;
int val_color_v_now = 92;

int circle_size = 200;

//時間空間に戻す前の時間と分, つまりラジアン空間のやつ
double 	long_hand_val  = 0.0,
		short_hand_val = 0.0;

//円検知で使う
int off_x = 320;	//円の原点x
int off_y = 240;	//円の原点y座標
int radius = 200; 	//円の半径

int main(int argc, char *argv[]){
	VideoCapture cap(0);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, CAP_W);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAP_H);
	if(!cap.isOpened()){
		cout << "cant opne" << endl;
		return -1;
	}

	while(1){
		Mat frame;		//Capture frame data
		cap >> frame;	//frameにcapを読み取る。
		Mat frame_plus = frame.clone(); //これにいろいろ追加していく

		//ノイズ軽減--------------------------------
		Mat blur;
		GaussianBlur(frame, blur, Size(9, 9), 2, 2);
		
		//特定の色域のみ抽出する======================================
		Mat hsv_img;
		cvtColor(blur, hsv_img, CV_BGR2HSV);					//hsvに変換
		Mat color_zone_img = Mat(Size(CAP_W, CAP_H), CV_8UC1); 	//1channel,8bit??のblue_img
		uchar hue, sat, val;									//hue:色相	sat:彩度
		for(int y = 0; y < CAP_H; y++){
			for(int x = 0; x < CAP_W; x++){
				//hsを分離--------------------------------------
				hue = hsv_img.at<Vec3b>(y,x)[0];
				sat = hsv_img.at<Vec3b>(y,x)[1];
				val = hsv_img.at<Vec3b>(y,x)[2];
				//trackbarで指定した色域のみ抽出,二値化
				if( (hue > val_color_h_min_now && hue < val_color_h_max_now) && sat > val_color_s_now && val > val_color_v_now)
					color_zone_img.at<uchar>(y,x) = 255;
				else
					color_zone_img.at<uchar>(y,x) = 0;
			}
		}

		//円の検出=================================================
		Mat gray;
		cvtColor(blur, gray, CV_BGR2GRAY);
		vector<Vec3f> circles;
		
		HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, 200, 100, 50, 10);

		double S = 0;	//面積
		double S1 = 1; 	//10円1個の面積

		//i番目に検出した円に対して...
		for( size_t i=0; i < circles.size(); i++){
			//原点座標と半径の取得
			off_x  = cvRound(circles[i][0]);
			off_y  = cvRound(circles[i][1]);
			radius = cvRound(circles[i][2]);
			
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));

			// 円の中心と円のそれを描画
       		circle( frame_plus, center, 3,		Scalar(0,255,0), -1, 8, 0 );
       		circle( frame_plus, center, radius, Scalar(0,0,255), 3, 8, 0 );

			//面積の計算
			S += radius*radius*PI;
			if(i == 0){
				S1 = radius*radius*PI;
				cout << S1 <<endl;
			}
        }

		//面積の合計を表示
		cout << "面積の合計: " << S << endl;

		//
		char key = waitKey(20);
/*		if (key == '1')
			rot = 0;
		
		//etc proc=======================================================
		//put time-------------------------
		char daigou[500];
*/		
		char cS[50];
		rectangle(frame_plus, Point(560, 455), Point(640, 490), Scalar(255, 255, 255), -1, CV_AA);
		sprintf(cS, "%d Yen", (int)S/(int)(S1-200)*10);
		putText(frame_plus, cS, Point(560, 475), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(10, 10, 10), 1, CV_AA);

		//resize---------------------------
		Mat re_color_zone_img;
		Mat re_shot;
		resize(color_zone_img, re_color_zone_img, Size(), 0.6, 0.6);
		resize(frame_plus, re_shot, Size(), 0.6, 0.6);

		//screen---------------------------
		namedWindow ("frame");
		imshow("frame", frame_plus);

		namedWindow("color_zone");
		imshow("color_zone", re_color_zone_img);
			createTrackbar("color_min", "color_zone", &val_color_h_min_now, 255);
			createTrackbar("color_max", "color_zone", &val_color_h_max_now, 255);
			createTrackbar("color_s", "color_zone", &val_color_v_now, 255);
			createTrackbar("color_v", "color_zone", &val_color_s_now, 255);
		
		//key read-------------------------
		if (key == 'q')			//終了
			break;
		else if(key == 's'){	//screenshot	
			namedWindow("shot");
			imshow("shot", frame_plus);
		}
	}
	return 0;
}
