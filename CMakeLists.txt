cmake_minimum_required(VERSION 3.16)
project(polygon_cgal LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(CGAL REQUIRED COMPONENTS Core)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(polygon_intersections
    src/polygon_intersections.cpp
)

target_link_libraries(polygon_intersections
    PRIVATE
        CGAL::CGAL
        CGAL::CGAL_Core
)
