project("Hello, World!")

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_executable(imshow "imshow.cpp")
target_link_libraries(imshow ${OpenCV_LIBS})
