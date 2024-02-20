#include <iostream>
#include <vector>
#include <cmath>

// 最小二乘法拟合直线的函数
void leastSquaresFit(const std::vector<double>& x, const std::vector<double>& y, double& slope, double& intercept) {
    int n = x.size();
    if (n != y.size() || n == 0) {
        std::cerr << "Error: Invalid input data\n";
        return;
    }

    double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_x_squared = 0.0;

    // 计算各种累加值
    for (int i = 0; i < n; ++i) {
        sum_x += x[i];
        sum_y += y[i];
        sum_xy += x[i] * y[i];
        sum_x_squared += x[i] * x[i];
    }

    // 计算斜率和截距
    slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x_squared - sum_x * sum_x);
    intercept = (sum_y - slope * sum_x) / n;
}

int main() {
    // 示例数据
    // std::vector<double> x = {1.0, 2.0, 3.0, 4.0, 5.0};
    // std::vector<double> y = {2.0, 3.0, 3.5, 4.0, 5.0};
    std::vector<double> x = {1.0, 2.0, 3.0, 4.0, 5.0};
    std::vector<double> y = {20.0, 35.0, 40.0, 50.0, 55.0};

    // 计算最小二乘法拟合直线
    double slope, intercept;
    leastSquaresFit(x, y, slope, intercept);

    // 输出结果
    std::cout << "拟合直线方程: y = " << slope << "x + " << intercept << std::endl;

    return 0;
}
