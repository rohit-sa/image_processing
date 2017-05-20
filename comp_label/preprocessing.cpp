#include <iostream>
#include <string>
#include <stdlib.h>
#include "preprocessing.h"

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
	
	
	
	
	
	
	
	
	
