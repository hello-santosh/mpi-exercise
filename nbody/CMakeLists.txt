cmake_minimum_required(VERSION 3.10)

project(main)
add_executable(nbody nbody.cpp)
find_package(MPI REQUIRED)
target_link_libraries(nbody MPI::MPI_CXX)
