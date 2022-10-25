cmake_minimum_required(VERSION 3.13)
project(hart_ocl_accel)

file(GLOB SRC "src/*.cpp")

add_library(hart_ocl_accel SHARED ${SRC})
set_target_properties(hart_ocl_accel PROPERTIES PREFIX "")

target_include_directories(hart_ocl_accel PRIVATE "../../libhart/")
target_include_directories(hart_ocl_accel PRIVATE "../../libhart/thirdparty")
target_include_directories(hart_ocl_accel PRIVATE "src/")
