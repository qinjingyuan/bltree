#include<iostream>
#include<stdio.h>
#include<ctime>
// time
#include <iostream>  
#include <sys/time.h>
#include <cstdlib>  
#include <cstdio>
#include <ctime>
#include <cmath>
#include <unistd.h>

// btree
#include <stx/btree_multiset.h>
#include <stx/btree_multimap.h>
#include <stx/btree_map.h>
#include <cstdlib>
#include <inttypes.h>
#include "tpunit.h"
#include <vector>

#include <pthread.h>
#include <chrono>

// using std::chrono::high_resolution_clock;
// using std::chrono::milliseconds;

// template <int Slots>
// struct SimpleTest : public tpunit::TestFixture
// {
//     SimpleTest() : tpunit::TestFixture(
//                        TEST(SimpleTest::test_map_insert_erase_3200)
//                        )
//     { }

//     template <typename KeyType>
//     struct traits_nodebug : stx::btree_default_set_traits<KeyType>
//     {
//         static const bool selfverify = true;
//         static const bool debug = true;

//         static const int  leafslots = Slots;
//         static const int  innerslots = Slots;
//     };

//     void test_empty()
//     {
//         typedef stx::btree_multiset<unsigned int,
//                                     std::less<unsigned int>, traits_nodebug<unsigned int> > btree_type;

//         btree_type bt, bt2;
//         bt.verify();
//         std::cout << "bt.verify();" << "\n";

//         ASSERT(bt.erase(42) == false);

//         ASSERT(bt == bt2);
//     }

//     void test_map_insert_erase_3200()
//     {
//         typedef stx::btree_multimap<unsigned int, std::string,
//                                     std::less<unsigned int>, traits_nodebug<unsigned int> > btree_type;

//         btree_type bt;

//         srand(34234235);
//         for (unsigned int i = 0; i < 320000; i++)
//         {
//             ASSERT(bt.size() == i);
//             bt.insert2(rand() % 1000, "10100");
//             ASSERT(bt.size() == i + 1);
//         }


//     using namespace std;
//     struct timeval tv;  
//     gettimeofday(&tv,NULL); 


//         for(int i=0;i<10;i++){
//             srand(34234235);
//             for (unsigned int i = 0; i < 320000; i++)
//             {
//                 bt.find(rand() % 1000);

//             }
//         }

//     struct timeval tv1;  
//     gettimeofday(&tv1,NULL); 
//     std::cout<<"10e6 微秒级s ----:";
//     std::cout<<tv1.tv_sec<<"s,"<<tv1.tv_usec<<"微秒"<<endl;
//     std::cout<<"用时"<< tv1.tv_sec - tv.tv_sec << "秒，"  <<  tv1.tv_usec - tv.tv_usec<<"微秒"<<endl;

//         // srand(34234235);
//         // for (unsigned int i = 0; i < 32000; i++)
//         // {
//         //     ASSERT(bt.size() == 3200 - i);
//         //     ASSERT(bt.erase_one(rand() % 100));
//         //     ASSERT(bt.size() == 3200 - i - 1);
//         // }

//         // ASSERT(bt.empty());
//         bt.verify();
//     }

// };


bool SubTimeval(timeval &result, timeval &begin, timeval &end)
// 计算gettimeofday函数获得的end减begin的时间差，并将结果保存在result中。
{
          if ( begin.tv_sec>end.tv_sec ) return false;
    
          if ( (begin.tv_sec == end.tv_sec) && (begin.tv_usec > end.tv_usec) )   
          return   false;
    
          result.tv_sec = ( end.tv_sec - begin.tv_sec );   
          result.tv_usec = ( end.tv_usec - begin.tv_usec );   
    
          if (result.tv_usec<0) {
                   result.tv_sec--;
                   result.tv_usec+=1000000;}  

          return true;
}
// unsigned long ns_count[4] = {0};

struct BulkLoadTest : public tpunit::TestFixture
{
    BulkLoadTest() : tpunit::TestFixture(
                         TEST(BulkLoadTest::test_map)
                         )
    { }

    template <typename KeyType>
    struct traits_nodebug : stx::btree_default_set_traits<KeyType>
    {
        static const bool selfverify = false;
        static const bool debug = false;

        static const int  leafslots = 1024;
        static const int  innerslots = 1024;
    };

