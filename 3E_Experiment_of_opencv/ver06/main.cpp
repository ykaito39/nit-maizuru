/*********************************************************************************
 * H27年度3E 電気情報工学実験2Bのやつ
 * Ver0.6
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
 *				2015/12/21	CV:dの部分的実装
 *				2015/12/21	ver05が謎拡張を繰り返したせいでよくわからなくなってver06に移行
 *
 * 予定: 		CVの完全実装
 * 				リファクタリングとコードの簡素化
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
int minHue = 90,	//色相の下限
	maxHue = 125,	//色相の上限
	nowSat = 176,
	nowVal = 92;

//色分離
void PicUpColor(Mat blurImg, Mat binaryColorImg){
	Mat hsvImg;
	cvtColor(blurImg, hsvImg, CV_BGR2HSV);//conv hsv
	uchar hue, sat, val;
	for(int y = 0; y < CAP_H; y++){
		for(int x = 0; x < CAP_W; x++){
			//hsvを分離
			hue = hsvImg.at<Vec3b>(y,x)[0];
			sat = hsvImg.at<Vec3b>(y,x)[1];
			val = hsvImg.at<Vec3b>(y,x)[2];
			//設定したしきい値内のみを黒に、他を白にする
			if( (minHue <= hue && hue <= maxHue) && (nowSat <= sat) && (nowVal <= val) )
				binaryColorImg.at<uchar>(y,x) = 0;
			else
				binaryColorImg.at<uchar>(y,x) = 255;
		}
	}
}

//円検出
void SerchCircles(Mat blurImg, Mat circlePlusImg, int *offset_x, int *offset_y, int *radius){
	Mat grayImg;
	cvtColor(blurImg, grayImg, CV_BGR2GRAY);
	vector<Vec3f> circles;
	
	HoughCircles(grayImg, circles, CV_HOUGH_GRADIENT, 1, 200, 100, 100, 50);

	//i番目に検出した円に対して...
	for( size_t i=0; i < circles.size(); i++){
		//原点座標と半径の取得
		*offset_x  = cvRound(circles[i][0]);
		*offset_y  = cvRound(circles[i][1]);
		*radius = cvRound(circles[i][2]);
		
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		//byouga
      	circle( circlePlusImg, center, 3,		Scalar(0,255,0), -1, 8, 0 );
       	circle( circlePlusImg, center, *radius, Scalar(0,0,255), 3, 8, 0 );
	}
}

//長針判定
int SerchLongHand(Mat binaryColorImg, Mat framePlus, int *offset_x, int *offset_y, int *radius){
	int longHand = 0;
	return longHand;
}

//短針判定
void SerchShortHand();
//傾き検出
void ReadSlope();
//ラディアンから時間空間に
void CvtRadtoTime();

//時間空間に戻す前の時間と分, つまりラジアン空間のやつ
double 	long_hand_val  = 0.0,
		short_hand_val = 0.0;

//円検知で使う
int offset_x = 320,	//円の原点x
	offset_y = 240,	//円の原点y座標
	radius = 200; 	//円の半径


//水平検知で使う
double slope = 0.0;	//傾き

int xxx = 0;


int main(int argc, char *argv[]){
	VideoCapture cap(0);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, CAP_W);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAP_H);
	if(!cap.isOpened()){
		cout << "cant opned." << endl;
		return -1;
	}

	while(1){
		Mat frame;						//Capture frame data
		cap >> frame;					//frameにcapを読み取る。
		Mat framePlus = frame.clone(); //これにいろいろ追加していく

		//ノイズ軽減--------------------------------
		Mat blurImg;
		GaussianBlur(frame, blurImg, Size(9, 9), 2, 2);
		
		//色分離
		Mat binaryColorImg = Mat(Size(CAP_W, CAP_H), CV_8UC1);
		PicUpColor(blurImg, binaryColorImg);

		//円検出
		//Mat circlePlusImg = frame_plus;
		SerchCircles(blurImg, framePlus, &offset_x, &offset_y, &radius);

 		//長針判定
		//SerchLongHand();
		//短針判定
//		SerchShortHand();
		//傾き検出
//		ReadSlope();
		//ラディアンから時間空間に
//		CvtRadtoTime();
		
		namedWindow("hell");
		imshow("hell", binaryColorImg);
		namedWindow("hello");
		imshow("hello", framePlus);


				char key = waitKey(1);
		if (key == 'q')
			break;
		}
	return 0;
}
