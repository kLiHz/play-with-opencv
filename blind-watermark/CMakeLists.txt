project(blind-watermark)
cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_executable(blind-wm "main.cpp")
target_link_libraries(blind-wm ${OpenCV_LIBS})
