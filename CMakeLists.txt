# cmake 最低版本需求
cmake_minimum_required(VERSION 3.11)
# 工程名称
project(cmp_lab)
set(CMAKE_CXX_STANDARD 17)
add_executable(btree_prefetch src/main.cpp)

if(MSVC)
    set(CMAKE_CXX_FLAGS "/O2 /arch:AVX2 /W1 /EHsc")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "-O3 -xHost")
else()
    # clang and gcc
    set(CMAKE_CXX_FLAGS "-O1 -march=native -Wall -Wextra")
endif()


include_directories(./)
include_directories(lipp/src/core)
include_directories(DyTIS)
include_directories(DyTIS/benchmark)
# include_directories(/usr/include)
# include_directories(/home/qinjingyuan/index_opt/btree_prefetch/src/include)

add_definitions( -ggdb)
add_definitions( -DSEP -DNDEBUG )
add_definitions( -lrt -lpthread -lm )
add_definitions( -flto -fno-stack-protector  -Wno-unknown-pragmas  -rdynamic -O3 -Wall -Wextra -march=native -Wall -Wextra -Wshadow  )

