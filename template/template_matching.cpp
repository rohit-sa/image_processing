#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int cross_corr( Mat &sourceIm, Mat &tempIm, Mat &outIm);

int main(){
	//init
	double t = (double)getTickCount();
	Mat sourceIm, sourceMat, tempIm, tempMat, outIm;
	
	string sourcePath = "/home/r/Documents/C++ Projects/template/connectors.png";
	string tempPath = "/home/r/Documents/C++ Projects/template/connector_pattern.png";
	
	sourceMat = imread(sourcePath, CV_LOAD_IMAGE_COLOR);
	tempMat = imread(tempPath, CV_LOAD_IMAGE_COLOR);
	if((!sourceMat.data) || (!tempMat.data)){
		cout<< "can't open image"<< endl;
		return -1;
	}
	cvtColor(sourceMat, sourceIm, COLOR_BGR2GRAY);
	cvtColor(tempMat, tempIm, COLOR_BGR2GRAY);
	outIm = sourceMat;
	
	cross_corr(sourceIm, tempIm, outIm);
	
	
	namedWindow("original", WINDOW_AUTOSIZE);
	imshow("original", sourceIm);
	namedWindow("match", WINDOW_AUTOSIZE);
	imshow("match", outIm);
	
	t = ((double)getTickCount() - t)/getTickFrequency();
	cout << "runtime: "<< t << endl;
	waitKey(0);
	return 0;
}


int cross_corr( Mat &sourceIm, Mat &tempIm, Mat &outIm){
	float tempMean = 0;
	for (int i = 0; i < tempIm.rows; i++ ) {
		for (int j = 0; j < tempIm.cols; j++ ) {
			uchar k = tempIm.at<uchar>(i, j);
			tempMean += k;
		}
	} 
	tempMean = tempMean/(tempIm.rows * tempIm.cols);
	
	for (int row = 0; row < sourceIm.rows - tempIm.rows; row++ ){ 
		for (int col = 0; col < sourceIm.cols - tempIm.cols; col++ ){
			float sourceMean = 0; 
			for (int i = 0; i < tempIm.rows; i++ ) {
				for (int j = 0; j < tempIm.cols; j++ ) {
					uchar k = sourceIm.at<uchar>(row+i, col+j);
					sourceMean += k;
				}
			}
			sourceMean = sourceMean/(tempIm.rows * tempIm.cols);
			float sourceVar = 0, tempVar = 0, corr = 0;
			for (int i = 0; i < tempIm.rows; i++ ) {
				for (int j = 0; j < tempIm.cols; j++ ) {
					uchar s = sourceIm.at<uchar>(row+i, col+j);
					uchar t = tempIm.at<uchar>(i, j);
					sourceVar += (s - sourceMean) * (s - sourceMean);
					tempVar += (t - tempMean) * (t - tempMean);
					corr += (s - sourceMean) * (t - tempMean);
				}
			}
			float corrCoeff = 0;
			if(tempVar == 0 || sourceVar == 0){
				corrCoeff = 0;
			}
			else{
				corrCoeff = corr / sqrt(tempVar * sourceVar);
				//cout << corrCoeff << endl;
				if(abs(corrCoeff) > 0.9){
					rectangle(outIm, Point(col, row), Point(col+tempIm.cols, row+tempIm.rows), 
					Scalar(255,0,0), 1, 8, 0);
					//cout << row << " " << col << endl;
				} 
			}
		}
			
	}
	return 0;
}
	
