# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project("dcc_client" CXX C)

# Include sub-projects.
add_subdirectory ("dcc_client")
 