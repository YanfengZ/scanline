#pragma once 
#include "scanlinezbuffer.h"
#include "obj.h"
#include <algorithm>
#define INF 100000

using namespace cv;
scanlinezbuffer::scanlinezbuffer(int w, int h) {
	width = w;
	height = h;
	m_zbuffer.resize(w);
	m_fbuffer.resize(w, h);
}


void scanlinezbuffer::InitEtAndPt(Obj obj) {
	ET.clear();
	PT.clear();
	m_zbuffer.resize(width);
	m_fbuffer.resize(width, height);
	ET.resize(height+1);
	PT.resize(height+1);
	for (auto it = obj.surfaces.begin(); it != obj.surfaces.end(); it++) {
		if ((*it).normal[2] ==0.f) continue;
		double MinY = INF, MaxY = -INF;
		ploygon Ploygon;
		Ploygon.color = (*it).c;
		Ploygon.ploy_id = (*it).id;


		int ind1, ind2;
		for (auto j = (*it).face.begin(); j != (*it).face.end(); j++) {
			edge Edge;
			Edge.ploy_id = (*it).id;
			Edge.flag = false;
			ind1 = (*j);
			if (ind1 == (*it).face.back()) {
				ind2 = (*it).face[0];
			}
			else ind2 = *(j + 1);

			Point3d p1, p2;
			p1 = obj.points[ind1 - 1];
			p2 = obj.points[ind2 - 1];


			MinY = MinY < p1.y ? MinY : p1.y;
			MaxY = MaxY > p1.y ? MaxY : p1.y;

			Edge.dy = ceil(p1.y) - ceil(p2.y);
			if (Edge.dy == 0) continue;
			else if (Edge.dy > 0) {
				Edge.x = p1.x;
				Edge.minx = p2.x;
				//cout << p1.x << endl;
				Edge.dx = -(p1.x - p2.x) / (p1.y - p2.y);
				ET[ceil(p1.y)].push_back(Edge);
			}
			else {
				Edge.x = p2.x;
				Edge.minx = p1.x;
				Edge.dx = -(p2.x - p1.x) / (p2.y - p1.y);
				Edge.dy = -Edge.dy;
				ET[ceil(p2.y)].push_back(Edge);
			}
		}
		Ploygon.dy = ceil(MaxY) - ceil(MinY);
		if (Ploygon.dy != 0) {
			Ploygon.a = (*it).normal[0];
			Ploygon.b = (*it).normal[1];
			Ploygon.c = (*it).normal[2];
			Point3d p = obj.points[(*it).face[0]-1];
			Ploygon.d = -(p.x * Ploygon.a + p.y * Ploygon.b + p.z * Ploygon.c);
			PT[ceil(MaxY)].push_back(Ploygon);
		}
	}
}

edge* scanlinezbuffer::FindEdgeFromET(int y,int ploy_id) {
	for (auto it = ET[y].begin(); it != ET[y].end(); it++) {
		if ((*it).ploy_id == ploy_id && (*it).flag == false) {
			(*it).flag = true;
			return &(*it);
		}
	}
	return NULL;
}

	//ploygon* scanlinezbuffer::FindPloygonFromIPL(int ploy_id) {
	//	for (auto it = IPL.begin(); it != IPL.end(); it++) {
	//		if ((*it).ploy_id == ploy_id) {
	//			return &(*it);
	//		}
	//	}
	//	return NULL;
	//}
void scanlinezbuffer::AddEdgeToAET(int y,ploygon ploy) {
	edge* left, * right;
	left = FindEdgeFromET(y, ploy.ploy_id);
	right = FindEdgeFromET(y, ploy.ploy_id);
	if (left->x > right->x||(left->x == right->x && left->dx > right->dx)) {
		swap(left, right);
	}
	active_edge NewNode;
	NewNode.xl = left->x;
	NewNode.minxl = left->minx;
	NewNode.dxl = left->dx;
	NewNode.dyl = left->dy;
	NewNode.xr = right->x;
	NewNode.minxr = right->minx;
	NewNode.dxr = right->dx;
	NewNode.dyr = right->dy;
	NewNode.ploy_id = ploy.ploy_id;
	NewNode.dzx = -(ploy.a / ploy.c);
	NewNode.dzy = ploy.b / ploy.c;
	NewNode.zl = -(ploy.a * NewNode.xl + ploy.b * y + ploy.d) / ploy.c;
	NewNode.p = &ploy;
	AET.push_back(NewNode);
}
void scanlinezbuffer::AddPloygonToIPL(int y) {
	for (auto it = PT[y].begin(); it != PT[y].end(); it++) {
		IPL.push_back(*it);
		AddEdgeToAET(y, (*it));
	}
}

Color scanlinezbuffer::GetColorById(int ploy_id) {
	for (auto it = IPL.begin(); it != IPL.end(); it++) {
		if ((*it).ploy_id == ploy_id) {
			return (*it).color;
		}
	}
}

void scanlinezbuffer::UpdateIPL(int y) {
	for (auto it = IPL.begin(); it != IPL.end(); ) {
		
		if ((*it).dy <= 0) {
			for (auto e = AET.begin(); e != AET.end();) {
				if ((*e).ploy_id == (*it).ploy_id) {
					e = AET.erase(e);
				}
				else e++;
			}
			it = IPL.erase(it);
		}
		else {
			(*it).dy--;
			it++;
		}
	}
}

