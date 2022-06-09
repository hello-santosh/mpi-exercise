cmake_minimum_required(VERSION 3.10)

project(main)
add_executable(monteCarloMpi monteCarloMpi.cpp)
find_package(MPI REQUIRED)
target_link_libraries(monteCarloMpi MPI::MPI_CXX)
