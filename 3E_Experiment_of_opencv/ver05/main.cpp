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
 *							CV:d(console)を実装
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

//水平検知で使う
double slope = 0.0;					//傾き

int xxx = 0;	//は?キレそう

int rot = 0;	//1-4のキー入力で角度を切り替えるあれ

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
					color_zone_img.at<uchar>(y,x) = 0;
				else
					color_zone_img.at<uchar>(y,x) = 255;
			}
		}

		//円の検出=================================================
		Mat gray;
		cvtColor(blur, gray, CV_BGR2GRAY);
		vector<Vec3f> circles;
		
		HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, 200, 100, 100, 100);

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
		//長針判定-------------------------
		//外側から内側に向かって絞っていく
		for (int r = radius*4/5; r > radius/2; r--){
			//12時のあたりから360°ぐるっと走査,半時計回り。
			for(double i = (PI/180); i <= (PI*2); i+=(PI/180)){
				//(x,y)の点をたどる。だいたい円形。
				int x = (int)((double)r*cos(i+(PI/2)) + (double)off_x);
				int y = (int)((double)r*(-sin(i+(PI/2))) + (double)off_y);
				xxx = x; //なんの変数だ?
				//円周上の先端の位置を求める。半時計回り->360-云々
				long_hand_val = (PI*2)-i;
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
			for(double i=(PI/180); i <= (PI*2); i+=(PI/180)){
				int x = (int)((double)r*cos(i+(PI/2)) + (double)off_x);
				int y = (int)((double)r*(-sin(i+(PI/2))) + (double)off_y);
				short_hand_val = (PI*2)-i;
				if ((( abs(long_hand_val - short_hand_val) >= 0.2) && (color_zone_img.at<uchar>(y,x) == 0) ) && (abs(long_hand_val - short_hand_val) <= 6)){
					circle(frame_plus, Point(x, y), 5, Scalar(0, 0, 200), 3, 1);
					r = 0;
					break;
				}
			}
		}

		//
		char key = waitKey(20);
/*		if (key == '1')
			rot = 0;
		else if (key == '2')
			rot = PI/2;
		else if (key == '3')
			rot = PI;
		else if (key == '4')
			rot = 3/2 * PI;
*/
		//水平検知紛い(大郷の第二メソッド)
		int cube_rad = radius*1.6;	//中心から四角形の頂点までの距離
		for(int corner_y=(off_y-cube_rad); corner_y < (off_y-radius); corner_y++){		//yhoukou
			for(int corner_x=(off_x-cube_rad); corner_x < (off_x+cube_rad); corner_x++){	//xhoukou
				if(color_zone_img.at<uchar>(corner_y, corner_x) == 0){
					//傾き角度の検出(ifで検出したコーナーの位置を使う)
					if( (corner_x < off_x) && (corner_y < off_y-radius*1.2) ){
						slope = ((PI/4) - atan( (off_y - corner_y)/(off_x - corner_x) ) );
					}else if( (corner_x > off_x) && (corner_y < off_y-radius*1.2) ){
						slope = -((PI/4) + atan( (off_y - corner_y)/(off_x - corner_x) ) );
					}else{
						slope = 0.0;
					}
					circle(frame_plus, Point(corner_x, corner_y), 5, Scalar(0, 200, 200), 3, 1);
					corner_y = 640;
					break;
				}
			}
		}
		
		//cout << short_hand_val << "\t" << long_hand_val << endl;
		//TIP: PI*2 = 60分
		int hour   = (short_hand_val + slope-rot)*(12/(PI*2)),
			minuts = (long_hand_val + slope-rot)*(60/(PI*2));
		
		//
/*		if (hour < 0)
			hour += 11;

		if (minuts < 0)
			minuts += 59;
*/
		//etc proc=======================================================
		//put time-------------------------
		char daigou[500];
		sprintf(daigou, "「おにぃちゃん?もう%d:%dだよ?? はやく起きてよね!」\tCV:d", hour, minuts);
		cout << daigou << endl;
		
		char time[50];
		rectangle(frame_plus, Point(560, 455), Point(640, 490), Scalar(255, 255, 255), -1, CV_AA);
		sprintf(time, "%d:%0d", hour, minuts);
		putText(frame_plus, time, Point(560, 475), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(10, 10, 10), 1, CV_AA);

		char slope_txt[50];
		sprintf(slope_txt, "%f", slope);
		putText(frame_plus, slope_txt, Point(off_x, off_y), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 250, 0), 1, CV_AA);

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
