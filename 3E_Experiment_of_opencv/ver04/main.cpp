/*********************************************************************************
 * H27年度3E 電気情報工学実験2Bのやつ
 * Ver0.4 
 * 実装履歴:	2015/12/16	dメソッドの実装
 * 							ついでにyメソッドの実装
 * 				2015/12/17	時間をチャプチャ画面に出力できるようにしたよ!
 *							色認識のしきい値をトラックバーで変更できるようにしたよ!!
 *				2015/12/18	ver0.4に移行
 *							CV:ddd(console)を実装
 *				2015/12/19	円の中心を自動取得化できるようになったよ!!!
 *
 * 実装予定: 	水平検出
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

//色認識のしきい値
int val_color_s_min_now = 90;	//色相の下限
int val_color_s_max_now = 125;	//色相の上限
//int circle_size = 200;

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
		
		//cursorの描画------------------------------
/*		line(frame_plus, Point(0, CAP_H/2), Point(CAP_W, CAP_H/2), Scalar(0, 200, 0), 2, 4);
		line(frame_plus, Point(CAP_W/2, 0), Point(CAP_W/2, CAP_H), Scalar(0, 200, 0), 2, 4);
		circle(frame_plus, Point(CAP_W/2, CAP_H/2), circle_size, Scalar(0, 200, 0), 2, 4);
*/		
		//特定の色域のみ抽出する======================================
		Mat hsv_img;
		cvtColor(blur, hsv_img, CV_BGR2HSV);					//hsvに変換
		Mat color_zone_img = Mat(Size(CAP_W, CAP_H), CV_8UC1); 	//1channel,8bit??のblue_img
		uchar hue, sat;											//hue:色相	sat:彩度
		for(int y = 0; y < CAP_H; y++){
			for(int x = 0; x < CAP_W; x++){
				//hsを分離--------------------------------------
				hue = hsv_img.at<Vec3b>(y,x)[0];
				sat = hsv_img.at<Vec3b>(y,x)[1];
				//trackbarで指定した色域のみ抽出,二値化
				if( (hue > val_color_s_min_now && hue < val_color_s_max_now) && sat > 100)
					color_zone_img.at<uchar>(y,x) = 0;
				else
					color_zone_img.at<uchar>(y,x) = 255;
			}
		}

		//円の検出=================================================
		Mat gray;
		cvtColor(blur, gray, CV_BGR2GRAY);
		vector<Vec3f> circles;
		HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, 200, 100, 100, 50);
		
		int off_x = 0;		//円の原点x
		int off_y = 0;		//円の原点y座標
		int radius = 200; 	//円の半径
		
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
        }

		//yoshida & daigou Method======================================
		int long_hand_val,
			short_hand_val;
		const double dig1  	= CV_PI/180,
			  		 dig360 = CV_PI*2,
					 dig90  = CV_PI/2;
		//長針判定-------------------------
		//外側から内側に向かって絞っていく
		for (int r = radius*4/5; r > radius/2; r--){
			//12時のあたりから360°ぐるっと走査
			for(double i = dig1; i <= dig360; i+=dig1){
				//三角比を用いて先端の座標を求める。
				int x = ((double)r*cos(i+dig90) + (double)off_x);
				int y = ((double)r*(-sin(i+dig90)) + (double)off_y);
				//円周上の先端の位置を求める。
				long_hand_val = (int)(60*(dig360-i)/dig360);
				//そこが黒(2値化済み)ならば
				if(color_zone_img.at<uchar>(y,x) == 0){
					circle(frame_plus, Point(x, y), 5, Scalar(200, 200, 0), 3, 1);
					r = 0; //2重ループを抜け出すためっすよ
					break;
				}
			}
		}
		//短針判定-------------------------
		for (int r = radius*7/10; r > radius/3; r--){
			for(double i=dig1; i <= dig360; i+=dig1){
				int x = ((double)r*cos(i+dig90) + (double)off_x);
				int y = ((double)r*(-sin(i+dig90)) + (double)off_y);
				short_hand_val = (int)(60*(dig360-i)/dig360);
				if ((abs(long_hand_val - short_hand_val) >= 4) && (color_zone_img.at<uchar>(y,x) == 0)){
					circle(frame_plus, Point(x, y), 5, Scalar(0, 0, 200), 3, 1);
					r = 0;
					break;
				}
			}
		}

		//時間の算出-----------------------
		int hour   = short_hand_val/5,
			minuts = long_hand_val;
					
		//etc proc=======================================================
		//put time-------------------------
		char daigou[500];
		sprintf(daigou, "「おにぃちゃん?もう%d:%dだよ?? はやく起きてよね!」\t\tCV:ddd", hour, minuts);
		cout << daigou << endl;
		
		char time[50];
		rectangle(frame_plus, Point(560, 455), Point(640, 490), Scalar(255, 255, 255), -1, CV_AA);
		sprintf(time, "%d:%0d", hour, minuts);
		putText(frame_plus,time, Point(560, 475), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(10, 10, 10), 1, CV_AA);

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
			createTrackbar("color_min", "color_zone", &val_color_s_min_now, 255);
			createTrackbar("color_max", "color_zone", &val_color_s_max_now, 255);
		
		//key read-------------------------
		char key = waitKey(3);
		if (key == 'q')			//終了
			break;
		else if(key == 's'){	//screenshot	
			namedWindow("shot");
			imshow("shot", frame_plus);
		}
	}
	return 0;
}
