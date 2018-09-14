// zfc_tool.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <limits>

using namespace std;

static double start_value = 0.8f;
static double end_value = 1.5f;
const float step = 0.1f;
const int X_COUNT = 6;


double ff1 = 0.f;
double pp[X_COUNT] = {};
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
	if (index >= X_COUNT - 1) {
		double f1 = 0.f;
		double f2 = 0.f;
		for (int i = 0; i < X_COUNT - 1; ++i) {
			f1 += (xx[i] * pp[i]);
			f2 += (xx[i] * dd[i]);
		}
		double x6 = xx[X_COUNT - 1] = (ff1 - f1) / pp[X_COUNT - 1];
		if (x6 < start_value - 0.000001f || x6 > end_value + 0.000001f) return;

		double value = f2 + (xx[X_COUNT - 1] * dd[X_COUNT - 1]);
		if (max_value < value) {
			max_value = value;
			memcpy(temp_xx, xx, sizeof(xx));
			find_max = true;
		}
		++times;
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
	if (argc != X_COUNT * 2 + 1 + 1) {
		printf("请以此输入ABCDEFabcdef，总共13个数，当前输入数量=%d\n", argc - 1);
		return 0;
	}
	for (int i = 0; i < X_COUNT * 2; ++i) {
		auto& temp = i < X_COUNT ? pp[i] : dd[i - X_COUNT];
		temp = atof(argv[i + 1]);
	}
	ff1 = atof(argv[argc - 1]);
	for (auto& it : xx) {
		it = start_value;
	}
	search(0);
	if (!find_max) {
		cout << "没有找到合适的值" << endl;
	}
	else 
	{
		cout << "f2最大值：" << max_value;
		cout << " 其中x1 x2 x3 x4 x5 x6 分别为：";
		for (auto it : temp_xx) {
			cout << it << " ";
		}
		cout << endl;
	}
	cout << "f1 = " << ff1 << endl;
	cout << "ABCDEF 分别为：";
	for (auto it : pp) {
		cout << it << " ";
	}
	cout << endl;
	cout << "abcdef 分别为：";
	for (auto it : dd) {
		cout << it << " ";
	}
	cout << endl;
    return 0;
}

