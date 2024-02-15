// #pragma once

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


#include "stx/btree_multimap.h"
#include "alex/alex.h"
// #include "lipp.h"


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
typedef alex::Alex<KEY_TYPE, PAYLOAD_TYPE> alex_type;
// extern size_t level_delay[4];



auto test_btree(std::vector<std::pair<size_t,size_t>> data, 
    int insert_count, int erase_count, int find_count, int times) -> bool{
    btree_type bt;
    bt.bulk_load(data.begin(),data.end());
    // for(auto e:bt){
    //     std::cout << e.first << "\t" << e.second << std::endl;
    // }
    // auto res = bt.find(718186352);
    // std::cout << res->first << " " << res->second << "\n";
    size_t states[2] = {0};
    for(int i=0;i<times;i++){


        for(int j=0;j<5;j++){
            stx::gaps[j] = 0;
            stx::gaps_count[j] = 0;
            stx::level_delay[j] = 0;
        }

        uint64_t insert_times = 0;
        uint64_t erase_times = 0;
        uint64_t find_times = 0;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> intDistribution(1,data.size());

        // for(int j = 0; j < insert_count; j++){
        //     int randomInt = intDistribution(gen) % data.size();
        //     auto currentTime1 = std::chrono::high_resolution_clock::now();
        //     bt.insert(data[randomInt].first,data[randomInt].second);
        //     auto currentTime2 = std::chrono::high_resolution_clock::now();
        //     auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
        //     auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
        //     insert_times += (nanoseconds2 - nanoseconds1);
        // }

        for(int j = 0; j < erase_count; j++){
            int randomInt = intDistribution(gen) % data.size();
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            bt.erase(data[randomInt].first);
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            erase_times += (nanoseconds2 - nanoseconds1);
        }

        for(int j = 0; j < find_count; j++){
            int randomInt = intDistribution(gen) % data.size();
            auto ttt =  data[randomInt].first;
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            auto res = bt.find(data[randomInt].first);
            // auto res = bt.find(ttt);

            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            find_times += (nanoseconds2 - nanoseconds1);
            if(res != bt.end() && data[randomInt].first != res->second){
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
        << " insert用时-纳秒:" << insert_times / insert_count 
        << " erase用时-纳秒:" << erase_times / erase_count 
        << " find用时-纳秒:" << find_times / find_count 
        << " 数据量-百万:" << bt.size()/1000000 
        // << " level0:" << stx::level_delay[0] / find_count  
        // << " level1:" << stx::level_delay[1] / find_count  
        // << " level2:" << stx::level_delay[2] / find_count  
        // << " level3:" << stx::level_delay[3] / find_count  
        // << " level4:" << stx::level_delay[4] / find_count
        // << " gap0:" << stx::gaps[0] / (stx::gaps_count[0]+1   )
        // << " gap1:" << stx::gaps[1] / (stx::gaps_count[1]+1   )
        // << " gap2:" << stx::gaps[2] / (stx::gaps_count[2]+1   )
        // << " gap3:" << stx::gaps[3] / (stx::gaps_count[3]+1   )
        // << " gap4:" << stx::gaps[4] / (stx::gaps_count[4]+1   )
        <<  "\n";
    }

    return true;
    
}


auto print_sts(size_t sum[5], size_t count[5]){
    for(int i=0;i<5;i++){
        std::cout << sum[i] / (count[i]+1) << " ";
    }
    std::cout << "|";
}

static size_t tmp_num = 0;

auto test_btree_x(std::vector<std::pair<size_t,size_t>> data,
    int insert_count, int erase_count, int find_count, int times) -> bool{

    btree_type bt;
    bt.bulk_load_x(data.begin(),data.end());
    std::cout << "node_type_counts " 
    << " " << stx::node_type_counts[0]  
    << " " << stx::node_type_counts[1]  
    << " " << stx::node_type_counts[2]  
    << " " << stx::node_type_counts[3]  
    << " " << stx::node_type_counts[4]
    << " " << stx::node_type_counts[5]
    << " " << stx::node_type_counts[6]
    << "\n";

    size_t states[2] = {0};
    for(int i=0;i<times;i++){
        for(int j=0;j<5;j++){
            stx::gaps[j] = 1;
            stx::gaps_count[j] = 1;
            stx::level_delay[j] = 1;
            stx::level_delay_count[j] = 1;
            stx::mul_times[j] = 1;
            stx::mul_counts[j] = 1;
            stx::load_times[j] = 1;
            stx::load_counts[j] = 1;
        }

        uint64_t insert_times = 0;
        uint64_t erase_times = 0;
        uint64_t find_times = 0;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> intDistribution(1,data.size());

        // for(int j = 0; j < insert_count; j++){
        //     int randomInt = intDistribution(gen) % data.size();
        //     auto currentTime1 = std::chrono::high_resolution_clock::now();
        //     bt.insert_x(data[randomInt].first,data[randomInt].first);
        //     auto currentTime2 = std::chrono::high_resolution_clock::now();
        //     auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
        //     auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
        //     insert_times += (nanoseconds2 - nanoseconds1);
        // }

        for(int j = 0; j < erase_count; j++){
            int randomInt = intDistribution(gen) % data.size();
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            bt.erase_x(data[randomInt].first);
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            erase_times += (nanoseconds2 - nanoseconds1);
        }

        for(int j = 0; j < find_count; j++){
            int randomInt = intDistribution(gen) % data.size();
            size_t ttt = data[randomInt].first;
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            // auto res = bt.find_x(ttt);
            auto res = bt.find_x(data[randomInt].first);
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();

            if(res){
                // std::cout << data[randomInt].first << std::endl;
                if(data[randomInt].first != *res) 
                    std::cout << data[randomInt].first <<" "<< *res << std::endl;
            }

            find_times += (nanoseconds2 - nanoseconds1);
        }


        std::cout 
        << __func__ 
        << " insert用时-纳秒:" << insert_times / insert_count 
        << " erase用时-纳秒:" << erase_times / erase_count 
        << " find用时-纳秒:" << find_times / find_count 
        << " 数据量-百万:" << bt.size()/1000000 << " |" ;
        print_sts(stx::gaps, stx::gaps_count);
        print_sts(stx::mul_times, stx::mul_counts);
        print_sts(stx::load_times, stx::load_counts);
        std::cout <<  "\n";
    }
    return true;
    
}

auto test_alex(std::vector<std::pair<size_t,size_t>> data, 
    int insert_count, int erase_count, int find_count, int times) -> bool{
    alex_type index;
    index.bulk_load(data.data(), data.size());
    size_t states[2] = {0};
    for(int i=0;i<times;i++){

        for(int j=0;j<5;j++){
            stx::gaps[j] = 0;
            stx::gaps_count[j] = 0;
            stx::level_delay[j] = 0;
        }

        uint64_t insert_times = 0;
        uint64_t erase_times = 0;
        uint64_t find_times = 0;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> intDistribution(1,data.size());


        // for(int j = 0; j < insert_count; j++){
        //     int randomInt = intDistribution(gen)  % data.size();
        //     auto currentTime1 = std::chrono::high_resolution_clock::now();
        //     index.insert(data[randomInt].first,data[randomInt].second);
        //     auto currentTime2 = std::chrono::high_resolution_clock::now();
        //     auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
        //     auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
        //     insert_times += (nanoseconds2 - nanoseconds1);
        // }


        for(int j = 0; j < erase_count; j++){
            int randomInt = intDistribution(gen)  % data.size();
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            index.erase(data[randomInt].first);
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            erase_times += (nanoseconds2 - nanoseconds1);
        }


        for(int j = 0; j < find_count; j++){
            int randomInt = intDistribution(gen)  % data.size();
            size_t ttt = data[randomInt].first;
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            // auto res = index.get_payload(ttt);
            auto res = index.get_payload(data[randomInt].first);
            // auto resd = *res;
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            find_times += (nanoseconds2 - nanoseconds1);
            if(res){
                // std::cout << data[randomInt].first << std::endl;
                if(data[randomInt].first != *res) std::cout << data[randomInt].first <<" "<< *res << std::endl;
            }
        }


        std::cout 
        << __func__ 
        << " insert用时-纳秒:" << insert_times / insert_count 
        << " erase用时-纳秒:" << erase_times / erase_count 
        << " find用时-纳秒:" << find_times / find_count 
        << " 数据量-百万:" << index.size()/1000000 
        << " level0:" << stx::level_delay[0] / find_count  
        << " level1:" << stx::level_delay[1] / find_count  
        << " level2:" << stx::level_delay[2] / find_count  
        << " level3:" << stx::level_delay[3] / find_count  
        << " level4:" << stx::level_delay[4] / find_count  
        << " gap0:" << stx::gaps[0] / (stx::gaps_count[0]+1   )
        << " gap1:" << stx::gaps[1] / (stx::gaps_count[1]+1   )
        << " gap2:" << stx::gaps[2] / (stx::gaps_count[2]+1   )
        << " gap3:" << stx::gaps[3] / (stx::gaps_count[3]+1   )
        << " gap4:" << stx::gaps[4] / (stx::gaps_count[4]+1   )
        <<  "\n";
    }
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
    for(size_t i=0;i<data.size();i++){
        data[i].first = keys[i];
        data[i].second = keys[i];
    }

    test_btree_x(data, 20000, 20000, 200000, 10);
    test_alex(data, 20000, 20000, 200000, 10);
    test_btree(data, 20000, 20000, 200000, 10);



    return 0;

}








