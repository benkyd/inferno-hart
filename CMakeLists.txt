cmake_minimum_required(VERSION 3.13)
project(inferno)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.c")

add_executable(inferno ${SRC})
target_include_directories(inferno PRIVATE "libhart/")
target_include_directories(inferno PRIVATE "libhart/thirdparty")
target_include_directories(inferno PRIVATE "src/")
target_include_directories(inferno PRIVATE "src/thirdparty")

# Hardware Acceleration Modules (HART)
#add_subdirectory("hart/inferno-hart-cpu")
#add_subdirectory("hart/inferno-hart-opencl")

# Compiler arguments
if (WIN32)
    target_compile_options(inferno PRIVATE /std:c++17 /EHsc)
elseif (APPLE)
    target_compile_options(inferno PRIVATE -std=c++17 -Wno-unused-command-line-argument -undefined dynamic_lookup)
    if (CMAKE_BUILD_TYPE MATCHES "Release")
        target_compile_options(inferno PRIVATE -O3)
    endif()
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        target_compile_options(inferno PRIVATE -O1)
endif()
else()
    target_compile_options(inferno PRIVATE -std=c++17)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        target_compile_options(inferno PRIVATE -O1)
        target_compile_options(inferno PRIVATE -Wall)
        target_compile_options(inferno PRIVATE -g)
    endif()
endif()

# "Universal" libraries
set(THREADS_PREFER_PTHREAD_FLAD ON)
find_package(Threads REQUIRED)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# Libraries
if (WIN32)
    # cmake .. "-DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"
    find_package(glfw3 CONFIG REQUIRED)
    target_link_libraries(inferno PUBLIC glfw)

    # OpenGL
    target_link_libraries(inferno PRIVATE OpenGL::GL)
else()
    find_package(PkgConfig)

    pkg_check_modules(GLFW3 REQUIRED glfw3)

    target_include_directories(inferno PRIVATE
        ${GLFW3_INCLUDE_DIRS}
    )

    target_link_directories(inferno PRIVATE
        ${GLFW3_LIBRARY_DIRS}
    )

    target_link_libraries(inferno PRIVATE
        ${GLFW3_LIBRARIES}
        OpenGL::GL
    )
endif()

