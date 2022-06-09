cmake_minimum_required(VERSION 3.10)

project(main)
add_executable(treeSum treeSum.cpp)
find_package(MPI REQUIRED)
target_link_libraries(treeSum MPI::MPI_CXX)