void scanlinezbuffer::UpdateAET(int y) {
	for (auto it = AET.begin(); it != AET.end();it++) {
		(*it).dyl--;
		(*it).dyr--;
		ploygon *p = (*it).p;
		if ((*it).dyl <= 0 && (*it).dyr <= 0) {
			edge* left, * right;
			left = FindEdgeFromET(y, (*it).ploy_id);
			right = FindEdgeFromET(y, (*it).ploy_id);


			if (left && right) {
				if (left->x > right->x||(left->x==right->x&&left->dx>right->dx)) {
					swap(left, right);
				}
				//active_edge NewNode;
				(*it).xl = left->x;
				(*it).minxl = left->minx;
				(*it).dxl = left->dx;
				(*it).dyl = left->dy;
				(*it).xr = right->x;
				(*it).minxr = right->minx;
				(*it).dxr = right->dx;
				(*it).dyr = right->dy;
				//(*it).ploy_id = ploy.ploy_id;
				//(*it).dzx = -(ploy.a / ploy.c);
				//(*it).dzy = ploy.b / ploy.c;
				(*it).zl = -(p->a * left->x + p->b * y + p->d) / p->c;
			}
		}
		else if ((*it).dyl <=0 && (*it).dyr > 0) {
			edge* left = FindEdgeFromET(y, (*it).ploy_id);
			(*it).xl = left->x;
			(*it).minxl = left->minx;
			(*it).dxl = left->dx;
			(*it).dyl = left->dy;
			(*it).zl = -(p->a * left->x + p->b * y + p->d) / p->c;
		}
		else if ((*it).dyl> 0 && (*it).dyr <= 0) {
			edge* right = FindEdgeFromET(y, (*it).ploy_id);
			(*it).xr = right->x;
			(*it).minxr = right->minx;
			(*it).dxr = right->dx;
			(*it).dyr = right->dy;
		}
		/*(*it).xl += (* it).dxl*(1<(y-(*it).minyl)?1: (y-(*it).minyl));
		(*it).xr += (*it).dxr* (1 < (y-(*it).minyr ) ? 1 : (y-(*it).minyr ));
		(*it).zl += (*it).dzx * (*it).dxl + (*it).dzy;*/
		if ((*it).dyl == 1) {
			(*it).xl = (*it).minxl;
			
		}
		else { 
			(*it).xl += (*it).dxl; 
			
		}
		//(*it).zl += (*it).dzx * (*it).dxl + (*it).dzy;
		if ((*it).dyr == 1) (*it).xr = (*it).minxr;
		else (*it).xr += (*it).dxr;
		(*it).zl += (*it).dzx * (*it).dxl + (*it).dzy;
	}
}

void scanlinezbuffer::Scan(Obj obj) {
	InitEtAndPt(obj);
	for (int y = height; y >0; y--) {
		for (int x = 0; x < width; x++) {
			m_zbuffer.setdepth(x, -INF);
		}
		AddPloygonToIPL(y);
		for (auto it = AET.begin(); it != AET.end(); it++) {
			Color c = GetColorById((*it).ploy_id);
			int MinX = ceil((*it).xl);
			int MaxX = ceil((*it).xr);
			//if (MinX > MaxX) swap(MinX, MaxX);
			/*if (MinX > MaxX){
				
				MinX = (int)ceil(MinX < (*it).minxl ? MinX : (*it).minxl);
				MaxX = (int)ceil(MaxX > (*it).minxr ? MaxX: (*it).minxr); 
				swap(MinX, MaxX);
			}*/
			
			//	MinX = (int)ceil(MinX > (*it).minxl ? MinX : (*it).minxl);
				//MaxX = (int)ceil(MaxX < (*it).minxr ? MaxX : (*it).minxr);
			
			

			double z = (*it).zl + (MinX - (*it).xl) * (*it).dzx;
			//double z = (*it).zl;
			for (int j = MinX; j <= MaxX&&j<width; j++) {
				if (z > m_zbuffer.getdepth(j)) {
					m_zbuffer.setdepth(j, z);
					//m_fbuffer.fbuffer[y*width]
					m_fbuffer.setpixel(j, y, c);
				}
				z += (*it).dzx;
			}

		}
		UpdateIPL(y-1);
		UpdateAET(y-1);
	}
}

//void scanlinezbuffer::Display() {
//	namedWindow("display", WINDOW_AUTOSIZE);
//	Mat img(width, height, CV_8UC3);
//	img.resize(this->width, this->height);
//	obj.load("../models/f-16.obj");
//	while (true) {
//		Scan();
//		for (int i = 0; i < img.rows; i++) {
//			for (int j = 0; j < img.cols; j++) {
//				if (img.rows - 1 - i == 875 && j == 512)
//					int a = 1;
//				//cout << m_fbuffer.getpixel(j, img.rows - 1 - i).b_ << m_fbuffer.getpixel(j, img.rows - 1 - i).g_ << m_fbuffer.getpixel(j, img.rows - 1 - i).r_ << endl;
//				Vec3b c(m_fbuffer.getpixel(j, img.rows - 1 - i).b_, m_fbuffer.getpixel(j, img.rows - 1 - i).g_, m_fbuffer.getpixel(j, img.rows - 1 - i).r_);
//				img.at<Vec3b>(i, j) = c;
//			}
//		}
//		imshow("display", img);
//		int key = waitKeyEx();
//		cout << key << endl;
//		if (key == 27) {
//			break;
//		}
//		else if (key == 2555904) obj.turnLeft();
//		else if (key == 2424832) obj.turnRight();
//		else if (key == 2424832) obj.turnUp();
//		else if (key == 2621440) obj.turnDown();
//
//	}
//}