#include <iostream>
#include "feature.h"

Coordinate<int> BoundingBox(Mat &im){
	int totalPixel = im.rows * im.cols;
	Coordinate<int> vertices(im.cols,0,0,0);
	for(int i= 0; i < totalPixel; i++){
		if(im.at<uchar>(i/im.cols,i%im.cols) > (uchar)200){
			vertices.y = i/im.cols;
			break;
		}
	}
	for(int i= 0; i < totalPixel; i++){
		if(im.at<uchar>(i/im.cols,i%im.cols) > (uchar)200){
			if(vertices.x > i%im.cols){
				vertices.x = i%im.cols;
			}
			else{
				i = ((i/im.cols) + 1)*im.cols;
			}
		}
	}
	for(int i= totalPixel; i >= 0 ; i--){
		if(im.at<uchar>(i/im.cols,i%im.cols) > (uchar)200){
			vertices.v = i/im.cols;
			break;
		}
	}
	for(int i= totalPixel; i >= 0; i--){
		if(im.at<uchar>(i/im.cols,i%im.cols) > (uchar)200){
			if(vertices.u < i%im.cols){
				vertices.u = i%im.cols;
			}
			else{
				i = ((i/im.cols) - 1)*im.cols;
			}
		}
	}
	return vertices;
}

Coordinate<double> Centroid(Mat &im, Coordinate<int> vertices){
	int totalSum = 0;
	double rowSum = 0.0,colSum = 0.0;
	for(int i= vertices.y; i < vertices.v; i++){
		for(int j =vertices.x; j < vertices.u; j++){
			if(im.at<uchar>(i,j) > (uchar)200){
				totalSum ++;
				rowSum += i;
				colSum += j;
			}
		}
	}
	//center of mass with (0,0) at (bounded_height/2,bounded_ width/2)
	Coordinate<double> hor_ver_center((rowSum/totalSum) - ((vertices.v - vertices.y)/2), (colSum/totalSum) - ((vertices.u - vertices.x)/2));
	return hor_ver_center;
}

double Area(Mat &im, Coordinate<int> vertices){
	double totalArea =0;
	for(int i= vertices.y; i < vertices.v; i++){
		for(int j =vertices.x; j < vertices.u; j++){
			if(im.at<uchar>(i,j) > (uchar)200){
				totalArea ++;
			}
		}
	}
	return totalArea/((vertices.u - vertices.x) * (vertices.v - vertices.y));
}

double HorSymm(Mat &im, Coordinate<int> vertices){
	double symm = 0;
	int total_sum = 0;
	int mid = (vertices.u- vertices.x)/2;
	for(int i= vertices.y; i < vertices.v; i++){
		for(int j =vertices.x; j <= mid ; j++){
			if(im.at<uchar>(i,j) > 200){
				total_sum++;
			}
			if((im.at<uchar>(i,j) > (uchar)200) && (im.at<uchar>(i,vertices.u - j) > (uchar)200)){
				symm++;
			}
		}
	}
	if(total_sum != 0){
		return symm/total_sum;
	}
	else{
		return 0;
	}
}

double VerSymm(Mat &im, Coordinate<int> vertices){
	double symm = 0;
	int total_sum = 0;
	int mid = (vertices.v- vertices.y)/2;
	for(int i= vertices.x; i < vertices.u; i++){
		for(int j =vertices.y; j <= mid ; j++){
			if(im.at<uchar>(j,i) > 200){
				total_sum++;
			}
			if((im.at<uchar>(j,i) > (uchar)200) && (im.at<uchar>(vertices.v - j,i) > (uchar)200)){
				symm++;
			}
		}
	}
	if(total_sum != 0){
		return symm/total_sum;
	}
	else{
		return 0;
	}
}

int EulerCalc(Mat &im, Coordinate<int> vertices){
	int convexities = 0, concavities = 0;
	for(int i= vertices.y; i < vertices.v-1; i++){
		for(int j =vertices.x; j < vertices.u-1; j++){
			if((im.at<uchar>(i,j) > (uchar)200) && (im.at<uchar>(i,j-1) < (uchar)200) && (im.at<uchar>(i-1,j) < (uchar)200) && (im.at<uchar>(i-1,j-1) < (uchar)200)){
				convexities++;
			}
			if((im.at<uchar>(i,j) < (uchar)200) && (im.at<uchar>(i,j+1) > (uchar)200) && (im.at<uchar>(i+1,j) > (uchar)200) && (im.at<uchar>(i+1,j+1) > (uchar)200)){
				concavities++;
			}
		}
	}
	//cout << convexities << " "<< concavities << endl;
	//cout << (convexities - concavities) << endl;
	return (convexities - concavities) ;
}
