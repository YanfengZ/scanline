#pragma once
#include "obj.h"
#define MAXLEN 10000
#define INF 10000
#define PI 3.14
#include <algorithm>
void Obj::calcolor(Face& f) {
	//TODO 根据法向量算平面颜色
	
	
	/*int r =(int) rand() % 255;
	int g = (int)rand() % 255;
	int b = (int)rand() % 255;
	f.c = Color(r, g, b);*/

	int cos = (int) 255*fabs(f.normal[2]);
	f.c = Color(cos,cos,cos);
}
void Obj::calnormal(Face& f) {
	Point3d p1, p2, p3;
	p1 = points[f.face[0]-1];
	p2 = points[f.face[1]-1];
	p3 = points[f.face[2]-1];
	Vec3d v1, v2;
	v1 = p1 - p2;
	v2 = p1 - p3;
	v1= v1.cross(v2);
	v1 = v1 / sqrt(pow(v1[0], 2) + pow(v1[1], 2) + pow(v1[2], 2));
	f.normal = v1;
}

void Obj::adjust(double scale, Point3d move) {
	for (auto &p : points) {
		p = p * scale + move;
		points_init.push_back(p - center);
	}
	
}
void Obj::load(string path) {
	points.clear();
	fstream file;
	file.open(path);
	if (!file.is_open()) {
		cout << "无法打开obj文件" << endl;
	}

	int countface = 0;
	Point3d MIN_P(INF, INF, INF), MAX_P(-INF, -INF, -INF);
	char* ch;
	ch = (char*) malloc(sizeof(char) * MAXLEN);
	while (file.getline(ch, MAXLEN)) {
		if (ch[0] == 'v' && ch[1] == ' ') {
			Point3d p;
			sscanf_s(ch,"v %lf%lf%lf", &p.x, &p.y, &p.z);
			MIN_P.x = MIN_P.x < p.x ? MIN_P.x: p.x;
			MIN_P.y = MIN_P.y < p.y ? MIN_P.y : p.y;
			MIN_P.z = MIN_P.z < p.z ? MIN_P.z : p.z;
			MAX_P.x = MAX_P.x > p.x ? MAX_P.x : p.x;
			MAX_P.y = MAX_P.y > p.y ? MAX_P.y : p.y;
			MAX_P.z = MAX_P.z > p.z ? MAX_P.z : p.z;
			points.push_back(p);
			//points_init.push_back(p);
			
		}
		else if (ch[0] == 'f' && ch[1] == ' ') {
			Face f;
			int index = 2;
			while (ch[index]) {
				int pointid = 0;
				bool negative = false;
				if (ch[index] == '-') {
					negative = true;
					index++;
				}
				while(ch[index] >= '0' && ch[index] <= '9') {
					pointid = 10 * pointid + (ch[index] - '0');
					index++;
				}
				if (negative) {
					pointid = points.size() - pointid;
				}
				f.face.push_back(pointid);
				while (ch[index] != ' ' && ch[index]) {
					index++;
				}
				if (ch[index])
					index++;
			}

			calnormal(f);
			calcolor(f);
			separate(f, countface);
			int i = 0;
			//surfaces.push_back(f);

		}		
	}
	file.close();
	double w = sqrt(pow((MAX_P.x - MIN_P.x), 2) + pow((MAX_P.y - MIN_P.y), 2) + pow((MAX_P.z - MIN_P.z), 2));
	double scale = width / w;
	center = Point3d(width / 2, height / 2, 512);
	Point3d move = center - (MIN_P + MAX_P) * scale / 2;
	adjust(scale, move);
	cout << "从" << path << "中共读取" << points.size() << "个点," << countface << "个面。" << endl;
	
}

void Obj::separate(Face f,int& countface) {
	if (f.face.size() > 3) {//将面片分解为多个三角形
		int v;
		v = f.face[0];//固定一个顶点
		for (auto it = f.face.begin() + 1; it != f.face.end() - 1; it++) {
			countface++;
			Face newf;
			newf.face.push_back(v);
			newf.face.push_back((*it));
			newf.face.push_back((*(it + 1)));
			newf.normal = f.normal;
			newf.c = f.c;
			newf.id = countface;
			surfaces.push_back(newf);
		}
	}
	else {
		countface++;
		f.id = countface;
		surfaces.push_back(f);
	}
}


void Obj::rotate() {
	double sin_theta = sin(PI * theta / 180);
	double cos_theta = cos(PI * theta / 180);
	double sin_beta = sin(PI * beta / 180);
	double cos_beta = cos(PI * beta / 180);
	double t[3][3] = { {cos_theta,					0.f,		-sin_theta},
					 {sin_beta * sin_theta,		cos_beta,		sin_beta * cos_theta},
					 {cos_beta * sin_theta,		-sin_beta,		cos_beta * cos_theta} };

	for (int i = 0; i < points.size(); i++) {
		auto& cur = points[i];
		auto& p = points_init[i];
		double n[3];
		for (int j = 0; j < 3; j++) {
			n[j] = t[j][0] * p.x + t[j][1] * p.y + t[j][2] * p.z;
		}

		cur.x = n[0] + center.x;
		cur.y = n[1] + center.y;
		cur.z = n[2] + center.z;
	}
	for (auto it = surfaces.begin(); it != surfaces.end(); it++) {
		calnormal((*it));
		calcolor((*it));
	}
}

void Obj::turnLeft()
{
	theta = (theta - 10 + 360) % 360;
	rotate();
}

void Obj::turnRight()
{
	theta = (theta + 10) % 360;
	rotate();
}

void Obj::turnUp()
{
	beta = min(beta + 5, 90);
	rotate();
}

void Obj::turnDown()
{
	beta = max(-90, beta - 5);
	rotate();
}
