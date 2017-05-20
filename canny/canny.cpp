#include <iostream>
#include <stdlib.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define PI           3.14159265358979323846
#define GUASS_SIZE	 3
#define KERNELSIZE	 3
#define THRESHOLD    0

using namespace std;
using namespace cv;

int sobelFilter( Mat &im,Mat &imMagn, Mat &imAngle, int ker_x[][KERNELSIZE], int ker_y[][KERNELSIZE]);
int guassFilter(float kernel[][GUASS_SIZE], float sigma);
int conv( Mat &im,Mat &outIm, float kernel[][GUASS_SIZE]);
int nms(Mat &im, Mat &imAngle, Mat &result);
int erosion(Mat &im, Mat &outIm, string type);
int dilation(Mat &im, Mat &outIm, string type);

int main(){
	double t = (double) getTickCount();
	string path = "/home/r/Documents/C++ Projects/canny/test1.jpg";
	Mat im, colorMat;
	float kernel[GUASS_SIZE][GUASS_SIZE];
	colorMat = imread(path, CV_LOAD_IMAGE_COLOR);
	if(!colorMat.data){
		cout<< "can't open image"<< endl;
		return -1;
	}
	cvtColor(colorMat, im, COLOR_BGR2GRAY);
	
	Mat imFilt(im.rows, im.cols, CV_8UC1, Scalar(0));
	Mat imMagn(im.rows, im.cols, CV_8UC1, Scalar(0));
	Mat imOut(im.rows, im.cols, CV_8UC1, Scalar(0));
	Mat imAngle(im.rows, im.cols, CV_8UC1, Scalar(0));
	Mat imX(im.rows, im.cols, CV_8UC1, Scalar(0));
	Mat imY(im.rows, im.cols, CV_8UC1, Scalar(0));
	Mat imDil(im.rows, im.cols, CV_8UC1, Scalar(0));
	Mat imEro(im.rows, im.cols, CV_8UC1, Scalar(0));
	Mat imNms(im.rows, im.cols, CV_8UC1, Scalar(0));
	
	int Gy[KERNELSIZE][KERNELSIZE] = {{1,2,1},{0,0,0},{-1,-2,-1}};
	int Gx[KERNELSIZE][KERNELSIZE] = {{1,0,-1},{2,0,-2},{1,0,-1}};
	
	guassFilter(kernel, 0.7);	
	conv(im, imFilt, kernel);
	sobelFilter(imFilt, imMagn, imAngle, Gx, Gy);
	nms(imMagn, imAngle, imOut);
	
	namedWindow("original", WINDOW_AUTOSIZE);
	imshow("original", im);
	namedWindow("magn", WINDOW_AUTOSIZE);
	imshow("magn", imMagn);
	namedWindow("out", WINDOW_AUTOSIZE);
	imshow("out", imOut);
	
	t = ((double)getTickCount() - t)/getTickFrequency();
	cout << t << endl;
	waitKey(0);
	return 0;
}


int sobelFilter( Mat &im,Mat &imMagn, Mat &imAngle, int ker_x[][KERNELSIZE], int ker_y[][KERNELSIZE]){
	int limit = (KERNELSIZE-1)/2;
	
	for (int row = 1; row < im.rows-1; row++ ) { 
		for (int col = 1; col < im.cols-1; col++ ) {
			int op_x = 0, op_y= 0, adj_out = 0;
			for (int i = -limit; i <= limit; i++ ) {
				for (int j = -limit; j <= limit; j++ ) {
				int k = (int) im.at<uchar>(row+i, col+j);
					op_x += k * ker_x[limit+i][limit+j];
					op_y += k * ker_y[limit+i][limit+j];;
				}
			} 
			int dirAngle = 0;
			float angle = (atan2(op_y,op_x) * 180)/ PI;
			if ( ( (angle < 22.5) && (angle > -22.5) ) || ((angle > 157.5) && (angle < -157.5)) ){
				dirAngle = 0;
			}
			if ( ( (angle > 22.5) && (angle < 67.5) ) || ( (angle < -112.5) && (angle > -157.5) ) ){
				dirAngle = 45;
			}
			if ( ( (angle > 67.5) && (angle < 112.5) ) || ( (angle < -67.5) && (angle > -112.5) ) ){
				dirAngle = 90;
			}
			if ( ( (angle > 112.5) && (angle < 157.5) ) || ( (angle < -22.5) && (angle > -67.5) ) ){
				dirAngle = 135;
			}	
			imAngle.at<uchar>(row,col) = (uchar) dirAngle;
			
			adj_out = hypot(op_x, op_y);
			imMagn.at<uchar>(row,col) = (uchar) (adj_out);
		}
	}
	return 0;
}

int guassFilter(float kernel[][GUASS_SIZE], float sigma){
	int limit = (GUASS_SIZE-1)/2;
	
	float temp = 2 * sigma * sigma;
	for(int i = -limit;  i <= limit; i++){
		for(int j = -limit; j <= limit; j++){
			kernel[limit+i][limit+j] = (1/(PI * temp)) * exp(-((i*i)+(j*j))/ temp); 
		}
	}
	return 0;
} 
		
