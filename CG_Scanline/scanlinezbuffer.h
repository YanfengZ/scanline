#pragma once

#include "scanline.h"
#include "obj.h"

struct active_edge {
	double xl, dxl,minxl;
	double xr, dxr,minxr;
	int dyl, dyr;
	double zl, dzx, dzy;
	int ploy_id;
	ploygon* p;
};
class zbuffer {
public:
	zbuffer(int w) :width(w) {};
	zbuffer(void) {};
	~zbuffer(void) {};

	void resize(int w) {
		width = w;
		buffer.clear();
		buffer.resize(w);
	}

	double getdepth(int col) {
		return buffer[col];
	}
	void setdepth(int col, double z) {
		buffer[col] = z;
	}
private:
	vector<double>buffer;
	int width;
};

class scanlinezbuffer {
public:
	//Obj obj;
	int width, height;
	framebuffer m_fbuffer;
	zbuffer m_zbuffer;
	scanlinezbuffer(int w, int h);
	~scanlinezbuffer(void) {};
	void InitEtAndPt(Obj obj);
	void AddEdgeToAET(int y,ploygon ploy);
	void AddPloygonToIPL(int y);
	void UpdateAET(int y);
	void UpdateIPL(int y);
	edge* FindEdgeFromET(int y, int ploy_id);
	Color GetColorById(int ploy_id);
	///ploygon* FindPloygonFromIPL(int ploy_id);
	void Scan(Obj obj);
	//void Display();
private:
	vector<vector<edge>>ET;
	vector<vector<ploygon>>PT;
	vector<active_edge>AET;
	vector<ploygon>IPL;
	
};