project(draw)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_executable(draw "main.cpp" "methods.hpp" "polygon.cpp" "flower.cpp")

target_link_libraries(draw ${OpenCV_LIBS})
