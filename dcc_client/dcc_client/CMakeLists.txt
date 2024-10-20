# CMakeList.txt : CMake project for dcc_client, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTORCC ON)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_COMPILER_IS_GNUXX)
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wzero-as-null-pointer-constant")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif(CMAKE_COMPILER_IS_GNUXX)


# Add source to this project's executable.
add_executable (dcc_client "json.hpp" "Main.h" "Main.cpp" "Console.h" "Console.cpp" "FileManip.h" "FileManip.cpp" "Network.h" "Network.cpp" "strops.h" "strops.cpp" "P2PClient.h" "P2PClient.cpp" "Network.h" "Network.cpp" "crypto.h" "crypto.cpp" "Blockchain.cpp" "Blockchain.h" "Miner.cpp" "Miner.h" "SettingsConsts.h" "SettingsConsts.cpp" "System.cpp" "System.h" "BaseConverter.cpp" "BaseConverter.h" "benchmark.cpp" "benchmark.cpp" "indicators.hpp" "uint128.h" "uint256.h" "uint128.inl" "uint256.inl" "DCC-Logo.rc")


#add_subdirectory(extlibs/elzip) # Path to the 11zip
#target_link_libraries(dcc_client elzip)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET dcc_client PROPERTY CXX_STANDARD 20)
endif()

if(WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG ee7f91f5566b6279aadf6d0b1aaf56b11963f527) # The commit hash for 1.8.2. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

#target_link_libraries(dcc_client PRIVATE cpr::cpr)

#find_package(OpenSSL REQUIRED)
#target_link_libraries(dcc_client OpenSSL::SSL)

if(WIN32)
    set(Boost_ADDITIONAL_VERSIONS 1.78.0 1.78)
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    FIND_PACKAGE(Boost 1.78.0 REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    set(Boost_ADDITIONAL_VERSIONS 1.71.0 1.71)
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    FIND_PACKAGE(Boost 1.71.0 REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})

    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIR})
endif()



#find_package(CURL REQUIRED)
#add_executable (dcc_client convert.cpp)
#include_directories(${CURL_INCLUDE_DIR})
#target_link_libraries(dcc_client ${CURL_LIBRARIES})

if(WIN32)
set(CURL_INCLUDE_DIR "C:\\curl\\include")
set(CURL_LIBRARY "C:\\curl")
endif()

#set(LIB_CRYPTO "lib/libcrypto.lib")

add_definitions(-DCURL_STATICLIB)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

#include_directories("C:\\Program Files\\OpenSSL-Win64\\")
if(WIN32)
set(OPENSSL_ROOT_DIR "C:\\Program Files\\OpenSSL-Win64\\")
endif()
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

if(WIN32)
	target_link_libraries(dcc_client ${CURL_LIBRARY} ${Boost_LIBRARIES} "C:\\Boost\\lib\\libboost_filesystem-vc143-mt-gd-x64-1_78.lib" OpenSSL::SSL cpr::cpr OpenSSL::Crypto)
else()
	target_link_libraries(dcc_client ${CURL_LIBRARY} ${CURSES_LIBRARIES} ${Boost_LIBRARIES} OpenSSL::SSL cpr::cpr OpenSSL::Crypto)
endif()

#set(ELZIP_LIBRARY include/lib/libcurl_a_debug)
#target_link_libraries(dcc_client )

# find_package(CURL REQUIRED)
# if (WIN32)
#   list(LENGTH CURL_LIBRARY CURL_LIBRARY_LENGTH)
#   if (CURL_LIBRARY_LENGTH EQUAL 1)
#     set(CURL_LIBRARY_DEBUG_LIB   ${CURL_LIBRARY})
#      set(CURL_LIBRARY_RELEASE_LIB ${CURL_LIBRARY_DEBUG_LIB}/../../../lib/libcurl.lib)
#       get_filename_component(CURL_LIBRARY_RELEASE_LIB ${CURL_LIBRARY_RELEASE_LIB} REALPATH)
#       unset(CURL_LIBRARY CACHE)
#       unset(CURL_LIBRARY)
#       unset(CURL_LIBRARIES  CACHE)
#       unset(CURL_LIBRARIES)
#       set(CURL_LIBRARY "debug;${CURL_LIBRARY_DEBUG_LIB};optimized;${CURL_LIBRARY_RELEASE_LIB}")
#       set(CURL_LIBRARIES ${CURL_LIBRARY})
#   endif()
# endif()

#add_library(elzip SHARED IMPORTED)
#set_target_properties(elzip PROPERTIES
#    IMPORTED_LOCATION "extlibs/zlib1.dll"
#)
