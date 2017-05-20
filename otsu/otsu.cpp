#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int main(){
	//init
	double t = (double)getTickCount();
	Mat im, colorMat;
	
	string path = "/home/r/Documents/C++ Projects/corner/output.png";
	float hist[256] = {0.0}, var[256] = {0.0};
	uint pixelVal = 0, thresh = 0;
	float mean_t = 0;
	
	colorMat = imread(path, CV_LOAD_IMAGE_COLOR);
	if(!colorMat.data){
		cout<< "can't open image"<< endl;
		return -1;
	}
	cvtColor(colorMat, im, COLOR_BGR2GRAY);
	Mat outIm(im.rows,im.cols, CV_8UC1, Scalar(0));


	for(int y = 0; y < im.rows; y++){
		for(int x = 0; x < im.cols; ++x){
			pixelVal = (uint) im.at<uchar>(y,x);
			hist[pixelVal]++;
		}
	}
	
	for(int i = 0; i < 256; i++){
		hist[i] = hist[i]/(im.rows * im. cols);
		mean_t += i * hist[i];
	}
	
	for(int t = 0; t < 256; t++){
		float w = 0.0, mean_k = 0.0, temp = 0.0;
		for(int i =0; i < t; i++){
			w += hist[i];
			mean_k += i * hist[i];
		}
		
		if((w == 0.0) || (w == 1.0)){
			//do nothing
		}
		else{
			temp = ((mean_t * w) - mean_k);
			var[t] = (temp * temp) / (w *( 1 - w));
		}

		if(var[t] > var[thresh]){
			thresh = t;
		}
	}
	cout << var[thresh] << endl;
	for(int y = 0; y < im.rows; y++){
		for(int x = 0; x < im.cols; ++x){
			if((uint) im.at<uchar>(y,x) > thresh){
				outIm.at<uchar>(y,x) = 255;
			}
			else{
				outIm.at<uchar>(y,x) = 0;
			}
		}
	}
	
	cout << "threshold level = " << thresh << endl;
	namedWindow("original", WINDOW_AUTOSIZE);
	imshow("original", im);
	namedWindow("otsu", WINDOW_AUTOSIZE);
	imshow("otsu", outIm);
	
	t = ((double)getTickCount() - t)/getTickFrequency();
	cout << t << endl;
	waitKey(0);
	return 0;
}

	
	
	
	
	
	
	
	
	
