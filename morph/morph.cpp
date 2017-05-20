#include <iostream>
#include <string>
#include <stdlib.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int conv(Mat &im, Mat &outIm, int kernel[][3]);
int sobelFilter( Mat &im,Mat &outIm, int ker_x[][3], int ker_y[][3]);
int erosion(Mat &im, Mat &outIm, string type);
int dilation(Mat &im, Mat &outIm, string type);
int subtract(Mat &a, Mat &b, Mat &result);

int main(){
	double t = (double)getTickCount();
	Mat im, colorMat;
	int out = 1;
	string path = "/home/r/Documents/C++ Projects/morph/erosion.bmp";

	colorMat = imread(path, CV_LOAD_IMAGE_COLOR);
	if(!colorMat.data){
		cout<< "can't open image"<< endl;
		return -1;
	}
	cvtColor(colorMat, im, COLOR_BGR2GRAY);
	Mat outIm(im.rows,im.cols, CV_8UC1, Scalar(0));
	Mat temp1 = im, temp2 =outIm, temp3 = outIm;
	
	int Gy[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
	int Gx[3][3] = {{1,0,-1},{2,0,-2},{1,0,-1}};
	
	//for(int i = 0; i < 5 ; i++){
		//erosion(temp1, temp2, "cross");
		//temp1 = temp2;
	//}
	out = erosion(im, temp2, "rect");
	//subtract(im, temp2, outIm);
	//out = sobelFilter(im, outIm, Gx, Gy);
	cout << out << endl;
	
	namedWindow("original", WINDOW_AUTOSIZE);
	imshow("original", im);
	namedWindow("morph", WINDOW_AUTOSIZE);
	imshow("morph", temp2);
	
	t = ((double)getTickCount() - t)/getTickFrequency();
	cout << t << endl;
	waitKey(0);
	return 0;
}

int conv( Mat &im,Mat &outIm, int kernel[][3]){
	for (int row = 1; row < im.rows-1; row++ ) { 
		for (int col = 1; col < im.cols-1; col++ ) {
			float accumulation = 0;
			float weightsum = 0; 
			for (int i = -1; i <= 1; i++ ) {
				for (int j = -1; j <= 1; j++ ) {
				unsigned char k = im.at<uchar>(row+i, col+j);
					accumulation += k * kernel[1+i][1+j];
					weightsum += kernel[1+i][1+j];
				}
			} 
			outIm.at<uchar>(row,col) = (uchar) (accumulation/weightsum);
		}
	}
	return 0;
}
	

int sobelFilter( Mat &im,Mat &outIm, int ker_x[][3], int ker_y[][3]){
	for (int row = 1; row < im.rows-1; row++ ) { 
		for (int col = 1; col < im.cols-1; col++ ) {
			int op_x = 0, op_y= 0, adj_out = 0;
			for (int i = -1; i <= 1; i++ ) {
				for (int j = -1; j <= 1; j++ ) {
				unsigned char k = im.at<uchar>(row+i, col+j);
					op_x += k * ker_x[1+i][1+j];
					op_y += k * ker_y[1+i][1+j];;
				}
			} 
			adj_out = (abs(op_x) + abs(op_y))/4;
			outIm.at<uchar>(row,col) = (uchar) (adj_out);
		}
	}
	return 0;
}
	
int erosion(Mat &im, Mat &outIm, string type){
	if(type == "cross"){
		for (int row = 1; row < im.rows-1; row++ ) { 
			for (int col = 1; col < im.cols-1; col++ ) {
					if(im.at<uchar>(row,col) > 200 && im.at<uchar>(row,col-1) > 200 
					&& im.at<uchar>(row,col+1) > 200 && im.at<uchar>(row-1,col) > 200
					&& im.at<uchar>(row+1,col) > 200){
						outIm.at<uchar>(row,col) = 255;
					}
				}
			}
		}
		else if(type == "rect"){
		for (int row = 1; row < im.rows-1; row++ ) { 
			for (int col = 1; col < im.cols-1; col++ ) {			
					if(im.at<uchar>(row,col) > 200 && im.at<uchar>(row,col-1) > 200 
					&& im.at<uchar>(row,col+1) > 200 && im.at<uchar>(row-1,col) > 200
					&& im.at<uchar>(row+1,col) > 200 && im.at<uchar>(row-1,col-1) > 200 
					&& im.at<uchar>(row-1,col+1) > 200 && im.at<uchar>(row+1,col-1) > 200
					&& im.at<uchar>(row+1,col+1) > 200){
						outIm.at<uchar>(row,col) = 255;
					}
				}
			}
		}
	
	return 0;
}
	
int dilation(Mat &im, Mat &outIm, string type){
	if(type == "cross"){
		for (int row = 1; row < im.rows-1; row++ ) { 
			for (int col = 1; col < im.cols-1; col++ ) {
					if(im.at<uchar>(row,col) > 200 || im.at<uchar>(row,col-1) > 200 
					|| im.at<uchar>(row,col+1) > 200 || im.at<uchar>(row-1,col) > 200
					|| im.at<uchar>(row+1,col) > 200){
						outIm.at<uchar>(row,col) = 255;
					}
				}
			}
		}
		else if(type == "rect"){
		for (int row = 1; row < im.rows-1; row++ ) { 
			for (int col = 1; col < im.cols-1; col++ ) {			
					if(im.at<uchar>(row,col) > 200 || im.at<uchar>(row,col-1) > 200 
					|| im.at<uchar>(row,col+1) > 200 || im.at<uchar>(row-1,col) > 200
					|| im.at<uchar>(row+1,col) > 200 || im.at<uchar>(row-1,col-1) > 200 
					|| im.at<uchar>(row-1,col+1) > 200 || im.at<uchar>(row+1,col-1) > 200
					|| im.at<uchar>(row+1,col+1) > 200){
						outIm.at<uchar>(row,col) = 255;
					}
				}
			}
		}
	
	return 0;
}	
	
int subtract(Mat &a, Mat &b, Mat &result){
	for (int row = 1; row < a.rows-1; row++ ) { 
		for (int col = 1; col < a.cols-1; col++ ) {
			int val_a = (int) a.at<uchar>(row,col);
			int val_b = (int) b.at<uchar>(row,col);
			result.at<uchar>(row,col) = (uchar) (val_a - val_b);
		}
	}
	return 0;
}
	
	
	
	
	
	
	
	
	
