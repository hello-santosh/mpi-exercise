cmake_minimum_required(VERSION 3.10)

project(main)
add_executable(newTreeSum newTreeSum.cpp)
find_package(MPI REQUIRED)
target_link_libraries(newTreeSum MPI::MPI_CXX)
