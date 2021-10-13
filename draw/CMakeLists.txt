project(Draw)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(fmt REQUIRED)

add_executable(draw "main.cpp" "elements.hpp" "elements.cpp" "draw.hpp" "draw_polygon.cpp" "draw_flower.cpp")

target_link_libraries(draw opencv_highgui opencv_imgcodecs opencv_imgproc fmt::fmt)
