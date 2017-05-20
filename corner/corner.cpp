#include <iostream>
#include <stdlib.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define PI           3.14159265358979323846
#define KERNELSIZE	 3

using namespace std;
using namespace cv;

int gradX(Mat &im, Mat &imX);
int gradY(Mat &im, Mat &imY);
int guassFilter(float kernel[][KERNELSIZE], float sigma);
int conv( Mat &im,Mat &outIm, float kernel[][KERNELSIZE]);
int multiply_s(Mat &a, Mat &b, Mat &result);
int multiply_u(Mat &a, Mat &b, Mat &result);
int add(Mat &a, Mat &b, Mat &result);
int subtract(Mat &a, Mat &b, Mat &result);
int scale(Mat &a, float sensitivity);
int gradAngle(Mat &imX, Mat &imY, Mat &result);
int nms(Mat &im, Mat &imAngle, Mat &result);

int main(){
	double t = (double) getTickCount();
	string path = "/home/r/Documents/C++ Projects/corner/check.png";
	Mat im, colorMat;
	float kernel[KERNELSIZE][KERNELSIZE];
	colorMat = imread(path, CV_LOAD_IMAGE_COLOR);
	if(!colorMat.data){
		cout<< "can't open image"<< endl;
		return -1;
	}
	
	cvtColor(colorMat, im, COLOR_BGR2GRAY);
	
	Mat out(im.rows, im.cols, CV_8UC1, Scalar(0));
	Mat imAngle(im.rows, im.cols, CV_8UC1, Scalar(0));
	Mat imX(im.rows, im.cols, CV_8UC1, Scalar(0));
	Mat imXX(im.rows, im.cols, CV_32SC1, Scalar(0));
	Mat imY(im.rows, im.cols, CV_8UC1, Scalar(0));
	Mat imYY(im.rows, im.cols, CV_32SC1, Scalar(0));
	Mat imXY(im.rows, im.cols, CV_32SC1, Scalar(0));
	Mat imA(im.rows, im.cols, CV_32SC1, Scalar(0));
	Mat imB(im.rows, im.cols, CV_32SC1, Scalar(0));
	Mat imC(im.rows, im.cols, CV_32SC1, Scalar(0));
	Mat imCC(im.rows, im.cols, CV_32SC1, Scalar(0));
	Mat imAB(im.rows, im.cols, CV_32SC1, Scalar(0));
	Mat imTr(im.rows, im.cols, CV_32SC1, Scalar(0));
	Mat imTr2(im.rows, im.cols, CV_32SC1, Scalar(0));
	Mat imDet(im.rows, im.cols, CV_32SC1, Scalar(0));
	Mat imOut(im.rows, im.cols, CV_32SC1, Scalar(0));
	
	guassFilter(kernel, 1);
	gradX(im, imX);
	gradY(im, imY);
	gradAngle(imX, imY, imAngle);
	multiply_u(imX, imX, imXX);
	multiply_u(imY, imY, imYY);
	multiply_u(imX, imY, imXY);
	conv(imXX, imA, kernel);
	conv(imYY, imB, kernel);
	conv(imXY, imC, kernel);
	multiply_s(imA, imB, imAB);
	multiply_s(imC, imC, imCC);
	subtract(imAB, imCC, imDet);
	add(imA, imB, imTr);
	multiply_s(imTr, imTr, imTr2);
	scale(imTr2, 0.04);
	subtract(imDet, imTr2, imOut);
	nms(imOut, imAngle, out);
	
	for(int row = 0; row < imOut.rows; row++){
		for(int col = 0; col < imOut.cols; col++){
			if(imOut.at<int>(row,col) > 20000){
				imOut.at<int>(row,col) = 255;
			}
			else{
				imOut.at<int>(row,col) = 0;
			}
		}
	}
	imOut.convertTo(imOut, CV_8U);
	
	//imwrite("/home/r/Documents/C++ Projects/corner/output.png", imOut);
	namedWindow("original", WINDOW_AUTOSIZE);
	imshow("original", im);

	namedWindow("out", WINDOW_AUTOSIZE);
	imshow("out", out);
	
	namedWindow("x-gradient", WINDOW_AUTOSIZE);
	imshow("x-gradient", imX);
	namedWindow("y-gradient", WINDOW_AUTOSIZE);
	imshow("y-gradient", imY);
	
	t = ((double)getTickCount() - t)/getTickFrequency();
	cout << t << endl;
	waitKey(0);
	return 0;
}

	
int gradX(Mat &im, Mat &imX){
	for(int row = 1; row < im.rows-1; row++){
		for(int col = 1; col < im.cols-1; col++){
			int k = im.at<uchar>(row-1,col+1) + im.at<uchar>(row+1,col+1) - im.at<uchar>(row+1,col-1) - im.at<uchar>(row-1,col-1)
			+(2*im.at<uchar>(row,col+1)) - (2*im.at<uchar>(row,col-1));
			imX.at<uchar>(row,col) = (uchar) abs(k);
		}
	}
	return 0;
}

int gradY(Mat &im, Mat &imY){
	for(int row = 1; row < im.rows-1; row++){
		for(int col = 1; col < im.cols-1; col++){
			int k = im.at<uchar>(row-1,col-1) + im.at<uchar>(row-1,col+1) - im.at<uchar>(row+1,col-1) - im.at<uchar>(row+1,col+1)
			+(2*im.at<uchar>(row-1,col)) - (2* im.at<uchar>(row+1,col));
			imY.at<uchar>(row,col) = (uchar) abs(k);
		}
	}		
	return 0;
}

