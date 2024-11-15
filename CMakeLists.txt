cmake_minimum_required(VERSION 3.28)
project(hw2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(inc test)
add_executable(hw2 main.cpp
        inc/standardMultiplication.h
        test/test.h
        inc/fastMultiplication.h
        test/generateNums.h)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
add_compile_options(-fstack-size=8388608)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -march=native -mtune=native")