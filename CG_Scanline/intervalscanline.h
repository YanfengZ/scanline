#pragma once
#include "scanline.h"

class intervalscanline {
public:
	int width, height;
	intervalscanline(int w, int h);
	~intervalscanline(void) {};
	framebuffer m_fbuffer;
	void InitPTAndET(Obj obj);
	void AddPloyToIPL(int y);
	void AddEdgeToAET(int y, ploygon ploy);
	void UpdateAET(int y);
	void UpdateIPL(int y);
	Color GetColorById(int ploy_id);
	void scan(Obj obj);
	int UpdateFlag(int ploy_id);
	edge* FindEdgeFromET(int y,int ploy_id);

private:
	vector<vector<ploygon>>PT;
	vector<vector<edge>>ET;
	vector<edge>AET;
	vector <ploygon>IPL;
};