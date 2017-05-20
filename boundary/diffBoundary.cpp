#include <iostream>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;



Point searchStart(Mat &im);
int findEdge(Mat &im, vector<uint> &chainCode, Point center);
int searchNextPixel(Point bPt, int startPos, Mat &im);
int diffChainCode(vector<uint> &chainCode, vector<uint> &diffCode);
int test(Mat &im);

int main(){
	
	//init
	double t = (double)getTickCount();
	Mat im, colorMat;
	Point Pt;
	vector<uint> chainCode;
	vector<uint> diffCode;
	string path = "/home/r/Documents/C++ Projects/boundary/boundary.bmp";
	
	colorMat = imread(path, CV_LOAD_IMAGE_COLOR);
	if(!colorMat.data){
		cout<< "can't open image"<< endl;
		return -1;
	}
	cvtColor(colorMat, im, COLOR_BGR2GRAY);
	
	Pt = searchStart(im);
	findEdge(im, chainCode, Pt);
	diffChainCode(chainCode, diffCode);
	
	
	for(vector<uint>::const_iterator i = diffCode.begin(); i != diffCode.end(); ++i){
		cout << *i << " ";
	}
	cout << endl;
	
	//endsequence
	namedWindow("original", WINDOW_AUTOSIZE);
	imshow("original", im);
	
	t = ((double)getTickCount() - t)/getTickFrequency();
	cout << t << endl;
	waitKey(0);
	
	return 0;
}

int findEdge(Mat &im, vector<uint> &chainCode, Point center){
	
	vector <Point> pixelOffset = {Point(-1,0), Point(-1,-1), Point(0,-1), Point(1,-1), Point(1,0),
		Point(1,1), Point(0,1), Point(-1,1)};
	int nextPos = 0;
	vector<Point> bPt;
	
	bPt.push_back(center);
	
	do{
		//cout << bPt.back() << " " << pixelOffset[nextPos] << endl;
		nextPos = searchNextPixel(bPt.back(),nextPos, im);
		chainCode.push_back(nextPos);
		if(nextPos != -1){
			bPt.push_back(bPt.back() + pixelOffset[nextPos]);
		}
		else{
			cout << "error" << endl;
			break;
		}
	}while(bPt.back() != center);

	return 0;
}

Point searchStart(Mat &im){
	for(int y =0; y < im.rows; y++){
		for(int x =0; x < im.cols; x++){
			if((uint)im.at<uchar>(y,x) == 255){
				Point Pt = Point(x,y);
				return Pt;
			}
		}
	}
	return Point(0,0);
}

int searchNextPixel(Point bPt, int startPos, Mat &im){
	vector <Point> pixelOffset = {Point(-1,0), Point(-1,-1), Point(0,-1), Point(1,-1), Point(1,0),
		Point(1,1), Point(0,1), Point(-1,1)};
	unsigned int i = startPos % 8;
	for(int j = 0; j < 8; j++){
		if((uint)im.at<uchar>(bPt + pixelOffset[i]) > 200){
			return i;
		}
		else{
			i++;
			i = i % 8;
		}
	}
	cout << "not continous" << endl;
	return -1;
}

int diffChainCode(vector<uint> &chainCode, vector<uint> &diffCode){
	for(uint i = 0; i < chainCode.size() - 1; ++i){
		diffCode.push_back((chainCode[i+1] - chainCode[i]) % 8);
	}	
	return 0;
}

int test(Mat &im){
	vector <Point> pixelOffset = {Point(-1,0), Point(-1,-1), Point(0,-1), Point(1,-1), Point(1,0),
		Point(1,1), Point(0,1), Point(-1,1)};
	Point cen = Point(10,10);
	int i = 20;
	for(int j = 0; j < 8; j++){
		im.at<uchar>(cen + pixelOffset[j])  = i;
		i += 20;
	}
	imwrite( "test.jpg", im);
	return 0;
}
