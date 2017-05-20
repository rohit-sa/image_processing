#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include "feature.h"
#include "preprocessing.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int convertImage(uchar buffer[], Mat &outIm, bool preprpcessing);
int ccLabeling(Mat &im, Mat &label);
int minLabel(Mat &im, Mat &label, int row, int col);
int im2bin(Mat &im, int limit);


int main(int argc, char* argv[]){
	double t = (double) getTickCount();
	string path = "/home/r/Documents/C++ Projects/lenna.png";
	string filename(argv[1]);
	int setsize = 0;
	double t_centroid_x = 0, t_centroid_y = 0, t_area_ratio = 0, t_aspect_ratio = 0, t_hor_symm = 0, t_ver_symm =0;
	string labels = "euler,centroid_x,centroid_y,hor_symm,ver_symm,aspect_ratio,area_ratio\n";
	Mat im, colorMat, image1, outImg1;
	ofstream outfile;
	ifstream infile;
	uchar buffer[784];
	Coordinate<double> centroid_feature(0.0, 0.0);
	Coordinate<int> bounding_limit(0,0,0,0);
	
	colorMat = imread(path, CV_LOAD_IMAGE_COLOR);
	if(!colorMat.data){
		cout<< "can't open image"<< endl;
		return -1;
	}
	cvtColor(colorMat, image1, COLOR_BGR2GRAY);
	//Mat imLabel(im.rows, im.cols, CV_8UC1, Scalar(0));
	//im2bin(im,200);
 
	outfile.open("output/features_"+filename+".txt");
	outfile << labels ;
	String data_path = "train/out_"+filename;
	infile.open(data_path, ios::in|ios::binary);
	if (!infile.is_open())
	{
		cout << infile.is_open();
		cout << "File error"<< endl; 
		exit (1);
	}
	infile.seekg (0, infile.end);
    setsize = infile.tellg()/784;
    infile.seekg (0, infile.beg);
	for(int i = 0; i < setsize*784; i += 784){
		Mat out;
		infile.read((char *)buffer, 784);
		convertImage(buffer,out, 0);
		bounding_limit = BoundingBox(out);
		centroid_feature = Centroid(out, bounding_limit);
		//namedWindow("original", WINDOW_AUTOSIZE);
		//imshow("original", out);
		//waitKey(0);
		vector<double> feature_array;
		double aspect_ratio = ((bounding_limit.v - bounding_limit.y)/(bounding_limit.u - bounding_limit.x));		
		double area_ratio = Area(out, bounding_limit);
		double hor_symm = HorSymm(out, bounding_limit);
		double ver_symm = VerSymm(out, bounding_limit);
		//double euler = EulerCalc(out, bounding_limit);
		
		feature_array.push_back(EulerCalc(out, bounding_limit));
		feature_array.push_back(centroid_feature.x);
		feature_array.push_back(centroid_feature.y);
		feature_array.push_back(hor_symm);
		feature_array.push_back(ver_symm);
		feature_array.push_back(aspect_ratio);
		feature_array.push_back(area_ratio);
		
		t_aspect_ratio += aspect_ratio;
		t_centroid_x += centroid_feature.x;
		t_centroid_y += centroid_feature.y;
		t_area_ratio += area_ratio;
		t_hor_symm += hor_symm;
		t_ver_symm += ver_symm;
		
		string feature = "";
		for(int i = 0; i < (int)feature_array.size()-1; i++){
			feature += to_string(feature_array[i]) + ",";
		}
		feature += to_string(feature_array[feature_array.size()-1]);
		//cout << feature << endl;
		outfile << feature << endl;
	}
	infile.close();
	outfile.close();
	cout <<"DONE" << endl;
	
	//~ namedWindow("original", WINDOW_AUTOSIZE);
	//~ imshow("original", im);

	t = ((double)getTickCount() - t)/getTickFrequency();
	cout << "Exec time : " << t << endl;
	waitKey(0);
	return 0;
}

int convertImage(uchar buffer[], Mat &outIm, bool preprocessing){
	outIm = Mat(28, 28, CV_8UC1, Scalar(0));
	Mat preIm(28, 28, CV_8UC1, Scalar(0));
	Mat eroIm(28, 28, CV_8UC1, Scalar(0));
	for(int i = 0; i < 28; i++){
		for(int j = 0; j < 28; j++){
			//cout << (uint)buffer[(i*28) + j] << " ";
			if(buffer[(i*28) + j] > 0){
				preIm.at<uchar>(i,j) = 255;
			}
		}
	}
	//preprocessing
	if(preprocessing){
		erosion(preIm, eroIm,"cross");
		dilation(eroIm,outIm,"cross");
	}
	else{
		outIm = preIm;
	}
	return 0;
}
	
