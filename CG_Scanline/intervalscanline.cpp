#pragma once 
#include "intervalscanline.h"
#define INF 10000
#include <algorithm>
intervalscanline::intervalscanline(int w, int h) {
	width = w;
	height = h;
	m_fbuffer.resize(w, h);
}

void intervalscanline::InitPTAndET(Obj obj) {
	ET.clear();
	PT.clear();
	m_fbuffer.resize(width, height);
	ET.resize(height + 1);
	PT.resize(height + 1);
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
			Point3d p = obj.points[(*it).face[0] - 1];
			Ploygon.d = -(p.x * Ploygon.a + p.y * Ploygon.b + p.z * Ploygon.c);
			Ploygon.flag = -1;
			PT[ceil(MaxY)].push_back(Ploygon);
		}
	}
}
int intervalscanline::UpdateFlag(int ploy_id) {
	for (auto it = IPL.begin(); it != IPL.end(); it++) {
		if ((*it).ploy_id == ploy_id) {
			(*it).flag = -(*it).flag;
			return (*it).flag;
		}
	}
}

void intervalscanline::AddEdgeToAET(int y, ploygon ploy) {
	for (auto it = ET[y].begin(); it != ET[y].end(); it++) {
		if ((*it).ploy_id == ploy.ploy_id&&(*it).flag==false) {
			AET.push_back((*it));
			(*it).flag = true;
		}
	}
}
void intervalscanline::AddPloyToIPL(int y) {
	for (auto it = PT[y].begin(); it != PT[y].end(); it++) {
		IPL.push_back((*it));
		AddEdgeToAET(y, (*it));
	}
	//sort(AET.begin(), AET.end());
}

void intervalscanline::UpdateIPL(int y) {
	for (auto it = IPL.begin(); it != IPL.end();) {
		
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
edge* intervalscanline::FindEdgeFromET(int y,int ploy_id) {
	for (auto it = ET[y].begin(); it!= ET[y].end(); it++) {
		if ((*it).ploy_id == ploy_id&&(*it).flag!=true) {
			(*it).flag = true;
			return &(*it);
		}
	}
	return NULL;
}
void intervalscanline::UpdateAET(int y) {
	for (auto it = AET.begin(); it != AET.end();) {
		(*it).dy--;
		if ((*it).dy <= 0) {
			edge* e = FindEdgeFromET(y,(*it).ploy_id);
			if (e == NULL) {
				it = AET.erase(it);
			}
			else {
				(*it) = *e;
				it++;
			}
		}
		else {
			if ((*it).dy <= 1) {
				(*it).x = (*it).minx;
			}
			else (*it).x += (*it).dx;
			it++;
		}
	}
	sort(AET.begin(), AET.end());
}


Color intervalscanline::GetColorById(int ploy_id) {
	for (auto it = IPL.begin(); it != IPL.end(); it++) {
		if ((*it).ploy_id == ploy_id) {
			return (*it).color;
		}
	}
	return Color(255, 0, 0);
}

void intervalscanline::scan(Obj obj) {
	InitPTAndET(obj);

	for (int y = height; y > 0; y--) {
		AddPloyToIPL(y);
		sort(AET.begin(), AET.end());
		int count = 0;
		double xl = 0, xr;
		for (auto e = AET.begin(); e != AET.end(); e++) {
			xr = (*e).x;
			/*int MaxX, MinX;
			MaxX = (int )ceil(el.x);
			MinX = (int)ceil(er.x);*/
			if (xl >= xr) {
				count = UpdateFlag((*e).ploy_id)==1? count + 1 : count - 1;
				continue;
			}
			if (count == 0) {
				//for (int i = ceil(xl); i <= ceil(xr); i++) {
				//	//if (i < 10) cout << i << endl;
				//	m_fbuffer.setpixel(i, y, Color(255,0,0));
				//}
			}
			else if (count == 1) {
				Color c = GetColorById((*e).ploy_id);
				for(int i=ceil(xl);i<=ceil(xr);i++){
					//if (i < 10) cout << i << endl;
					m_fbuffer.setpixel(i, y, c);
				}
			}
			else {
				int idl, idr;
				double la, lb, lc, ld, ra, rb, rc, rd;
				double zlmax = -INF, zrmax = -INF;
				double zl, zr;
				for (auto ploy = IPL.begin(); ploy != IPL.end(); ploy++) {
					if ((*ploy).flag == 1) {
						zl = -((*ploy).a * xl + (*ploy).b * y + (*ploy).d) / (*ploy).c;
						zr = -((*ploy).a * xr + (*ploy).b * y + (*ploy).d) / (*ploy).c;
						if (zlmax < zl) {
							zlmax = zl;
							idl = (*ploy).ploy_id;
							la = (*ploy).a;
							lb = (*ploy).b;
							lc = (*ploy).c;
							ld = (*ploy).d;
						}
						if (zrmax < zr) {
							zrmax = zr;
							idr = (*ploy).ploy_id;
							ra = (*ploy).a;
							rb = (*ploy).b;
							rc = (*ploy).c;
							rd = (*ploy).d;
						}
					}
				}
				if (idl != idr) {
					// (-(r_poly_c * l_poly_b * y) + (l_poly_c * r_poly_b * y) - (r_poly_c * l_poly_d) + (l_poly_c * r_poly_d)) / ((r_poly_c * l_poly_a) - (l_poly_c * r_poly_a))
					double Intersection =(-(rc*lb*y)+(lc*rb*y)-(rc*ld)+(lc*rd))/((rc*la)-(lc*ra));
					Color c = GetColorById(idl);
					for (int i = ceil(xl); i < ceil(Intersection)&& i <= ceil(xr); i++) {
						//if (i < 10) cout << i << endl;
						m_fbuffer.setpixel(i, y, c);
					}
					c = this->GetColorById(idr);
					for (int i = ceil(Intersection); i <= ceil(xr)&&i>ceil(xl); i++) {
						//if (i < 10) cout << i << endl;
						m_fbuffer.setpixel(i, y, c);
					}

				}
				else {
					Color c =GetColorById(idl);
					for (int i =ceil( xl); i <= ceil(xr); i++) {
						//if (i < 10) cout << i << endl;
						m_fbuffer.setpixel(i, y, c);
					}
				}
				
			}
			count =UpdateFlag((*e).ploy_id)==1? count + 1 : count - 1;
			xl = xr;
		}
		//cout << count << endl;
		UpdateIPL(y - 1);
		UpdateAET(y - 1);
	}
}