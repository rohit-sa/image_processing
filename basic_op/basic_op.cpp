#include <iostream>
#include "Shrink.h"
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;


int threshold(Mat &I,int limit);
int sharpen(Mat &I);
int smoothBlur(Mat &I);
Mat histogram(Mat &I, Mat &histImage);
int equalHist(Mat &I);

int main(){
	double t = (double)getTickCount();
	Mat image, grayMat;
	string path = "/home/r/Documents/C++ Projects/lenna.png";
	image = imread(path, CV_LOAD_IMAGE_COLOR);
	if(!image.data){
		cout<< "can't open image"<< endl;
		return -1;
	}
	cvtColor(image, grayMat, COLOR_BGR2GRAY);
	
	
	threshold(grayMat, 127);
	//sharpen(grayMat);
	//smoothBlur(grayMat);
	//~ equalHist(grayMat);
	t = ((double)getTickCount() - t)/getTickFrequency();
	cout << t << endl;
	
	
	imshow("original", image);
	waitKey(0);
	return 0;
	
}

int threshold(Mat &I, int limit){
	
	int nRows = I.rows;
	int nCols = I.channels() * I.cols;
	int i,j;
	uchar *p;
	
	for(i = 0; i < nRows; ++i){
		p = I.ptr<uchar>(i);
		for(j = 0; j < nCols; j++){
			if((int)p[j] > limit){
				p[j] = 255;
			}
		}
	}
	Mat out = cgmb::shrink_max(I);
	imshow("image", I);
	return 0;
}

int sharpen(Mat &I){
	Mat filtered_I;
	Mat kern = (Mat_<char>(3,3) << 0, -1, 0,
								  -1, 5, -1,
								  0, -1, 0);
	filter2D(I, filtered_I, I.depth(), kern);
	imshow("image", filtered_I);
	return 0;
}

int smoothBlur(Mat &I){
	Mat filtered_I;
	blur(I, filtered_I, Size(3,3), Point(-1,-1));
	imshow("image", filtered_I);
	return 0;
}

Mat histogram(Mat &I,Mat &histImage){
	int histSize = 256;
	float range[] = {0, 255};
	const float *ranges = {range};
	MatND hist;
	calcHist( &I, 1, 0, Mat(), hist, 1, &histSize, &ranges, true, false);
	/*double total;
	total = I.rows * I.cols;
	for(int i = 0; i < histSize; ++i){
		float binVal = hist.at<float>(i);
		cout << " " << binVal;
	}
	 */
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double) hist_w/histSize);
	
	normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	
	for(int i = 1; i < histSize; ++i){
		line(histImage, Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1))),
						Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i))),
						Scalar(255,0,0), 2, 8, 0);
	}
	
	return histImage;
	
}

int equalHist(Mat &I){
	int hist_w = 512; int hist_h = 400;
	Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0,0,0));
	Mat eqHistImage (hist_h, hist_w, CV_8UC1, Scalar(0,0,0));
	Mat equalizedI;
	equalizeHist(I,equalizedI);
	namedWindow( "unequalized", CV_WINDOW_AUTOSIZE);
	namedWindow("equalized", CV_WINDOW_AUTOSIZE);
	namedWindow("equalized Image", CV_WINDOW_AUTOSIZE);
	imshow("unequalized", histogram(I, histImage));
	imshow("equalized", histogram(equalizedI, eqHistImage));
	imshow("equalized Image", equalizedI);
	return 0;
}
