

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <istream>
#include <ostream>
#include <memory>
#include <cstddef>
#include <cassert>
#include <chrono>
#include <iostream>
#include <cmath>
#include <string>
#include "stx/btree.h"


namespace stx{


    // generate duoxiangshi model
    template<typename T>
    bool btree::generate_func_model_integrate(T* n){
        // sure s1 != 0, define 5 site for compute k.
        if(n->slotuse < 32) {
            n->model_type = modelType::GENERAL;
            return true;
        }

        // array sites
        int x4 = n->slotuse - 1;
        int x2 = x4 >> 1;
        int x1 = x2 >> 1;
        int x3 = x2 + x1;
        int x0 = 0;


        size_t value_gaps = n->slotkey[x4] - n->slotkey[x0];
        size_t value_base = n->slotkey[x0];
        int first1 = get_first_1_site(value_gaps) ;
        int right_move_bits = first1 > 8 ? first1 - 8 : 0;
        n->key_move_bits = right_move_bits;
        n->value_base = value_base;
        
        // 计算4段数据的斜率
        auto ry0 = static_cast<double>((n->slotkey[x0] - n->slotkey[x0]) >> right_move_bits);
        auto ry1 = static_cast<double>((n->slotkey[x1] - n->slotkey[x0]) >> right_move_bits);
        auto ry2 = static_cast<double>((n->slotkey[x2] - n->slotkey[x0]) >> right_move_bits);
        auto ry3 = static_cast<double>((n->slotkey[x3] - n->slotkey[x0]) >> right_move_bits);
        auto ry4 = static_cast<double>((n->slotkey[x4] - n->slotkey[x0]) >> right_move_bits);


        // double ratio = (y4-y0) / x4;

        // double ry0 = 0;
        // double ry1 = (y1-y0)/ratio;
        // double ry2 = (y2-y0)/ratio;
        // double ry3 = (y3-y0)/ratio;
        // double ry4 = (y4-y0)/ratio;

        // 求k
        double k0 = (ry1-ry0) / (x1 - x0);
        double k1 = (ry2-ry1) / (x2 - x1);
        double k2 = (ry3-ry2) / (x3 - x2);
        double k3 = (ry4-ry3) / (x4 - x3);
        double k = (ry4-ry0) / (x4 - x0);

        double b0 = ry0-k0*x0;
        double b1 = ry1-k1*x1;
        double b2 = ry2-k2*x2;
        double b3 = ry3-k3*x3;
        double b = ry0-k*x0;

        double sum = 0;
        sum += compute_integrate(k, k0, b, b0, x0, x1);
        sum += compute_integrate(k, k1, b, b1, x1, x2);
        sum += compute_integrate(k, k2, b, b2, x2, x3);
        sum += compute_integrate(k, k3, b, b3, x3, x4);
        // std::cout << "k = " << k << "\n";

        

        // fitting_liner(n,"sum_" + std::to_string(static_cast<int>(sum/ry4)));

        if(sum < (ry4*8)){
            fitting_liner(n,"liner");
        }else if(sum < (ry4*16)){
            fitting_liner(n,"liner_inferior");
        }else if(sum < (ry4*32)){
            fitting_liner(n,"xinferior2");
        }else if(sum < (ry4*55)){
            fitting_gap_x2(n,"x2");
            // fitting_liner(n,"sum_" + std::to_string(static_cast<int>(sum/ry4)));
        }else if(sum < (ry4*77)){
            fitting_gap_x3(n,"x3");
            // fitting_liner(n,"sum_" + std::to_string(static_cast<int>(sum/ry4)));
        }else{
            // fitting_gap_x6(n,"x6");
            fitting_liner(n,"sum_" + std::to_string(static_cast<int>(sum/ry4)));
        }



        n->insert_count = 0;
        n->delete_count = 0;
        return true;
    }



