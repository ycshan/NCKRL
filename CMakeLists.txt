cmake_minimum_required(VERSION 3.5)
project(NCKRL)

set(CMAKE_CXX_STANDARD 11)

add_executable(NCKRL main.cpp Config.h Train.cpp Train.h Test.cpp Test.h Utilities.h DataModel.h)