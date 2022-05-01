# CMakeList.txt : CMake project for DCC-Miner, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.0 FATAL_ERROR)

# Add source to this project's executable.
add_executable (DCC-Miner "Main.cpp" "Main.h"  "Console.cpp" "json.hpp" "FileManip.cpp" "extlibs/elzip" "Network.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DCC-Miner PROPERTY CXX_STANDARD 20)
endif()

if(WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG ee7f91f5566b6279aadf6d0b1aaf56b11963f527) # The commit hash for 1.8.2. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

target_link_libraries(DCC-Miner cpr::cpr)

find_package(OpenSSL REQUIRED)
target_link_libraries(DCC-Miner OpenSSL::SSL)

set(Boost_ADDITIONAL_VERSIONS 1.78.0 1.78)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
FIND_PACKAGE(Boost 1.78.0 COMPONENTS program_options) 

include_directories(${Boost_INCLUDE_DIRS}) 

target_link_libraries(DCC-Miner m ${Boost_LIBRARIES})

#add_subdirectory(extlibs/elzip) # Path to the 11zip

#target_link_libraries(DCC-Miner elzip)