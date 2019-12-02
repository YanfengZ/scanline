// CG_Scanline.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#pragma once
#include <iostream>
#include "obj.h"
#include "scanlinezbuffer.h"
#include "intervalscanline.h"
int main()
{
	int width = 1024, height = 1024;
	Obj obj(width,height);
	obj.load("../models/f-16.obj");
	scanlinezbuffer sz(width, height);
	intervalscanline is(width, height);
	namedWindow("display", WINDOW_AUTOSIZE);
	Mat img(width, height, CV_8UC3);
	img.resize(width, height);
	while (true) {
		sz.Scan(obj);
		for (int i = 0; i < img.rows; i++) {
			for (int j = 0; j < img.cols; j++) {
					//cout << m_fbuffer.getpixel(j, img.rows - 1 - i).b_ << m_fbuffer.getpixel(j, img.rows - 1 - i).g_ << m_fbuffer.getpixel(j, img.rows - 1 - i).r_ << endl;
				Vec3b c(sz.m_fbuffer.getpixel(j, img.rows - 1 - i).b_, sz.m_fbuffer.getpixel(j, img.rows - 1 - i).g_, sz.m_fbuffer.getpixel(j, img.rows - 1 - i).r_);
				img.at<Vec3b>(i, j) = c;
			}
		}
		imshow("display", img);
		int key = waitKeyEx();
		//cout << key << endl;
		if (key == 27) {
			break;
		}
		else if (key == 2555904) obj.turnLeft();
		else if (key == 2424832) obj.turnRight();
		else if (key == 2490368) obj.turnUp();
		else if (key == 2621440) obj.turnDown();
	}
		
	while (true) {
		is.scan(obj);
		for (int i = 0; i < img.rows; i++) {
			for (int j = 0; j < img.cols; j++) {
				//cout << m_fbuffer.getpixel(j, img.rows - 1 - i).b_ << m_fbuffer.getpixel(j, img.rows - 1 - i).g_ << m_fbuffer.getpixel(j, img.rows - 1 - i).r_ << endl;
				Vec3b c(is.m_fbuffer.getpixel(j, img.rows - 1 - i).b_, is.m_fbuffer.getpixel(j, img.rows - 1 - i).g_, is.m_fbuffer.getpixel(j, img.rows - 1 - i).r_);
				img.at<Vec3b>(i, j) = c;
			}
		}
		imshow("display", img);
		int key = waitKeyEx();
		//cout << key << endl;
		if (key == 27) {
			break;
		}
		else if (key == 2555904) obj.turnLeft();
		else if (key == 2424832) obj.turnRight();
		else if (key == 2490368) obj.turnUp();
		else if (key == 2621440) obj.turnDown();
	}

    //std::cout << obj.surfaces.back().id; 
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
