cmake_minimum_required(VERSION 3.14)
project(Projet_OpenCV_CMake)


set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})


add_executable(Projet_OpenCV_CMake
        include/histogram.hpp
        src/main_projet.cpp
        src/text_size.cpp
        src/icon_label.cpp
        include/redresser_image.h
        src/redresser_image.cpp)

target_link_libraries(Projet_OpenCV_CMake ${OpenCV_LIBS})

