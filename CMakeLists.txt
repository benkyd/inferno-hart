cmake_minimum_required(VERSION 3.13)
project(inferno)

# file(GLOB SRC "src/*.cpp" "src/graphics/*.cpp" "src/preview_renderer/*.cpp" "src/scene/*.cpp" "src/thirdparty/imgui/*.cpp" "src/preview_renderer/*.cpp")
file(GLOB_RECURSE SRC "src/*.cpp")

add_subdirectory("thirdparty/assimp")

add_executable(inferno ${SRC})
target_include_directories(inferno PRIVATE "libhart/thirdparty")
target_include_directories(inferno PRIVATE "libhart/")
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
    target_compile_options(inferno PRIVATE -std=c++20)
    target_compile_options(inferno PRIVATE -m64)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        target_compile_options(inferno PRIVATE -O1)
        target_compile_options(inferno PRIVATE -Wall)
        target_compile_options(inferno PRIVATE -g)
        target_compile_options(inferno PRIVATE -D_GLIBCXX_DEBUG)
    endif()
endif()

# Compile shaders first..
file(GLOB SHADERS "res/shaders/*.vert" "res/shaders/*.frag")
foreach(SHADER ${SHADERS})
    get_filename_component(FILENAME ${SHADER} ABSOLUTE)
    set(SPIRV "${FILENAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND glslc ${SHADER} -o ${SPIRV}
        DEPENDS ${SHADER}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(SHADER)

add_custom_target(
    shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(inferno shaders)

# Copy resources
#invole install_res.sh on compile with make
add_custom_command(TARGET shaders POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/install_res.sh
    COMMENT "Copying resources"
)

# "Universal" libraries
set(THREADS_PREFER_PTHREAD_FLAD ON)
find_package(Threads REQUIRED)

find_package(OpenMP REQUIRED)

find_package(Vulkan REQUIRED)

# Libraries
target_link_libraries(inferno PRIVATE assimp)

if (WIN32)
    # cmake .. "-DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"
    find_package(glfw3 CONFIG REQUIRED)
    target_link_libraries(inferno PUBLIC glfw)

    target_link_libraries(inferno PRIVATE Vulkan::Vulkan)
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
        Vulkan::Vulkan
        libvulkan.so
        OpenMP::OpenMP_CXX
    )
endif()

