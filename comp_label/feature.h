#ifndef FEATURE_H
#define FEATURE_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

template<typename T> 
class Coordinate
{	
	public:
	T x;
	T y;
	T u;
	T v;
	template<typename Y> Coordinate(Y i, Y j){
		x = i;
		y = j;
	}
	template<typename Y> Coordinate(Y i, Y j, Y k, Y l){
		x = i;
		y = j;
		u = k;
		v = l;
	}
};


Coordinate<double> Centroid(Mat &im, Coordinate<int> vertices);
double Area(Mat &im, Coordinate<int> vertices);
Coordinate<int> BoundingBox(Mat &im);
double HorSymm(Mat &im, Coordinate<int> vertices);
double VerSymm(Mat &im, Coordinate<int> vertices);
int EulerCalc(Mat &im, Coordinate<int> vertices);

#endif
