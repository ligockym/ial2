cmake_minimum_required(VERSION 3.16)
project(ial_2021_du2 C)

set(CMAKE_C_STANDARD 11)

include_directories(iter)

add_executable(btree
        iter/stack.h
        iter/stack.c
        iter/btree.c
        btree.h
        btree.c
        iter/btree.c
        test_util.h
        test_util.c
        test.c
        )


