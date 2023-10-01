#pragma once

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <vector>
#include <thread>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include "util/pair.h"
#include <string>
#include <sstream>
#include <stdlib.h>
#include <numeric>

#include "flags.h"
#include "utils.h"

// #include "src/DyTIS.h"
// #include "src/DyTIS_impl.h"

#include <vector>
#include <chrono>
#include <iostream>


#include "stx/btree_multimap.h"
#include "alex/alex.h"
#include "lipp.h"


#define KEY_TYPE size_t
#define PAYLOAD_TYPE size_t


template <typename KeyType>
struct traits_nodebug : stx::btree_default_set_traits<KeyType> {
    static const bool selfverify = false;
    static const bool debug = false;

    static const int  leafslots = 256;
    static const int  innerslots = 256;
};

// extern size_t level_delay[4];

void test_mydata_btree_instance(size_t numkeys, int statistic_nums, int statistic_nums_times){
    int find_count = statistic_nums;
    typedef stx::btree_multimap<size_t, size_t, std::less<unsigned int>, traits_nodebug<unsigned int> > btree_type;
    std::vector<std::pair<size_t, size_t> > pairs(find_count);

    srand(342342350);
    for (int i = 0; i < find_count; i++)
    {
        size_t insert_num =  rand() % numkeys;
        pairs[i].first = insert_num;
        pairs[i].second = insert_num;
    }

    std::sort(pairs.begin(), pairs.end());
    btree_type bt;
    
    bt.bulk_load(pairs.begin(), pairs.end());

    srand(342342350);
    for(int i=0;i<statistic_nums_times;i++){
        unsigned long using_times1 = 0;
        unsigned long using_times2 = 0;
        for (int i = 0; i < find_count; i++)
        {
            size_t tmp = (rand() % numkeys);
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            bt.insert(tmp,tmp);
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            // if(tmp != res->second) std::cout << tmp <<" "<< res->second << std::endl;
            using_times1 +=  (nanoseconds2 - nanoseconds1);
        }

        for (int i = 0; i < find_count; i++)
        {
            size_t tmp = (rand() % numkeys);
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            auto res = bt.find(tmp);
            if(res != bt.end()){
                if(tmp != res->second) std::cout << tmp <<" "<< res->second << std::endl;
            }
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            using_times2 +=  (nanoseconds2 - nanoseconds1);
        }


        std::cout 
        << __func__ 
        << " insert用时-纳秒:" << using_times1 / find_count 
        << " find用时-纳秒:" << using_times2 / find_count 
        << " 数据量-百万:" << bt.size()/1000000 
        <<  "\n";
    }

}


void test_mydata_btree_opt_instance(size_t numkeys, int statistic_nums, int statistic_nums_times){
    int find_count = statistic_nums;
    typedef stx::btree_multimap<size_t, size_t, std::less<unsigned int>, traits_nodebug<unsigned int> > btree_type;
    std::vector<std::pair<size_t, size_t> > pairs(find_count);

    srand(342342350);
    for (int i = 0; i < find_count; i++)
    {
        size_t insert_num =  rand() % numkeys;
        pairs[i].first = insert_num;
        pairs[i].second = insert_num;
    }

    std::sort(pairs.begin(), pairs.end());
    btree_type bt;
    
    bt.bulk_load(pairs.begin(), pairs.end());

    srand(342342350);
    for(int i=0;i<statistic_nums_times;i++){

        stx::level_delay[0] = 0;
        stx::level_delay[1] = 0;
        stx::level_delay[2] = 0;
        stx::level_delay[3] = 0;
        stx::level_delay[4] = 0;

        unsigned long using_times1 = 0;
        unsigned long using_times2 = 0;
        for (int i = 0; i < find_count; i++)
        {
            size_t tmp = (rand() % numkeys);
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            bt.insert(tmp,tmp);
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            // if(tmp != res->second) std::cout << tmp <<" "<< res->second << std::endl;
            using_times1 +=  (nanoseconds2 - nanoseconds1);
        }

        for (int i = 0; i < find_count; i++)
        {
            size_t tmp = (rand() % numkeys);
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            auto res = bt.find_line(tmp);
            if(res != bt.end()){
                if(tmp != res->second) std::cout << tmp <<" "<< res->second << std::endl;
            }
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            using_times2 +=  (nanoseconds2 - nanoseconds1);
        }


        std::cout 
        << __func__ 
        << " insert用时-纳秒:" << using_times1 / find_count 
        << " find用时-纳秒:" << using_times2 / find_count 
        // << " L0用时-纳秒:" << stx::level_delay[0] / find_count 
        // << " L1用时-纳秒:" << stx::level_delay[1] / find_count 
        // << " L2用时-纳秒:" << stx::level_delay[2] / find_count 
        // << " L3用时-纳秒:" << stx::level_delay[3] / stx::level_delay[4] 
        << " 数据量-百万:" << bt.size()/1000000 
        <<  "\n";
    }

}


