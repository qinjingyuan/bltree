//
// Created by admin on 2024/1/16.
//
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// 定义最小二乘法函数
void leastSquaresFit(const vector<double>& x, const vector<double>& y, double& slope, double& intercept) {
    int n = x.size();

    // 计算 x 和 y 的平均值
    double sumX = 0.0, sumY = 0.0;
    for (int i = 0; i < n; ++i) {
        sumX += x[i];
        sumY += y[i];
    }
    double meanX = sumX / n;
    double meanY = sumY / n;

    // 计算最小二乘法的系数
    double numerator = 0.0, denominator = 0.0;
    for (int i = 0; i < n; ++i) {
        numerator += (x[i] - meanX) * (y[i] - meanY);
        denominator += pow(x[i] - meanX, 2);
    }

    // 计算斜率和截距
    slope = numerator / denominator;
    intercept = meanY - slope * meanX;
}

int main() {
    // 示例数据
    vector<double> x = {1.0, 2.0, 3.0, 4.0, 5.0};
    vector<double> y = {2.0, 4.0, 4.0, 6.0, 100.0};

    // 输出原始数据
    cout << "原始数据：" << endl;
    for (int i = 0; i < x.size(); ++i) {
        cout << "(" << x[i] << ", " << y[i] << ")" << endl;
    }

    // 调用最小二乘法函数
    double slope, intercept;
    leastSquaresFit(x, y, slope, intercept);

    // 输出最小二乘法的结果
    cout << "\n最小二乘法结果：" << endl;
    cout << "斜率 (slope): " << slope << endl;
    cout << "截距 (intercept): " << intercept << endl;

    return 0;
}