int conv( Mat &im, Mat &outIm, float kernel[][GUASS_SIZE]){
	int limit = (GUASS_SIZE-1)/2;
		
	for (int row = limit; row < im.rows-limit; row++ ) { 
		for (int col = limit; col < im.cols-limit; col++ ) {
			float sum = 0;
			float weightsum = 0; 
			for (int i = -limit; i <= limit; i++ ) {
				for (int j = -limit; j <= limit; j++ ) {
					int k = im.at<uchar>(row+i, col+j);
					sum += k * kernel[limit+i][limit+j];
					weightsum += kernel[limit+i][limit+j];
				}
			} 
			outIm.at<uchar>(row,col) = (uchar) (sum/weightsum);
		}
	}
	return 0;
}	

int nms(Mat &imMagn, Mat &imAngle, Mat &result){
	result = imMagn.clone();
	for (int row = 1; row < imMagn.rows -1; row++ ){ 
		for (int col = 1; col < imMagn.cols -1; col++ ){
			int val = imMagn.at<uchar>(row,col);
			if(val >= THRESHOLD){
				if(imAngle.at<uchar>(row,col) == 0){
					if( !(val >= (int) imMagn.at<uchar>(row,col-1)) || !(val >= (int) imMagn.at<uchar>(row,col+1))){
						result.at<uchar>(row,col) = 0;
					}
				}
				else if(imAngle.at<uchar>(row,col) == 90){
					if( (val >= (int) imMagn.at<uchar>(row-1,col)) || !(val >= (int) imMagn.at<uchar>(row+1,col))){
						result.at<uchar>(row,col) = 0;
					}
				}
				else if(imAngle.at<uchar>(row,col) == 45){
					if( (val >= (int) imMagn.at<uchar>(row-1,col+1)) || !(val >= (int) imMagn.at<uchar>(row+1,col-1))){
						result.at<uchar>(row,col) = 0;
					}
				}
				else if(imAngle.at<uchar>(row,col) == 135){
					if( !(val >= (int) imMagn.at<uchar>(row-1,col-1)) || !(val >= (int) imMagn.at<uchar>(row+1,col+1))){
						result.at<uchar>(row,col) = 0;
					}
				}
			}
		}
	}
	return 0;
}

	
int erosion(Mat &im, Mat &outIm, string type){
	int thres = 100;
	if(type == "cross"){
		for (int row = 1; row < im.rows-1; row++ ) { 
			for (int col = 1; col < im.cols-1; col++ ) {
					if(im.at<uchar>(row,col) > thres && im.at<uchar>(row,col-1) > thres 
					&& im.at<uchar>(row,col+1) > thres && im.at<uchar>(row-1,col) > thres
					&& im.at<uchar>(row+1,col) > thres){
						outIm.at<uchar>(row,col) = 255;
					}
				}
			}
		}
		else if(type == "rect"){
		for (int row = 1; row < im.rows-1; row++ ) { 
			for (int col = 1; col < im.cols-1; col++ ) {			
					if(im.at<uchar>(row,col) > thres && im.at<uchar>(row,col-1) > thres 
					&& im.at<uchar>(row,col+1) > thres && im.at<uchar>(row-1,col) > thres
					&& im.at<uchar>(row+1,col) > thres && im.at<uchar>(row-1,col-1) > thres 
					&& im.at<uchar>(row-1,col+1) > thres && im.at<uchar>(row+1,col-1) > thres
					&& im.at<uchar>(row+1,col+1) > thres){
						outIm.at<uchar>(row,col) = 255;
					}
				}
			}
		}
	
	return 0;
}
	
int dilation(Mat &im, Mat &outIm, string type){
	int thres = 100;
	if(type == "cross"){
		for (int row = 1; row < im.rows-1; row++ ) { 
			for (int col = 1; col < im.cols-1; col++ ) {
					if(im.at<uchar>(row,col) > thres || im.at<uchar>(row,col-1) > thres 
					|| im.at<uchar>(row,col+1) > thres || im.at<uchar>(row-1,col) > thres
					|| im.at<uchar>(row+1,col) > thres){
						outIm.at<uchar>(row,col) = 255;
					}
				}
			}
		}
		else if(type == "rect"){
		for (int row = 1; row < im.rows-1; row++ ) { 
			for (int col = 1; col < im.cols-1; col++ ) {			
					if(im.at<uchar>(row,col) > thres || im.at<uchar>(row,col-1) > thres 
					|| im.at<uchar>(row,col+1) > thres || im.at<uchar>(row-1,col) > thres
					|| im.at<uchar>(row+1,col) > thres || im.at<uchar>(row-1,col-1) > thres 
					|| im.at<uchar>(row-1,col+1) > thres || im.at<uchar>(row+1,col-1) > thres
					|| im.at<uchar>(row+1,col+1) > thres){
						outIm.at<uchar>(row,col) = 255;
					}
				}
			}
		}
	
	return 0;
}	
	
	
