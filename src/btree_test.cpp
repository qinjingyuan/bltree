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
#include "stx/btree.h"
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


#include "stx_pure/btree_multimap.h"


#define KEY_TYPE size_t
#define PAYLOAD_TYPE size_t


template <typename KeyType>
struct traits_nodebug : stx::btree_default_set_traits<KeyType> {
    static const bool selfverify = false;
    static const bool debug = false;

    static const int  leafslots = 256;
    static const int  innerslots = 256;
};

typedef stx::btree_multimap<size_t, size_t, std::less<size_t>, traits_nodebug<size_t> > btree_type;
// extern size_t level_delay[4];




auto test_btree(std::vector<std::pair<size_t,size_t>> data,int count,int times,std::string workload) -> bool{
    btree_type bt;
    bt.bulk_load(data.begin(),data.end());
    // for(auto e:bt){
    //     std::cout << e.first << "\t" << e.second << std::endl;
    // }
    auto res = bt.find(718186352);
    std::cout << res->first << " " << res->second << "\n";
    size_t states[2] = {0};
    for(int i=0;i<times;i++){


        for(int i=0;i<5;i++){
            stx::gaps[i] = 0;
            stx::gaps_count[i] = 0;
            stx::level_delay[i] = 0;
            stx::level_delay_count[i] = 1;
        }


        unsigned long using_times1 = 0;
        unsigned long using_times2 = 0;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> intDistribution(1,data.size());
        // for(int j=0;j<count;j++){
        //     int randomInt = intDistribution(gen);
        //     auto currentTime1 = std::chrono::high_resolution_clock::now();
        //     bt.insert(data[randomInt].first,data[randomInt].second);
        //     auto currentTime2 = std::chrono::high_resolution_clock::now();
        //     auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
        //     auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
        //     using_times1 += (nanoseconds2 - nanoseconds1);
        // }

        for(int j=0;j<count;j++){
            int randomInt = intDistribution(gen);
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            auto res = bt.find(data[randomInt].first);

            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            using_times2 += (nanoseconds2 - nanoseconds1);
            if(res == bt.end() && data[randomInt].first != 0){
                std::cout 
                << data[randomInt].first  <<"\t"
                << data[randomInt].second <<"\t"
                << res->first << "\t" 
                << res->second << "\t"
                << std::endl;
            }
        }


        std::cout 
        << __func__ 
        << " insert用时-纳秒:" << using_times1 / count 
        << " find用时-纳秒:" << using_times2 / count 
        << " 数据量-百万:" << bt.size()/1000000 
        << " level0:" << stx::level_delay[0] / stx::level_delay_count[0]  
        << " level1:" << stx::level_delay[1] / stx::level_delay_count[1]  
        << " level2:" << stx::level_delay[2] / stx::level_delay_count[2]  
        << " level3:" << stx::level_delay[3] / stx::level_delay_count[3]  
        << " level4:" << stx::level_delay[4] / stx::level_delay_count[4]  
        << " gap0:" << stx::gaps[0] / (stx::gaps_count[0]+1   )
        << " gap1:" << stx::gaps[1] / (stx::gaps_count[1]+1   )
        << " gap2:" << stx::gaps[2] / (stx::gaps_count[2]+1   )
        << " gap3:" << stx::gaps[3] / (stx::gaps_count[3]+1   )
        << " gap4:" << stx::gaps[4] / (stx::gaps_count[4]+1   )
        <<  "\n";
        states[0] += using_times2;
    }
    std::cout << "avg_all_times " << __func__ << " " << workload <<  " " << states[0] / (times*count) << "\n";

    return true;
    
}


int main(int argc, char** argv){

  auto flags = parse_flags(argc, argv);
  std::string keys_file_path = get_required(flags, "keys_file");
//   std::string keys_file_type = get_required(flags, "keys_file_type");
  auto init_num_keys = stoi(get_required(flags, "init_num_keys"));
  auto total_num_keys = stoi(get_required(flags, "total_num_keys"));
//   auto batch_size = stoi(get_required(flags, "batch_size"));

  std::vector<size_t> keys(init_num_keys,0);
  load_binary_data(keys.data(),init_num_keys,keys_file_path);

    std::random_device rd;
    std::mt19937 gen(rd()); // 使用Mersenne Twister引擎
    // 生成随机整数
    std::uniform_int_distribution<int> intDistribution(1, init_num_keys); // 生成1到100之间的随机整数
    for(int i=0;i<total_num_keys - init_num_keys;i++){
        int randomInt = intDistribution(gen);
        keys.push_back(keys[randomInt]);
        // std::cout << "随机整数: " << randomInt << std::endl;

    }

    std::sort(keys.begin(),keys.end());
    std::vector<std::pair<size_t,size_t>> data(keys.size());
    for(int i=0;i<data.size();i++){
        data[i].first = keys[i];
        data[i].second = keys[i];
    }
    // std::cout << data[0].first << " " << data[0].second << std::endl;

    //   for(int i=0;i<keys.size()-1;i++){
    //     std::cout << keys[i+1] - keys[i] << " " << keys[i] << std::endl;
    //   }


    test_btree(data, 5000000, 5,keys_file_path);
    // test_btree_l(data, 2000000, 10);

    // int base = 1000000;
    // test_mydata_btree_x_instance    (base * 200,base * 2, 10);
    // test_mydata_alex_instance     (base * 200,base * 2, 20);
    // test_mydata_btree_instance    (base * 200,base * 2, 10);
    // test_mydata_btree_opt_instance  (base * 200,base * 2, 10);
    // test_mydata_lipp_instance(base * 2000,base * 2, 20);
    // test_mydata_dytis_instance(base * 2000,base * 2, 20);




    return 0;

}








