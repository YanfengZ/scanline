#pragma once
#include<opencv2\opencv.hpp> 
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp> 
#include <vector>
#include <ostream>
#include <istream>

using namespace cv;
using namespace std;

class Color {
public:
	double r_, g_, b_;
	Color(void) {};
	Color(double r, double g, double b) :r_(r), g_(g), b_(b) {};
	~Color(void) {};

	void operator = (const Color& c) {
		r_ = c.r_;
		b_ = c.b_;
		g_ = c.g_;
	}
};

struct Face {
	vector<int>face;
	Vec3d normal;
	int id;
	Color c;
};

class Obj {
public:
	int width, height;
	int theta, beta;
	Obj(int w, int h) :width(w), height(h) {};
	Obj(void) {};
	vector<Face>surfaces;
	vector<Point3d>points;
	vector<Point3d>points_init;
	Point3d center;
	void load(string path);
	void calnormal(Face& f);
	void adjust(double scale, Point3d move);
	void separate(Face f,int& countface);
	void calcolor(Face& f);
	void rotate();
	void turnLeft();
	void turnRight();
	void turnUp();
	void turnDown();
};

