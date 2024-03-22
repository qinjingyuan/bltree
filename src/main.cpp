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

    static const int  leafslots = 512;
    static const int  innerslots = 512;
};

typedef stx::btree_multimap<size_t, size_t, std::less<size_t>, traits_nodebug<size_t> > btree_type;
typedef alex::Alex<KEY_TYPE, PAYLOAD_TYPE> alex_type;
// extern size_t level_delay[4]; 
// btree alex btree_x
size_t stat_res[3][3]; 

auto test_btree(std::vector<std::pair<size_t,size_t>> data, 
    int insert_count, int erase_count, int find_count, int times) -> bool{
    btree_type bt;
    auto t1 = std::chrono::high_resolution_clock::now();
    bt.bulk_load(data.begin(),data.end());
    // for(auto e : data){
    //     std::cout << e.first << " " << e.second << "\n";
    // }
    std::cout << "data.begin() = " << (*bt.begin()).first << "\n";
    std::cout << "data.end() = " << (*(bt.end()--)).first << "\n";
    auto t2 = std::chrono::high_resolution_clock::now();
    auto st1 = std::chrono::duration_cast<std::chrono::nanoseconds>(t1.time_since_epoch()).count();
    auto st2 = std::chrono::duration_cast<std::chrono::nanoseconds>(t2.time_since_epoch()).count();
    auto bulk_load_time = (st2 - st1);
    std::cout << "bulk_load_time = " << bulk_load_time / 1000000 << "\n";
    // for(auto e:bt){
    //     std::cout << e.first << "\t" << e.second << std::endl;
    // }
    // auto res = bt.find(718186352);
    // std::cout << res->first << " " << res->second << "\n";
    
    // bt.find(123);
    size_t inum = 1069446001;
    
    size_t states[2] = {0};
    for(int i=0;i<times;i++){
        int right_count = 0, error_count = 0;

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

        for(int j = 0; j < insert_count; j++){
            int randomInt = intDistribution(gen) % data.size();
            auto key = data[randomInt].first;
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            // bt.insert(inum++ ,inum++);
            bt.insert(key ,key);
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            insert_times += (nanoseconds2 - nanoseconds1);
        }

        for(int j = 0; j < erase_count; j++){
            int randomInt = intDistribution(gen) % data.size();
            auto key = data[randomInt].first;
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            bt.erase(key);
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            erase_times += (nanoseconds2 - nanoseconds1);
        }
        
        for(int j = 0; j < find_count; j++){
            int randomInt = intDistribution(gen) % data.size();
            auto ttt =  data[randomInt].first;
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            // auto res = bt.find(data[randomInt].first);
            auto res = bt.find(ttt);

            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            find_times += (nanoseconds2 - nanoseconds1);

            if(res == bt.end() || data[randomInt].first != res->first){
                // std::cout 
                // << data[randomInt].first  <<"\t"
                // << data[randomInt].second <<"\t"
                // << res->first << "\t" 
                // << res->second << "\t"
                // << std::endl;
                error_count++;
            }else{
                right_count++;
            }
        }

        stat_res[0][0] += insert_times / insert_count;
        stat_res[0][1] += erase_times / erase_count;
        stat_res[0][2] += find_times / find_count;
#ifdef RES

        std::cout << "right_count = " << right_count << ", error_count = " << error_count << " "
        << __func__ 
        << " insert用时-纳秒:" << insert_times / insert_count 
        << " erase用时-纳秒:" << erase_times / erase_count 
        << " find用时-纳秒:" << find_times / find_count 
        << " 数据量-万:" << bt.size()/10000 
        <<  "\n";
#endif
    }

    

    stat_res[0][0] /= times;
    stat_res[0][1] /= times;
    stat_res[0][2] /= times;
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
    auto t1 = std::chrono::high_resolution_clock::now();
    bt.bulk_load_x(data.begin(),data.end());
    // for(auto e : data){
    //     std::cout << e.first << " " << e.second << "\n";
    // }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto st1 = std::chrono::duration_cast<std::chrono::nanoseconds>(t1.time_since_epoch()).count();
    auto st2 = std::chrono::duration_cast<std::chrono::nanoseconds>(t2.time_since_epoch()).count();
    auto bulk_load_time = (st2 - st1);
    std::cout << "bulk_load_time = " << bulk_load_time / 1000000 << "\n";

    std::cout << "liner sub-liner 凸 凹 S1 S2 普通\n";
    for(int i = 0; i < 7; i++){
        std::cout << stx::data_distribution_count[i] << " ";
    }
    std::cout << "\n";

    // bt.erase_x(505331665);
    // std::cout << "min = " << (*data.begin()).first << ", max = " << (*(data.end()-1)).first << "\n";

    size_t inum = 1069446001;

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
        int right_count = 0, error_count = 0;

        uint64_t insert_times = 0;
        uint64_t erase_times = 0;
        uint64_t find_times = 0;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> intDistribution(1,data.size());
        // std::cout << "bt.size() " << bt.size() << "\n";
        // bt.print();
        // bt.find_x(123);

        for(int j = 0; j < insert_count; j++){
            int randomInt = intDistribution(gen) % data.size();
            auto key = data[randomInt].first;
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            // bt.insert_x(data[randomInt].first,data[randomInt].first);
            bt.insert_x(key, key);
            // bt.insert_x(inum++, inum++);
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            insert_times += (nanoseconds2 - nanoseconds1);
        }

        for(int j = 0; j < erase_count; j++){
            int randomInt = intDistribution(gen) % data.size();
            auto key = data[randomInt].first;
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            bt.erase_x(key);
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            erase_times += (nanoseconds2 - nanoseconds1);
        }

        for(int j = 0; j < find_count; j++){
            int randomInt = intDistribution(gen) % data.size();
            size_t ttt = data[randomInt].first;
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            auto res = bt.find_x(ttt);
            // auto res = bt.find_x(data[randomInt].first);
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();


            if(res == bt.end() ||  data[randomInt].first != res->first){

                error_count++;
            }else{
                right_count++;
            }

            find_times += (nanoseconds2 - nanoseconds1);
        }
        stat_res[2][0] += insert_times / insert_count;
        stat_res[2][1] += erase_times / erase_count;
        stat_res[2][2] += find_times / find_count;
#ifdef RES
        std::cout << "right_count = " << right_count << ", error_count = " << error_count << " "
        << __func__ 
        << " insert用时-纳秒:" << insert_times / insert_count 
        << " erase用时-纳秒:" << erase_times / erase_count 
        << " find用时-纳秒:" << find_times / find_count 
        << " 数据量-万:" << bt.size()/10000 << " |" ;
        print_sts(stx::gaps, stx::gaps_count);
        print_sts(stx::mul_times, stx::mul_counts);
        print_sts(stx::load_times, stx::load_counts);
        std::cout <<  "\n";
#endif
    }

    stat_res[2][0] /= times;
    stat_res[2][1] /= times;
    stat_res[2][2] /= times;

    return true;
}

