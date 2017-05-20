#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;


int erosion(Mat &im, Mat &outIm, string type);
int dilation(Mat &im, Mat &outIm, string type);
int subtract(Mat &a, Mat &b, Mat &result);

#endif