    void test_map_instance(size_t numkeys, unsigned int mod)
    {
        int find_count = 1000000;
        typedef stx::btree_multimap<size_t, size_t,
                                    std::less<unsigned int>, traits_nodebug<unsigned int> > btree_type;

        std::vector<std::pair<size_t, size_t> > pairs(numkeys);

        srand(342342350);
        for (unsigned int i = 0; i < numkeys; i++)
        {
            size_t insert_num =  rand() % mod;
            pairs[i].first = insert_num;
            pairs[i].second = insert_num;
        }

        std::sort(pairs.begin(), pairs.end());

        btree_type bt;
        
        bt.bulk_load(pairs.begin(), pairs.end());

        std::cout << "btree size is " << bt.size() << "\n";

        // using namespace std;
        // struct timeval tstart,tend,tsub;  
        // gettimeofday(&tstart,NULL); 
        // clock_t start, ends;
        // start = clock();
        


        
        for(auto i : std::vector<int>{2}){ // find_level list
            std::vector<int>  hi_list{0};
            for(auto j : hi_list){
                // int j = 0;
                std::vector<int>  set_cacheline_n_list{3};
                for(auto n : set_cacheline_n_list){
                    std::vector<stx::search_type> v_search_type{
                        stx::search_type::SOURCE,
                        stx::search_type::GENERAL,
                        stx::search_type::DIRECT,
                        stx::search_type::EX_PREFECTCH
                    };
                    for(auto searchtype:v_search_type){

                        stx::search_type_ = searchtype;
                        stx::ns_count[0] = 0;
                        stx::ns_count[1] = 0;
                        stx::ns_count[2] = 0;
                        stx::ns_count[3] = 0;
                        stx::ns_count[4] = 0;
                        bt.set_prefetch_true(1);
                        bt.set_prefetch_false(1);
                        bt.set_prefetch_level(i);
                        bt.set_prefetch_hi(j);
                        bt.set_cacheline_n(n);
                        
                        srand(342342350);
                        unsigned long using_times = 0;
                        for (unsigned int i = 0; i < find_count; i++)
                        {
                            size_t tmp = (rand() % mod);
                            auto currentTime1 = std::chrono::high_resolution_clock::now();
                            auto res = bt.find(tmp);
                            // std::cout << tmp <<" "<< res->first << " " << res->second << std::endl;
                            auto currentTime2 = std::chrono::high_resolution_clock::now();

                            auto nanoseconds1 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime1.time_since_epoch()).count();
                            auto nanoseconds2 = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime2.time_since_epoch()).count();
                            using_times +=  (nanoseconds2 - nanoseconds1);

                        }
                        size_t ptrue_count = bt.get_prefetch_true();
                        size_t pfalse_count = bt.get_prefetch_false();

                        std::cout 
                        << "用时:" << using_times / find_count 
                        << " 数据量:" << numkeys/10000 
                        << " 命中率:" << ptrue_count * 100 / (ptrue_count + pfalse_count) 
                        << " 命中次数:" << ptrue_count 
                        << " 未命中次数:" << pfalse_count 
                        << " pfch_level:" << i 
                        << " pfch_hi:" << j 
                        << " cacheline_n:" << n  
                        << " L0ns:" << stx::ns_count[0]/find_count
                        << " L1ns:" << stx::ns_count[1]/find_count
                        << " L2ns:" << stx::ns_count[2]/find_count
                        << " L3ns:" << stx::ns_count[3]/find_count
                        << " L4ns:" << stx::ns_count[4]/find_count
                        << " allns:" << (stx::ns_count[1] + stx::ns_count[2] + stx::ns_count[3])/find_count 
                        <<  "\n";
                    }

                }
            }

        }

        unsigned int i = 0;
        for (btree_type::iterator it = bt.begin();
             it != bt.end(); ++it, ++i)
        {
            ASSERT(*it == pairs[i]);
        }
    }

    void test_map()
    {

        // test_map_instance(1000000, 1000000);
        // test_map_instance(2000000, 2000000);
        // test_map_instance(5000000, 5000000);
        test_map_instance(10000000, 10000000);
        test_map_instance(20000000, 20000000);
        test_map_instance(50000000, 50000000);
        test_map_instance(100000000, 100000000);
        test_map_instance(200000000, 200000000);
        test_map_instance(500000000, 500000000);
        // test_map_instance(1000000000, 1000000000);
        // test_map_instance(2000000000, 2000000000);
        // test_map_instance(5000000000, 5000000000);


    }
} _BulkLoadTest;



// struct SimpleTest<1024> _SimpleTest1024;

using namespace std;

#define NUM_THREADS 5

void* say_hello(void* args)
{
    cout << "Hello Runoob！" << endl;
    return NULL;
}

int main(int argc,char* args[]){
    // // 定义线程的 id 变量，多个变量使用数组
    // pthread_t tids[NUM_THREADS];
    // // void* thread_result;
    // for(int i = 0; i < NUM_THREADS; ++i)
    // {
    //     //参数依次是：创建的线程id，线程参数，调用的函数，传入的函数参数
    //     // int ret = pthread_create(&tids[i], NULL, say_hello, NULL);
    //     int ret = pthread_create(&tids[i], NULL, tpunit::Tests::Run, (void *)&tids[i]);
    //     if (ret != 0)
    //     {
    //        cout << "pthread_create error: error_code=" << ret << endl;
    //     }
    //     // pthread_join(tids[i], NULL);
    // }
    // //等各个线程退出后，进程才结束，否则进程强制结束了，线程可能还没反应过来；
    // pthread_exit(NULL);
    tpunit::Tests::Run(NULL);
    return 0;
}