int guassFilter(float kernel[][KERNELSIZE], float sigma){
	int limit = (KERNELSIZE-1)/2;
	float temp = 2 * sigma * sigma;
	for(int i = -limit;  i <= limit; i++){
		for(int j = -limit; j <= limit; j++){
			kernel[limit+i][limit+j] = (1/(PI * temp)) * exp(-((i*i)+(j*j))/ temp); 
		}
	}
	return 0;
} 
		
int conv( Mat &im, Mat &outIm, float kernel[][KERNELSIZE]){
	int limit = (KERNELSIZE-1)/2;
	
	for (int row = limit; row < im.rows-limit; row++ ) { 
		for (int col = limit; col < im.cols-limit; col++ ) {
			float sum = 0;
			float weightsum = 0; 
			for (int i = -limit; i <= limit; i++ ) {
				for (int j = -limit; j <= limit; j++ ) {
					int k = im.at<int>(row+i, col+j);
					sum += k * kernel[limit+i][limit+j];
					weightsum += kernel[limit+i][limit+j];
				}
			} 
			outIm.at<int>(row,col) = (int) (sum/weightsum);
		}
	}
	return 0;
}	

int add(Mat &a, Mat &b, Mat &result){
	int k =0;
	for (int row = 1; row < a.rows-1; row++ ) { 
		for (int col = 1; col < a.cols-1; col++ ) {
			int val_a = a.at<int>(row,col);
			int val_b = b.at<int>(row,col);
			k = val_a + val_b;
			result.at<int>(row,col) = (int) k;
		}
	}		
	return 0;
}

int subtract(Mat &a, Mat &b, Mat &result){
	int k =0;
	for (int row = 1; row < a.rows-1; row++ ) { 
		for (int col = 1; col < a.cols-1; col++ ) {
			int val_a = a.at<int>(row,col);
			int val_b = b.at<int>(row,col);
			k = val_a - val_b;
			result.at<int>(row,col) = (int) k;
		}
	}		
	return 0;
}
	
int multiply_u(Mat &a, Mat &b, Mat &result){
	float val_c =0;
	for (int row = 0; row < a.rows; row++ ) { 
		for (int col = 0; col < a.cols; col++ ) {
			int val_a = a.at<uchar>(row,col);
			int val_b = b.at<uchar>(row,col);
			val_c = val_a * val_b;
			//cout << val_a << "*" << val_b << " ";
			result.at<int>(row,col) = (int) val_c;
		}
	}
	return 0;
}

int scale(Mat &a, float sensitivity){
	float k = 0;
	for (int row = 0; row < a.rows; row++ ) { 
		for (int col = 0; col < a.cols; col++ ) {
			int val_a = a.at<int>(row,col);
			k = (float) (val_a * sensitivity);
			a.at<int>(row,col) = (int) k;
		}
	}
	return 0;
}

int multiply_s(Mat &a, Mat &b, Mat &result){
	int val_c =0;
	for (int row = 0; row < a.rows; row++ ) { 
		for (int col = 0; col < a.cols; col++ ) {
			int val_a = a.at<int>(row,col);
			int val_b = b.at<int>(row,col);
			val_c = val_a * val_b;
			//cout << val_a << "*" << val_b << " ";
			result.at<int>(row,col) = (int) val_c;
		}
	}
	return 0;
}

int gradAngle(Mat &imX, Mat &imY, Mat &result){\
	int newAngle = 0;
	for (int row = 1; row < imX.rows -1; row++ ) { 
		for (int col = 1; col < imY.cols -1; col++ ) {
			int x = imX.at<int>(row,col);
			int y = imY.at<int>(row,col);
			float angle = (atan2(y,x) / PI) * 180;
			if ( ( (angle < 22.5) && (angle > -22.5) ) || (angle > 157.5) || (angle < -157.5) )
				newAngle = 0;
			if ( ( (angle > 22.5) && (angle < 67.5) ) || ( (angle < -112.5) && (angle > -157.5) ) )
				newAngle = 45;
			if ( ( (angle > 67.5) && (angle < 112.5) ) || ( (angle < -67.5) && (angle > -112.5) ) )
				newAngle = 90;
			if ( ( (angle > 112.5) && (angle < 157.5) ) || ( (angle < -22.5) && (angle > -67.5) ) )
				newAngle = 135;
				
			result.at<uchar>(row,col) = (uchar) newAngle;
		}
	}
	return 0;
}

int nms(Mat &im, Mat &imAngle, Mat &result){
	for (int row = 1; row < im.rows -1; row++ ){ 
		for (int col = 1; col < im.cols -1; col++ ){
			int val = im.at<int>(row,col);
			if(val > 200){
				if(imAngle.at<uchar>(row,col) == 0){
					if(val > im.at<int>(row-1,col) && val > im.at<int>(row+1,col)){
						result.at<uchar>(row,col) = 255;
					}
					else{
						result.at<uchar>(row,col) = 0;
					}
				}
				else if(imAngle.at<uchar>(row,col) == 90){
					if(val > im.at<int>(row,col-1) && val > im.at<int>(row,col+1)){
						result.at<uchar>(row,col) = 255;
					}
					else{
						result.at<uchar>(row,col) = 0;
					}
				}
				else if(imAngle.at<uchar>(row,col) == 45){
					if(val > im.at<int>(row-1,col+1) && val > im.at<int>(row+1,col-1)){
						result.at<uchar>(row,col) = 255;
					}
					else{
						result.at<uchar>(row,col) = 0;
					}
				}
				else if(imAngle.at<uchar>(row,col) == 135){
					if(val > im.at<int>(row-1,col-1) && val > im.at<int>(row+1,col+1)){
						result.at<uchar>(row,col) = 255;
					}
					else{
						result.at<uchar>(row,col) = 0;
					}
				}
			}
		}
	}
	return 0;
}
			
	
