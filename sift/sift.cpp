#include <iostream>
#include <vector>
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

int subtract(Mat &a, Mat &b, Mat &result);
int guassFilter(float kernel[][KERNELSIZE], float sigma);
int conv( Mat &im,Mat &outIm, float kernel[][KERNELSIZE]);
int resizeHalf(Mat &in, Mat &out);
int extCalc(Mat &in, Mat &out);
int scaleExt(Mat &down, Mat &in, Mat &up, Mat &out);

int main(){
	double t = (double) getTickCount();
	string path = "/home/r/Documents/C++ Projects/corner/check.png";
	string winNames[16] = {"1-1","1-2","1-3","1-4","2-1","2-2","2-3","2-4","3-1","3-2","3-3","3-4","4-1","4-2","4-3","4-4"};	
	Mat im, colorMat, out;
	vector<Mat> imDif(16), imExt(16), imTemp(20), imOct(5), imOut(8);
	float kernel[GUASS_SIZE][GUASS_SIZE];
	float sigma;
	colorMat = imread(path, CV_LOAD_IMAGE_COLOR);
	if(!colorMat.data){
		cout<< "can't open image"<< endl;
		return -1;
	}
	cvtColor(colorMat, im, COLOR_BGR2GRAY);
	
	imOct[0] = im;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 5; j++){
			imTemp[(4 * i ) + j] = Mat(imOct[i].rows, imOct[i].cols, CV_8UC1, Scalar(0));
		}
		for(int j = 0; j < 4; j++){
			imDif[(4 * i ) + j] = Mat(imOct[i].rows, imOct[i].cols, CV_8UC1, Scalar(0));
			imExt[(4 * i ) + j] = Mat(imOct[i].rows, imOct[i].cols, CV_8UC1, Scalar(0));
		}
		
		sigma = 1.6 * pow(2,i);
		for(int j = 0; j < 5; j++){
			cout << sigma << " ";
			guassFilter(kernel, sigma);
			conv(imOct[i], imTemp[(4 * i ) + j], kernel);
			sigma = sigma * pow(2, 0.5);
		}
		
		for(int j = 0; j < 4; j++){
			subtract(imTemp[(4 * i ) + j + 1], imTemp[(4 * i ) + j], imDif[(4 * i ) + j]);
			extCalc(imDif[(4 * i) + j], imExt[(4 * i) + j]);
			namedWindow(winNames[(4 * i ) + j], WINDOW_AUTOSIZE);
			imshow(winNames[(4 * i ) + j], imExt[(4 * i ) + j]);	
		}
		
		for(int j = 1; j < 3; j++){
			scaleExt(imExt[(4 * i) + j - 1], imExt[(4 * i) + j], imExt[(4 * i) + j + 1], imOut[(2 * i) + j -1]);
			//imshow("asdf", imOut[(2 * i) + j -1]);
			//waitKey(0);
		}
		
		waitKey(0);
		resizeHalf(imOct[i], imOct[i+1]);
	}
	
	namedWindow("original", WINDOW_AUTOSIZE);
	imshow("original", im);
	
	t = ((double)getTickCount() - t)/getTickFrequency();
	cout << t << endl;
	waitKey(0);
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
					uchar k = im.at<uchar>(row+i, col+j);
					sum += k * kernel[limit+i][limit+j];
					weightsum += kernel[limit+i][limit+j];
				}
			} 
			outIm.at<uchar>(row,col) = (uchar) (sum/weightsum);
		}
	}
	return 0;
}	

int subtract(Mat &a, Mat &b, Mat &result){
	int k =0;
	for (int row = 1; row < a.rows-1; row++ ) { 
		for (int col = 1; col < a.cols-1; col++ ) {
			int val_a = a.at<uchar>(row,col);
			int val_b = b.at<uchar>(row,col);
			k = abs(val_a - val_b);
			//cout << k<<  " ";
			if(k != 0)
				cout << k;
			result.at<uchar>(row,col) = (uchar) k;
		}
		//cout << "\n";
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

int resizeHalf(Mat &in, Mat &out){
	out = Mat(in.rows/2, in.cols/2, CV_8UC1, Scalar(0));
	for(int row = 0, i = 0; row < in.rows - 1; row += 2, i++ ){
		for(int col = 0, j = 0; col < in.cols - 1; col += 2, j++ ){
			int val = (in.at<uchar>(row,col) + in.at<uchar>(row+1,col) + in.at<uchar>(row,col+1) + in.at<uchar>(row+1,col+1))/4;
			out.at<uchar>(i,j) = (uchar) val;
		}
	}
	return 0;
}

int extCalc(Mat &in, Mat &out){
	for(int row = 1; row < in.rows -1; row ++){
		for(int col = 1; col < in.cols - 1; col++){
			int val = in.at<uchar>(row,col);
			if(val > in.at<uchar>(row - 1, col -1) && val > in.at<uchar>(row, col -1) && val > in.at<uchar>(row - 1, col) &&
			val > in.at<uchar>(row + 1, col -1) && val > in.at<uchar>(row - 1, col +1) && val > in.at<uchar>(row + 1, col) &&
			val > in.at<uchar>(row, col + 1) && val > in.at<uchar>(row + 1, col +1)){
				out.at<uchar>(row, col) = 255;
				cout << row << " " << col;
			}
			else{
				out.at<uchar>(row, col) = 0;
			}
		}
	}
	return 0;
}

int scaleExt(Mat &down, Mat &in, Mat &up, Mat &out){
	out = Mat(in.rows, in.cols, CV_8UC1, Scalar(0));
	for(int row = 1; row < in.rows -1; row ++){
		for(int col = 1; col < in.cols - 1; col++){
			int val = in.at<uchar>(row,col);
			if(val > down.at<uchar>(row, col) && val > up.at<uchar>(row, col)){
				out.at<uchar>(row, col) = 255;
			}
			else{
				out.at<uchar>(row, col) = 0;
			}
		}
	}
	return 0;
}