int ccLabeling(Mat &im, Mat &label){
	int currentLabel = 1;
	for(int i = 1; i < im.rows -1; i++){
		for(int j = 1; j <im.cols -1; j++){
			//~ Neighbours element(im, label, row, col);
			if( im.at<uchar>(i, j) > 200){
				//cout << i << j << " ";
				if(im.at<uchar>(i-1,j) < 200 && im.at<uchar>(i,j-1) < 200){
					label.at<uchar>(i,j) = currentLabel;
					//cout << i << j << " "<<currentLabel<< " ";
					currentLabel++;
				}
				else{
					if(im.at<uchar>(i-1,j) > 200 && im.at<uchar>(i,j-1) > 200){
						if(label.at<uchar>(i-1,j) >= label.at<uchar>(i,j-1)){ 
							label.at<uchar>(i,j) = label.at<uchar>(i,j-1);
							label.at<uchar>(i-1,j) = label.at<uchar>(i,j);
							//cout << i << j << " "<<(int)label.at<uchar>(i,j)<< " ";
						}
						else{
							label.at<uchar>(i,j) = label.at<uchar>(i-1,j);
							label.at<uchar>(i,j-1) = label.at<uchar>(i,j);
						}
					}
					else if(im.at<uchar>(i-1,j) > 200){
						label.at<uchar>(i,j) = label.at<uchar>(i-1,j);
					}
					else{
						label.at<uchar>(i,j) = label.at<uchar>(i,j-1);
						//cout << i << j << " "<<(int)label.at<uchar>(i,j)<< " ";
					}
				}
				//cout << i << j << " "<<(int)label.at<uchar>(i,j)<< " ";
			}
		}
	}
	//second pass
	for(int i = 1; i < im.rows -1; i++){
		for(int j = 1; j <im.cols -1; j++){
			if( im.at<uchar>(i, j) > 200){
				minLabel(im, label, i ,j);
			}
			//cout << i << j << " "<<(int)label.at<uchar>(i,j)<< " ";
		}
	}
	return 0;	
}

int minLabel(Mat &im, Mat &label, int i, int j){
	uchar im_array[8];
	uchar label_array[8];
	im_array[0] = im.at<uchar>(i-1,j);
	im_array[1] = im.at<uchar>(i-1,j-1);
	im_array[2] = im.at<uchar>(i,j-1);
	im_array[3] = im.at<uchar>(i+1,j-1);
	im_array[4] = im.at<uchar>(i+1,j);
	im_array[5] = im.at<uchar>(i+1,j+1);
	im_array[6] = im.at<uchar>(i,j+1);
	im_array[7] = im.at<uchar>(i-1,j+1);
	
	label_array[0] = label.at<uchar>(i-1,j);
	label_array[1] = label.at<uchar>(i-1,j-1);
	label_array[2] = label.at<uchar>(i,j-1);
	label_array[3] = label.at<uchar>(i+1,j-1);
	label_array[4] = label.at<uchar>(i+1,j);
	label_array[5] = label.at<uchar>(i+1,j+1);
	label_array[6] = label.at<uchar>(i,j+1);
	label_array[7] = label.at<uchar>(i-1,j+1);
	
	uchar min = label.at<uchar>(i,j);
	for(int k=0; k< 8; k++){
		if(im_array[k] > 200){
			if(min > label_array[k]){
				min = label_array[k];
			}
			else{
				label_array[k] = min;
			}
		}
	}
	label.at<uchar>(i-1,j) = label_array[0];
	label.at<uchar>(i-1,j-1)= label_array[1];
	label.at<uchar>(i,j-1)= label_array[2];
	label.at<uchar>(i+1,j-1)= label_array[3];
	label.at<uchar>(i+1,j)= label_array[4];
	label.at<uchar>(i+1,j+1)= label_array[5];
	label.at<uchar>(i,j+1)= label_array[6];
	label.at<uchar>(i-1,j+1)= label_array[7];
	return 0;
}

int im2bin(Mat &im, int limit){
	
	int nRows = im.rows;
	int nCols = im.channels() * im.cols;
	int i,j;
	uchar *p;
	
	for(i = 0; i < nRows; ++i){
		p = im.ptr<uchar>(i);
		for(j = 0; j < nCols; j++){
			if((int)p[j] > limit){
				p[j] = 255;
			}
			else{
				p[j] = 0;
			}
		}
	}
	return 0;
}
