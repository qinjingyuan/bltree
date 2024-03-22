#include <iostream>
#include <vector>
#include <cmath>

// 定义最小二乘法函数
void leastSquaresPolynomialFit(const std::vector<double>& x, const std::vector<double>& y, int degree, std::vector<double>& coefficients) {
    int n = x.size();
    int m = degree + 1;

    // 构造矩阵X和Y
    std::vector<std::vector<double>> X(n, std::vector<double>(m, 0.0));
    std::vector<double> Y(n, 0.0);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            X[i][j] = std::pow(x[i], j);
        }
        Y[i] = y[i];
    }

    // 计算 (X^T * X)^(-1) * X^T * Y
    std::vector<std::vector<double>> XT_X(m, std::vector<double>(m, 0.0));
    std::vector<double> XT_Y(m, 0.0);
    std::vector<double> temp(m, 0.0);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            for (int k = 0; k < m; ++k) {
                XT_X[j][k] += X[i][j] * X[i][k];
            }
            XT_Y[j] += X[i][j] * Y[i];
        }
    }

    // 解线性方程组
    for (int i = 0; i < m; ++i) {
        for (int j = i + 1; j < m; ++j) {
            double ratio = XT_X[j][i] / XT_X[i][i];
            for (int k = 0; k < m; ++k) {
                XT_X[j][k] -= ratio * XT_X[i][k];
            }
            XT_Y[j] -= ratio * XT_Y[i];
        }
    }

    for (int i = m - 1; i >= 0; --i) {
        double sum = 0.0;
        for (int j = i + 1; j < m; ++j) {
            sum += XT_X[i][j] * temp[j];
        }
        temp[i] = (XT_Y[i] - sum) / XT_X[i][i];
    }

    coefficients = temp;
}

int main() {
    // 样本数据
    std::vector<double> x = {1.0, 2.0, 3.0, 4.0, 5.0};
    std::vector<double> y = {2.0, 3.0, 4.0, 5.0, 6.0};
    int degree = 2; // 多项式的次数

    // 定义拟合多项式的系数向量
    std::vector<double> coefficients(degree + 1);

    // 使用最小二乘法拟合多项式
    leastSquaresPolynomialFit(x, y, degree, coefficients);

    // 输出拟合结果
    std::cout << "拟合多项式的系数为：";
    for (int i = 0; i < coefficients.size(); ++i) {
        std::cout << coefficients[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
