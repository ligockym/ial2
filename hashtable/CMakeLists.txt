cmake_minimum_required(VERSION 3.16)
project(ial_2021_du2 C)

set(CMAKE_C_STANDARD 11)


add_executable(hashtable
        hashtable.h
        hashtable.c
        test_util.h
        test_util.c
        test.c
        )

