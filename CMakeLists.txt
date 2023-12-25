# cmake 最低版本需求
cmake_minimum_required(VERSION 3.11)
# 工程名称
project(cmp_lab)
set(CMAKE_CXX_STANDARD 17)
add_executable(btree_opt src/main.cpp)
# add_library(stx/bltree.h)


if(MSVC)
    set(CMAKE_CXX_FLAGS "/O2 /arch:AVX2 /W1 /EHsc")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "-O3 -xHost")
else()
    # clang and gcc
    set(CMAKE_CXX_FLAGS "-O1 -march=native -Wextra")
    # set(CMAKE_CXX_FLAGS "-O1 -march=native -Wall -Wextra")
endif()

include_directories(./)
include_directories(lipp/src/core)
include_directories(DyTIS)
include_directories(DyTIS/benchmark)
include_directories(eigen)
# include_directories(/usr/include)
# include_directories(/home/qinjingyuan/index_opt/btree_prefetch/src/include)

add_definitions( -ggdb)
# add_definitions( -DSEP -DNDEBUG )
# add_definitions( -DTIMING )
add_definitions( -lrt -lpthread -lm )
# add_definitions( -O0 -Wall -fno-stack-protector  )
add_definitions( -flto -fno-stack-protector -Wno-unknown-pragmas  -rdynamic -O2 -g  -Wextra -march=native  -Wextra -Wshadow  )
# add_definitions(-DMYDEBUG)
# add_definitions(-DX3COUNT_TIME)
# add_definitions(-DLINECOUNT_TIME)
# add_definitions(-DX6COUNT_TIME)
# add_definitions(-DLEVEL_COUNT_TIME)
# add_definitions(-DNODE_TYPE_COUNTS)

add_definitions(-DMUL_TIME)
add_definitions(-DLOAD_TIME)
add_definitions(-DCOUNT_GAP)






