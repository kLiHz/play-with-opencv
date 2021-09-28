project(draw)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_executable(flower "flower.cpp")
target_link_libraries(flower ${OpenCV_LIBS})

add_executable(polygon "polygon.cpp")
target_link_libraries(polygon ${OpenCV_LIBS})