auto test_alex(std::vector<std::pair<size_t,size_t>> data, 
    int insert_count, int erase_count, int find_count, int times) -> bool{
    alex_type index;

    for(auto& e : data){
        e.first = e.first * 4;
        e.second = e.second * 4;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    index.bulk_load(data.data(), data.size());
    auto t2 = std::chrono::high_resolution_clock::now();
    auto st1 = std::chrono::duration_cast<std::chrono::nanoseconds>(t1.time_since_epoch()).count();
    auto st2 = std::chrono::duration_cast<std::chrono::nanoseconds>(t2.time_since_epoch()).count();
    auto bulk_load_time = (st2 - st1);
    std::cout << "bulk_load_time = " << bulk_load_time / 1000000 << "\n";
    size_t states[2] = {0};

    size_t inum = 1069446001;
    for(int i=0;i<times;i++){

        for(int j=0;j<5;j++){
            stx::gaps[j] = 0;
            stx::gaps_count[j] = 0;
            stx::level_delay[j] = 0;
        }
        int right_count = 0, error_count = 0;

        uint64_t insert_times = 0;
        uint64_t erase_times = 0;
        uint64_t find_times = 0;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> intDistribution(1,data.size());

        for(int j = 0; j < insert_count; j++){
            int randomInt = intDistribution(gen)  % data.size();
            auto currentTime1 = std::chrono::high_resolution_clock::now();
            // std::cout << data[randomInt].first << "\n";
            // index.insert(inum++, inum++);
            index.insert(data[randomInt].first,data[randomInt].second);
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            insert_times += (nanoseconds2 - nanoseconds1);
        }

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
            auto res = index.get_payload(ttt);
            // auto resd = *res;
            auto currentTime2 = std::chrono::high_resolution_clock::now();
            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
            find_times += (nanoseconds2 - nanoseconds1);
            if(res){
                // std::cout << data[randomInt].first << std::endl;
                // if(data[randomInt].first != *res) std::cout << data[randomInt].first <<" "<< *res << std::endl;
                if(data[randomInt].first != *res)  error_count++;
                else right_count++;
            }else{
                error_count++;
            }
        }

        stat_res[1][0] += (insert_times / insert_count);
        stat_res[1][1] += (erase_times / erase_count);
        stat_res[1][2] += (find_times / find_count);

#ifdef RES
        std::cout
        << __func__ 
        << " insert用时-纳秒:" << (insert_times / insert_count)
        << " erase用时-纳秒:" << (erase_times / erase_count)
        << " find用时-纳秒:" << (find_times / find_count)
        << " 数据量-万:" << index.size()/10000 
        <<  "\n";
#endif
    }



    stat_res[1][0] /= times;
    stat_res[1][1] /= times;
    stat_res[1][2] /= times;

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
//   for(auto e : keys){
//     std::cout << e << "\n";
//   }

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
    std::cout << "keys.begin() = " << *keys.begin() << "\n";
    std::cout << "keys.end() = " << *keys.end() << "\n";
    std::vector<std::pair<size_t,size_t>> data(keys.size());
    for(size_t i=0;i<data.size();i++){
        data[i].first = keys[i];
        data[i].second = keys[i];
    }

    int erase_count = 0;
    int insert_count = 1000;
    int find_count = 0;
    int loop = 200;

    test_btree_x(data, insert_count, erase_count, find_count, loop);
    test_btree_x(data, insert_count, erase_count, find_count, loop);
    test_btree_x(data, insert_count, erase_count, find_count, loop);
    test_btree_x(data, insert_count, erase_count, find_count, loop);
    test_btree_x(data, insert_count, erase_count, find_count, loop);

    test_btree(  data, insert_count, erase_count, find_count, loop);
    test_btree(  data, insert_count, erase_count, find_count, loop);
    test_btree(  data, insert_count, erase_count, find_count, loop);
    test_btree(  data, insert_count, erase_count, find_count, loop);
    test_btree(  data, insert_count, erase_count, find_count, loop);
    // test_alex(   data, insert_count, erase_count, find_count, loop);
    // test_alex(   data, insert_count, erase_count, find_count, loop);
    // test_alex(   data, insert_count, erase_count, find_count, loop);
    // test_alex(   data, insert_count, erase_count, find_count, loop);
    // test_alex(   data, insert_count, erase_count, find_count, loop);

    std::cout << "index insert erase find\n";
    // btree alex btree_x
    std::string index_names[] = {"btree", "alex", "btree_x"};
    for(int i = 0; i < 3; i++){
        std::cout << index_names[i] << " ";

        for(auto& l : stat_res[i]){
            std::cout << l << " ";
        }
        std::cout << "\n";

    }

    double ins_rio = insert_count / (find_count + insert_count + 0.0);
    double find_rio = find_count / (find_count + insert_count + 0.0);
    // std::cout << ins_rio << " " << find_rio << "\n";

    std::cout << "qps milion\n";
    std::cout << 2500 / (stat_res[0][0] * ins_rio + stat_res[0][2] * find_rio) << "\n";
    std::cout << 2500 / (stat_res[1][0] * ins_rio + stat_res[1][2] * find_rio) << "\n";
    std::cout << 2500 / (stat_res[2][0] * ins_rio + stat_res[2][2] * find_rio) << "\n";

    return 0;

}