void test_mydata_alex_instance(size_t numkeys, int statistic_nums, int statistic_nums_times){
    int find_count = statistic_nums;
    alex::Alex<KEY_TYPE, PAYLOAD_TYPE> bt;
    std::vector<std::pair<size_t, size_t> > pairs(find_count);

    srand(342342350);
    for (int i = 0; i < find_count; i++)
    {
        size_t insert_num =  rand() % numkeys;
        pairs[i].first = insert_num;
        pairs[i].second = insert_num;
    }

    std::sort(pairs.begin(), pairs.end());
    
    bt.bulk_load(pairs.data(), pairs.size());

    srand(342342350);

    for(int i=0;i<statistic_nums_times;i++){
        unsigned long using_times1 = 0; 
        unsigned long using_times2 = 0; 
        for (int i = 0; i < find_count; i++)
        {
            size_t tmp = (rand() % numkeys);
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            bt.insert(tmp,tmp);
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            // if(tmp != *res) std::cout << tmp <<" "<< *res << std::endl;
            using_times1 +=  (nanoseconds2 - nanoseconds1);
        }




        for (int i = 0; i < find_count; i++)
        {
            size_t tmp = (rand() % numkeys);
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            auto res = bt.get_payload(tmp);
            if(res){
                if(tmp != *res) std::cout << tmp <<" "<< *res << std::endl;
            }
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            using_times2 +=  (nanoseconds2 - nanoseconds1);
        }


        std::cout 
        << __func__ 
        << " insert用时-纳秒:" << using_times1 / find_count 
        << " find用时-纳秒:" << using_times2 / find_count 
        << " 数据量-百万:" << bt.size()/1000000 
        <<  "\n";

    }

}



void test_mydata_lipp_instance(size_t numkeys, int statistic_nums, int statistic_nums_times){
    int find_count = statistic_nums;

    // alex::Alex<KEY_TYPE, PAYLOAD_TYPE> bt;
    LIPP<size_t, size_t> bt;
    std::vector<std::pair<size_t, size_t> > pairs(find_count);

    srand(342342350);
    for (int i = 0; i < find_count; i++)
    {
        size_t insert_num =  rand() % numkeys;
        pairs[i].first = insert_num;
        pairs[i].second = insert_num;
        if(!bt.exists(insert_num)){
            bt.insert(insert_num,insert_num);
        }
    }

    // std::sort(pairs.begin(), pairs.end());
    
    // bt.bulk_load(pairs.data(), pairs.size());

    srand(342342350);

    for(int i=0;i<statistic_nums_times;i++){
        unsigned long using_times1 = 0; 
        unsigned long using_times2 = 0; 
        for (int i = 0; i < find_count; i++)
        {
            size_t tmp = (rand() % numkeys);
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            if(!bt.exists(tmp)){
                bt.insert(tmp,tmp);
            }
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            // if(tmp != *res) std::cout << tmp <<" "<< *res << std::endl;
            using_times1 +=  (nanoseconds2 - nanoseconds1);
        }




        for (int i = 0; i < find_count; i++)
        {
            size_t tmp = (rand() % numkeys);
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            auto res = bt.at(tmp);
            // if(res){
            //     if(tmp != *res) std::cout << tmp <<" "<< *res << std::endl;
            // }
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            using_times2 +=  (nanoseconds2 - nanoseconds1);
        }


        std::cout 
        << __func__ 
        << " insert用时-纳秒:" << using_times1 / find_count 
        << " find用时-纳秒:" << using_times2 / find_count 
        << " 数据量-百万:" << bt.index_size()/1000000 
        <<  "\n";

    }

}



// void test_mydata_dytis_instance(size_t numkeys, int statistic_nums, int statistic_nums_times){
//     using namespace std;
//     int find_count = statistic_nums;

//     // alex::Alex<KEY_TYPE, PAYLOAD_TYPE> bt;
//     // LIPP<size_t, size_t> bt;
//     DyTIS* index = new DyTIS();
//     std::vector<std::pair<size_t, size_t> > pairs(find_count);

//     srand(342342350);
//     for (int i = 0; i < find_count; i++)
//     {
//         size_t insert_num =  rand() % numkeys;
//         pairs[i].first = insert_num;
//         pairs[i].second = insert_num;
//         index->Insert(insert_num,insert_num);

//     }

//     // std::sort(pairs.begin(), pairs.end());
    
//     // bt.bulk_load(pairs.data(), pairs.size());

//     srand(342342350);

//     for(int i=0;i<statistic_nums_times;i++){
//         unsigned long using_times1 = 0; 
//         unsigned long using_times2 = 0; 
//         for (int i = 0; i < find_count; i++)
//         {
//             size_t tmp = (rand() % numkeys);
//             auto currentTime1 = std::chrono::high_resolution_clock::now();
//                 index->Insert(tmp,tmp);
//             auto currentTime2 = std::chrono::high_resolution_clock::now();
//             auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
//             auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
//             // if(tmp != *res) std::cout << tmp <<" "<< *res << std::endl;
//             using_times1 +=  (nanoseconds2 - nanoseconds1);
//         }




//         for (int i = 0; i < find_count; i++)
//         {
//             size_t tmp = (rand() % numkeys);
//             auto currentTime1 = std::chrono::high_resolution_clock::now();
//             auto res = index->Get(tmp);
//             // if(res){
//             //     if(tmp != *res) std::cout << tmp <<" "<< *res << std::endl;
//             // }
//             auto currentTime2 = std::chrono::high_resolution_clock::now();
//             auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
//             auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
//             using_times2 +=  (nanoseconds2 - nanoseconds1);
//         }


//         std::cout 
//         << __func__ 
//         << " insert用时-纳秒:" << using_times1 / find_count 
//         << " find用时-纳秒:" << using_times2 / find_count 
//         << " 数据量-百万:" << find_count*i+statistic_nums/1000000 
//         <<  "\n";

//     }

// }





int main(){
    int base = 1000000;
    // test_mydata_dytis_instance(base * 200,base * 2, 20);
    test_mydata_lipp_instance(base * 200,base * 2, 20);
    test_mydata_btree_opt_instance(base * 200,base * 2, 20);
    test_mydata_alex_instance     (base * 200,base * 2, 20);
    // test_mydata_btree_instance    (base * 200,base * 2, 20);
    return 0;

}








