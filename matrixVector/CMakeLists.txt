cmake_minimum_required(VERSION 3.10)

project(main)
add_executable(matrixVector matrixVector.cpp)
find_package(MPI REQUIRED)
target_link_libraries(matrixVector MPI::MPI_CXX)