    // generate duoxiangshi model
    template<typename T>
    bool btree::generate_func_model_10(T* n){
        // sure s1 != 0, define 5 site for compute k.
        if(n->slotuse < 32) {
            n->model_type = modelType::GENERAL;
            return true;
        }

        // f(x)
        // array sites
        int x4 = n->slotuse - 1;
        int x2 = x4 >> 1;
        int x1 = x2 >> 1;
        int x3 = x2 + x1;
        int x0 = 0;

        // 计算4段数据的斜率
        auto y0 = static_cast<double>(n->slotkey[x0]);
        auto y1 = static_cast<double>(n->slotkey[x1]);
        auto y2 = static_cast<double>(n->slotkey[x2]);
        auto y3 = static_cast<double>(n->slotkey[x3]);
        auto y4 = static_cast<double>(n->slotkey[x4]);

        // f'(x) 原来是线性
        double k0 = (y1-y0) / (x1 - x0);
        double k1 = (y2-y1) / (x2 - x1);
        double k2 = (y3-y2) / (x3 - x2);
        double k3 = (y4-y3) / (x4 - x3);
        double left_kk  = k1-k0>0 ? k1/k0-1 : -k0/k1+1;
        double mid_kk   = k2-k1>0 ? k2/k1-1 : -k1/k2+1;
        double right_kk = k3-k2>0 ? k3/k2-1 : -k2/k3+1;

        int x_0 = ((0+x1)>>1 );
        int x_1 = ((x1+x2)>>1);
        int x_2 = ((x2+x3)>>1);
        int x_3 = ((x3+x4)>>1);

        auto y_1 = static_cast<double>(n->slotkey[x_1]);
        auto y_2 = static_cast<double>(n->slotkey[x_2]);

        double k_m = (y_2 - y_1) / (x_2 - x_1);


        // f''(x)  x^2 一个弯儿
        double kk0 = (k1-k0) / (x1 - x0);
        double kk1 = (k2-k1) / (x2 - x1);
        double kk2 = (k3-k2) / (x3 - x2);
        double left_kkk  = fabs(kk1-kk0) - 1;
        double right_kkk = fabs(kk2-kk1) - 1;

        // f'''(x) x^3 两个弯儿
        double kkk0 = (kk1-kk0) / x1;
        double kkk1 = (kk2-kk1) / x1;
        double mid_kkkk = fabs(kkk1-kkk0) - 1;

        // f''''(x) x^4 三个弯儿
        double kkkk0 = (kkk1 - kkk0) / x1;

        double threshold = std::max(std::max(fabs(left_kk),fabs(right_kk)),fabs(mid_kk))/3;
        double r = 1000 + threshold;
        double hr = 1000;

#define T___ fabs(left_kk) + fabs(right_kk) + fabs(mid_kk) <= 1.5
// #define T111 left_kkk > 0 && right_kkk > 0
#define T111 fabs(left_kk-r)<=hr  && fabs(mid_kk-r) <= hr && fabs(right_kk-r)<=hr
#define T110 fabs(left_kk-r)<=hr  && fabs(mid_kk-r) <= hr && fabs(right_kk+r)<=hr
#define T101 fabs(left_kk-r)<=hr  && fabs(mid_kk+r) <= hr && fabs(right_kk-r)<=hr
#define T100 fabs(left_kk-r)<=hr  && fabs(mid_kk+r) <= hr && fabs(right_kk+r)<=hr
#define T011 fabs(left_kk+r)<=hr  && fabs(mid_kk-r) <= hr && fabs(right_kk-r)<=hr
#define T010 fabs(left_kk+r)<=hr  && fabs(mid_kk-r) <= hr && fabs(right_kk+r)<=hr
#define T001 fabs(left_kk+r)<=hr  && fabs(mid_kk+r) <= hr && fabs(right_kk-r)<=hr
#define T000 fabs(left_kk+r)<=hr  && fabs(mid_kk+r) <= hr && fabs(right_kk+r)<=hr
// #define T111 left_kkk < 0 && right_kkk < 0

        if(T___) {
#ifdef MYDEBUG
std::cout << "liner ";
cout_nodeinfo(n);
#endif
            // ax + b model
            // double a1,a2;
            n->model_type = modelType::LINE;
            double a1 = static_cast<double>(x4-x0) / static_cast<double>(y4 - y0);
            n->fa = a1;
            n->fb = 0 - static_cast<double>(n->fa * y0);
            // a2 = static_cast<double>(y4 - y0) / static_cast<double>(x4-x0);
            // if(a2 > 16 && a2 < ((size_t) 1<<50)){
            //     n->reverse = true;
            //     int i=0;
            //     size_t k = get_lt1024(static_cast<size_t>(a2),i);
            //     n->rfa = ((size_t)1 << (i+10) ) / k;
            //     n->rfb = y0;
            //     n->right_move = i + 10;
            // }else{
                // n->reverse = false;
                // n->fa = a1;
                // n->fb = 0 - static_cast<double>(n->fa * y0);

            // }
            if(n->isleafnode())
                data_distribution_count[0]++;
            else
                data_distribution_count_inner[0]++;

        }
        else if(T111) {
#ifdef MYDEBUG
std::cout << "111 ";
cout_nodeinfo(n);
#endif
            // 111 x^2 model 
            n->model_type = modelType::GENERAL;
            n->model_type = modelType::X2;

            n->fa = 0.5 * (k3-k0) / (x_3-x_0);
            n->fb = k0 - 2*n->fa*x_0;
            n->fc = y4 - n->fa*table_x2[x4] - n->fb*x4;

            // n->model_type = modelType::LINE;
            // double a1 = static_cast<double>(x4-x0) / static_cast<double>(y4 - y0);
            // n->fa = a1;
            // n->fb = 0 - static_cast<double>(n->fa * y0);

            // n->fa = 0.5 * (k3-k0) / (n->slotkey[x_3] - n->slotkey[x_0]);
            // n->fb = k0 - 2*n->fa*n->slotkey[x_0];
            // n->fc = x0 - n->fa*y0*y0 - n->fb*y0;


            // double a1,a2;
            // n->model_type = modelType::LINE;
            // a1 = static_cast<double>(x4-x0) / static_cast<double>(y4 - y0);
            // n->fa = a1;
            // n->fb = 0 - static_cast<double>(n->fa * y0);


            // // std::cout << n->fa << "*x^2 + " << n->fb << "*x + " << n->fc << "\n";
            if(n->isleafnode())
                data_distribution_count[1]++;
            else
                data_distribution_count_inner[1]++;
        }
        else if(T000){
#ifdef MYDEBUG
std::cout << "000 ";
cout_nodeinfo(n);
#endif
            // 000 x^2 model
            n->model_type = modelType::GENERAL;
            n->model_type = modelType::X2;


            n->fa = 0.5 * (k3-k0) / (x_3-x_0);
            n->fb = k0 - 2*n->fa*x_0;
            n->fc = y0 - n->fa*table_x2[x0] - n->fb*x0;


            // n->model_type = modelType::LINE;
            // double a1 = static_cast<double>(x4-x0) / static_cast<double>(y4 - y0);
            // n->fa = a1;
            // n->fb = 0 - static_cast<double>(n->fa * y0);

            // n->fa = 0.5 * (k3-k0) / (n->slotkey[x_3] - n->slotkey[x_0]);
            // n->fb = k0 - 2*n->fa*n->slotkey[x_0];
            // n->fc = x4 - n->fa*y4*y4 - n->fb*y4;

            // double a1,a2;
            // n->model_type = modelType::LINE;
            // a1 = static_cast<double>(x4-x0) / static_cast<double>(y4 - y0);
            // n->fa = a1;
            // n->fb = 0 - static_cast<double>(n->fa * y0);

            if(n->isleafnode())
                data_distribution_count[8]++;
            else
                data_distribution_count_inner[8]++;
        }
        else if(T110){
#ifdef MYDEBUG
std::cout << "110 ";
cout_nodeinfo(n);
#endif
            // 110 x+sinx
            // n->model_type = modelType::X3;
            // n->fa = 0.333 * ((k0-k_m)*(x_0-x_3) - (k0 - k3)*(x_0 - x2)) / ((x_0-x2)*(x_0-x_3)*(x2-x_3));
            // n->fb = (k0-k_m - 3*n->fa*(table_x2[x_0] - table_x2[x2])) / (2*(x_0-x2));
            // n->fc = k0 - 3*n->fa*table_x2[x_0] - 2*n->fb*x_0;
            // n->fd = y2 - n->fa*table_x3[x2] - n->fb*table_x2[x2] - n->fc*x2;

            double a1,a2;
            n->model_type = modelType::LINE;
            a1 = static_cast<double>(x4-x0) / static_cast<double>(y4 - y0);
            n->fa = a1;
            n->fb = 0 - static_cast<double>(n->fa * y0);

            if(n->isleafnode())
                data_distribution_count[2]++;
            else
                data_distribution_count_inner[2]++;
            
        }
        else if(T101){
#ifdef MYDEBUG
std::cout << "101 ";
cout_nodeinfo(n);
#endif
            // 101 x+sinx
            n->model_type = modelType::GENERAL;

            double a1,a2;
            n->model_type = modelType::LINE;
            a1 = static_cast<double>(x4-x0) / static_cast<double>(y4 - y0);
            n->fa = a1;
            n->fb = 0 - static_cast<double>(n->fa * y0);

            // cout_nodeinfo(n);
            if(n->isleafnode())
                data_distribution_count[3]++;
            else
                data_distribution_count_inner[3]++;

        }
        else if(T100){
#ifdef MYDEBUG
std::cout << "100 ";
cout_nodeinfo(n);
#endif
            // 100 x+sinx
            double a1,a2;
            n->model_type = modelType::LINE;
            a1 = static_cast<double>(x4-x0) / static_cast<double>(y4 - y0);
            n->fa = a1;
            n->fb = 0 - static_cast<double>(n->fa * y0);


            if(n->isleafnode())
                data_distribution_count[4]++;
            else
                data_distribution_count_inner[4]++;
        }
        else if(T011){
#ifdef MYDEBUG
std::cout << "011 ";
cout_nodeinfo(n);
#endif
            // 011 x+sinx
/*             n->model_type = modelType::GENERAL;
            double maxk = std::max(std::max(k0,k1),std::max(k2,k3));
            double mink = std::min(std::min(k0,k1),std::min(k2,k3));
            double a = (x4-x0) / (y4-y0);
            double c = 6.28 /  (y4-y0);
            double b =  (maxk - mink) / (2*c) * c > a ? a/c : (maxk - mink) / (2*c) * c;
            double d = 0 - (a * y0);
            n->fa = a;
            n->fb = b;
            n->fc = c;
            n->fd = d;
            n->fe = y0;
            // std::cout << "f(x) = " 
            // << a << "*x + " << b << "*sin(" << c << "*(x-" << y0 << ")) + " << d << "\n" ;
            // cout_nodeinfo(n);
            size_t line_err = 0;
            size_t erros = 0;
            // check_fitting(n,line_err,erros);
            // std::cout << "error is " << line_err << " " << erros  << std::endl;
            // // if(erros > 30)
            //     cout_nodeinfo(n); */
            double a1,a2;
            n->model_type = modelType::LINE;
            a1 = static_cast<double>(x4-x0) / static_cast<double>(y4 - y0);
            n->fa = a1;
            n->fb = 0 - static_cast<double>(n->fa * y0);
            if(n->isleafnode())
                data_distribution_count[5]++;
            else
                data_distribution_count_inner[5]++;

        }
        else if(T010){
#ifdef MYDEBUG
std::cout << "010 ";
cout_nodeinfo(n);
#endif
            // 010 x+sinx
            double a1,a2;
            n->model_type = modelType::LINE;
            a1 = static_cast<double>(x4-x0) / static_cast<double>(y4 - y0);
            n->fa = a1;
            n->fb = 0 - static_cast<double>(n->fa * y0);


            // cout_nodeinfo(n);
            if(n->isleafnode())
                data_distribution_count[6]++;
            else
                data_distribution_count_inner[6]++;
        }
        else if(T001){
#ifdef MYDEBUG
std::cout << "001 ";
cout_nodeinfo(n);
#endif
            // 001 x+sinx
            double a1,a2;
            n->model_type = modelType::LINE;
            a1 = static_cast<double>(x4-x0) / static_cast<double>(y4 - y0);
            n->fa = a1;
            n->fb = 0 - static_cast<double>(n->fa * y0);
            if(n->isleafnode())
                data_distribution_count[7]++;
            else
                data_distribution_count_inner[7]++;

        }

        else{
#ifdef MYDEBUG
std::cout << "other ";
cout_nodeinfo(n);
#endif
            // x^n model
            double a1,a2;
            n->model_type = modelType::LINE;
            a1 = static_cast<double>(x4-x0) / static_cast<double>(y4 - y0);
            n->fa = a1;
            n->fb = 0 - static_cast<double>(n->fa * y0);
            // std::cout << "is general node" << "\n";
            // cout_nodeinfo(n);
            // std::cout << x_0 << " ";
            // std::cout << x_1 << " ";
            // std::cout << x_2 << " ";
            // std::cout << x_3 << "\n";

            // std::vector<std::vector<double>> input =  {
            //     {table_x3[x_0],table_x2[x_0],x_0,1},
            //     {table_x3[x_1],table_x2[x_1],x_1,1},
            //     {table_x3[x_2],table_x2[x_2],x_2,1},
            //     {table_x3[x_3],table_x2[x_3],x_3,1}
            // };
            
            // std::vector<std::vector<double>> r = get_inverse(input);
            // double a = r[0][0] * k0 + r[0][1] * k1 + r[0][2] * k2 + r[0][3] * k3;
            // double b = r[1][0] * k0 + r[1][1] * k1 + r[1][2] * k2 + r[1][3] * k3;
            // double c = r[2][0] * k0 + r[2][1] * k1 + r[2][2] * k2 + r[2][3] * k3;
            // double d = r[3][0] * k0 + r[3][1] * k1 + r[3][2] * k2 + r[3][3] * k3;
            // n->fa = a/4;
            // n->fb = b/3;
            // n->fc = c/2;
            // n->fd = d;
            // double e1 = y0 - (a*table_x4[x0] + b*table_x3[x0] + c*table_x2[x0] +d*x0) ;
            // double e2 = y2 - (a*table_x4[x2] + b*table_x3[x2] + c*table_x2[x2] +d*x2) ;
            // double e3 = y4 - (a*table_x4[x4] + b*table_x3[x4] + c*table_x2[x4] +d*x4) ;
            // n->fe = (e1+e2+e3)/3;

            // size_t line_err=0,errs=0;
            // check_fitting_x4(n, line_err, errs);
            // std::cout << line_err << " " << errs << std::endl;
            // cout_nodeinfo(n);
            if(n->isleafnode())
                data_distribution_count[9]++;
            else
                data_distribution_count_inner[9]++;
        }

        n->insert_count = 0;
        n->delete_count = 0;
        return true;
    }



//     // generate duoxiangshi model
//     template<typename T>
//     bool generate_func_model_27(T* n){
//         // sure s1 != 0, define 5 site for compute k.
//         if(n->slotuse < 32) {
//             n->model_type = modelType::GENERAL;
//             return true;
//         }

//         // array sites
//         int x4 = n->slotuse - 1;
//         int x2 = x4 >> 1;
//         int x1 = x2 >> 1;
//         int x3 = x2 + x1;
//         int x0 = 0;

//         // 计算4段数据的斜率
//         auto y0 = static_cast<double>(n->slotkey[x0]);
//         auto y1 = static_cast<double>(n->slotkey[x1]);
//         auto y2 = static_cast<double>(n->slotkey[x2]);
//         auto y3 = static_cast<double>(n->slotkey[x3]);
//         auto y4 = static_cast<double>(n->slotkey[x4]);

//         // 求k
//         double k0 = (y1-y0) / (x1 - x0);
//         double k1 = (y2-y1) / (x2 - x1);
//         double k2 = (y3-y2) / (x3 - x2);
//         double k3 = (y4-y3) / (x4 - x3);
//         double k = (y4-y0) / (x4 - x0);

//         // 求k的大小关系，作为判断类型的依据
//         double left_kk  = k1-k0>0 ? k1/k0-1 : -k0/k1+1;
//         double mid_kk   = k2-k1>0 ? k2/k1-1 : -k1/k2+1;
//         double right_kk = k3-k2>0 ? k3/k2-1 : -k2/k3+1;
//         double line_threshold = 0.5;
//         double turn_threshold = 0.3;

// #define TLLL fabs(left_kk) <= line_threshold && fabs(right_kk) <= line_threshold && fabs(mid_kk) <= line_threshold

//         if(TLLL){
//             // liner
//             fitting_liner(n,"liner_000");
//         }else{
//             double threshold = std::max(std::max(fabs(left_kk),fabs(right_kk)),std::max(fabs(mid_kk),turn_threshold))/8   ;

//             if(left_kk > threshold){
//                 if(mid_kk > threshold){
//                     if(right_kk > threshold){
//                         // 凹凹凹 = 凹
//                         fitting_x2(n,"x2_aaa");
//                     }else if(right_kk < -turn_threshold){
//                         // 凹凹凸 = S
//                         fitting_x3r(n,"x3r_aat");
//                     }else{
//                         // 凹凹直 = 凹
//                         fitting_liner(n,"x2_aaz");
//                     }
//                 }else if(mid_kk < -turn_threshold){
//                     if(right_kk > turn_threshold){
//                         // 凹凸凹 = W
//                         fitting_x4(n,"x4_ata");
//                     }else if(right_kk < -threshold){
//                         // 凹凸凸 = S
//                         fitting_x3r(n,"x3r_att");
//                     }else{
//                         // 凹凸直 = S
//                         fitting_x3r(n,"x3r_atz");
//                     }
//                 }else{
//                     if(right_kk > threshold){
//                         // 凹直凹 = 凹
//                         fitting_liner(n,"x2_aza");
//                     }else if(right_kk < -turn_threshold){
//                         // 凹直凸 = S
//                         fitting_x3r(n,"x3r_azt");
//                     }else{
//                         // 凹直直 = 凹
//                         fitting_liner(n,"x2_azz");
//                     }
//                 }
//             }else if(left_kk < -threshold){
//                 if(mid_kk > turn_threshold){
//                     if(right_kk > threshold){
//                         // 凸凹凹 = S
//                         fitting_x3(n,"x3_taa");
//                     }else if(right_kk < -turn_threshold){
//                         // 凸凹凸 = M
//                         fitting_x4r(n,"x4r_tat");
//                     }else{
//                         // 凸凹直 = S
//                         fitting_x3(n,"x3_taz");
//                     }
//                 }else if(mid_kk < -threshold){
//                     if(right_kk > turn_threshold){
//                         // 凸凸凹 = S
//                         fitting_x3(n,"x3_tta");
//                     }else if(right_kk < -threshold){
//                         // 凸凸凸 = 凸
//                         fitting_x2(n,"x2r_ttt");
//                     }else{
//                         // 凸凸直 = 凸
//                         fitting_liner(n,"x2r_ttz");
//                     }
//                 }else{
//                     if(right_kk > turn_threshold){
//                         // 凸直凹 = S
//                         fitting_x3(n,"x3_tza");
//                     }else if(right_kk < -threshold){
//                         // 凸直凸 = 凸
//                         fitting_liner(n,"x2r_tzt");
//                     }else{
//                         // 凸直直 = 凸
//                         fitting_liner(n,"x2r_tzz");
//                     }
//                 }
//             }else{
//                 if(mid_kk > threshold){
//                     if(right_kk > threshold){
//                         // 直凹凹 = 凹
//                         fitting_liner(n,"x2_zaa");
//                     }else if(right_kk < -turn_threshold){
//                         // 直凹凸 = S
//                         fitting_x3r(n,"x3r_zat");
//                     }else{
//                         // 直凹直 = 凹
//                         fitting_liner(n,"x2_zaz");
//                     }
//                 }else if(mid_kk < -threshold){
//                     if(right_kk > turn_threshold){
//                         // 直凸凹 = S
//                         fitting_x3(n,"x3_zta");
//                     }else if(right_kk < -threshold){
//                         // 直凸凸 = 凸
//                         fitting_liner(n,"x2r_ztt");
//                     }else{
//                         // 直凸直 = 凸
//                         fitting_liner(n,"x2r_ztz");
//                     }
//                 }else{
//                     if(right_kk > threshold){
//                         // 直直凹
//                         fitting_liner(n,"x2_zza");
//                     }else if(right_kk < -threshold){
//                         // 直直凸
//                         fitting_liner(n,"x2r_zzt");
//                     }else{
//                         // 直直直
//                         fitting_other(n,"other_zzz");
//                         std::cout << "why can happen this liner liner liner\n";
//                     }
//                 }
//             }
//         }

//         n->insert_count = 0;
//         n->delete_count = 0;
//         return true;
//     }

}
