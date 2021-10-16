project(hybrid)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(fmt REQUIRED)

add_executable(hybrid "main.cpp")
target_link_libraries(hybrid ${OpenCV_LIBS} fmt::fmt)
