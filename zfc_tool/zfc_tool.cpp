// zfc_tool.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <limits>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

static double start_value = 0.87f;
static double end_value = 1.35f;
const float step = 0.01f;
const int X_COUNT = 6;


double ff1 = 0.f;
double ff1b = 0.f;
double pp[X_COUNT] = {};
double ppb[X_COUNT] = {};
double dd[X_COUNT] = {};
double xx[X_COUNT];
double max_value = 0.f;
double temp_xx[X_COUNT];

double min_value = (numeric_limits<double>::max)();

double temp_min_xx[X_COUNT];

int times = 0;
bool find_max = false;
void printarr() {
	for (auto it : xx) {
		printf("%f ", it);
	}
	cout << endl;
}
void search(int index) {
	if (index >= X_COUNT - 2) {
		double f1 = 0.f;
		double f1b = 0.f;
		double f2 = 0.f;
		for (int i = 0; i < X_COUNT - 2; ++i) {
			f1 += (xx[i] * pp[i]);
			f1b += (xx[i] * ppb[i]);
			f2 += (xx[i] * dd[i]);
		}
		auto a1 = pp[4];
		auto b1 = pp[5];
		auto c1 = ff1 - f1;
		auto a2 = ppb[4];
		auto b2 = ppb[5];
		auto c2 = ff1b - f1b;
		double x5 = (b1*c2 - b2 * c1) / (b1*a2 - b2 * a1);
		double x6 = (c1*a2 - c2 * a1) / (b1*a2 - b2 * a1);
		++times;

		xx[4] = x5;
		xx[5] = x6;

		if (x6 < start_value - 0.000001f || x6 > end_value + 0.000001f) return;
		if (x5 < start_value - 0.000001f || x5 > end_value + 0.000001f) return;

		double value = f2 + xx[4] * dd[4] + xx[5] * dd[5];
		if (min_value > value) {
			min_value = value;
			memcpy(temp_xx, xx, sizeof(xx));
			find_max = true;
		}
		return;
	}
	for (; xx[index] <= end_value + 0.00001f; xx[index] += step) {
		if (index + 1 < X_COUNT) {
			xx[index + 1] = start_value;
		}
		search(index + 1);
	}
}

int main(int argc, char* argv[])
{
	if (argc != X_COUNT * 3 + 1 + 1 + 1) {
		printf("请以此输入ABCDEF abcdef hijklm f1 f2，总共20个数，当前输入数量=%d\n", argc - 1);
		return 0;
	}
	
	for (int i = 0; i < X_COUNT * 3; ++i) {
		auto& temp = i < X_COUNT ? pp[i] : ((i >= X_COUNT && i < X_COUNT * 2) ? ppb[i - X_COUNT] : dd[i - X_COUNT * 2]);
		temp = atof(argv[i + 1]);
	}
	ff1 = atof(argv[argc - 2]);
	ff1b = atof(argv[argc - 1]);
	for (auto& it : xx) {
		it = start_value;
	}
	search(0);
	if (!find_max) {
		cout << "没有找到合适的值" << endl << "次数=" << times << endl;
	}
	else 
	{
		cout << "最小值：" << min_value << "，次数：" << times;
		cout << " 其中x1 x2 x3 x4 x5 x6 分别为：";
		for (auto it : temp_xx) {
			cout << it << " ";
		}
		cout << endl;
	}
	cout << "f1 = " << ff1 << endl;
	cout << "f2 = " << ff1b << endl;
	cout << "ABCDEF 分别为：";
	for (auto it : pp) {
		cout << it << " ";
	}
	cout << endl;
	cout << "abcdef 分别为：";
	for (auto it : ppb) {
		cout << it << " ";
	}
	cout << endl;
	cout << "hijklm 分别为：";
	for (auto it : dd) {
		cout << it << " ";
	}
	cout << endl;
    return 0;
}

