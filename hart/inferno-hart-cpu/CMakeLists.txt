cmake_minimum_required(VERSION 3.13)
project(hart_cpu_accel)

file(GLOB SRC "src/*.cpp")

add_library(hart_cpu_accel SHARED ${SRC})
set_target_properties(hart_cpu_accel PROPERTIES PREFIX "")

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(hart_cpu_accel PUBLIC OpenMP::OpenMP_CXX)
endif()

target_include_directories(hart_cpu_accel PRIVATE "../../libhart/")
target_include_directories(hart_cpu_accel PRIVATE "../../libhart/thirdparty")
target_include_directories(hart_cpu_accel PRIVATE "src/")

