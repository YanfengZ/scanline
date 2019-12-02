#pragma once
#include <vector>
#include "obj.h"
using namespace std;

class framebuffer {
public:
	int width, height;
	framebuffer(int w, int h) :width(w), height(h) {};
	framebuffer(void) {};
	~framebuffer(void) {};

	void resize(int w, int h) {

		if (width != w || height != h) {
			fbuffer.clear();
			width = w;
			height = h;
			fbuffer.resize(w * h);
		}
		for (auto i =0; i< fbuffer.size(); i++) {
			Color c(255, 0,0);
			fbuffer[i] = c;
		}
	}

	Color getpixel(int col, int row) {
		return fbuffer[row * width + col];
	}

	void setpixel(int col, int row,Color c) {
		fbuffer[row*width + col ] = c;
	}

	vector<Color> fbuffer;
};


struct edge {
	double x, dx,minx;
	int dy, ploy_id;
	bool flag ;

	bool operator <(const edge& a) const {
		if (x != a.x) {
			return x < a.x;
		}
		else if(x==a.x&&dx!=a.dx){
			return dx < a.dx;
		}
		
	}
};

struct ploygon {
	int dy, ploy_id;
	double a, b, c, d;
	Color color;
	int flag;
};