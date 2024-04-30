# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.29.2)

# Reject any attempt to use a toolchain file. We must not use one because
# we could be downloading it here. If the CMAKE_TOOLCHAIN_FILE environment
# variable is set, the cache variable will have been initialized from it.
unset(CMAKE_TOOLCHAIN_FILE CACHE)
unset(ENV{CMAKE_TOOLCHAIN_FILE})

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(curl-populate NONE)



include(ExternalProject)
ExternalProject_Add(curl-populate
                     "UPDATE_DISCONNECTED" "False" "DOWNLOAD_EXTRACT_TIMESTAMP" "TRUE" "EXTERNALPROJECT_INTERNAL_ARGUMENT_SEPARATOR" "URL" "https://github.com/curl/curl/releases/download/curl-7_80_0/curl-7.80.0.tar.xz" "URL_HASH" "SHA256=a132bd93188b938771135ac7c1f3ac1d3ce507c1fcbef8c471397639214ae2ab"
                    SOURCE_DIR          "C:/Users/samda/Code/Distributed-Compute-Coin/dcc_client/out/_deps/curl-src"
                    BINARY_DIR          "C:/Users/samda/Code/Distributed-Compute-Coin/dcc_client/out/_deps/curl-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
                    USES_TERMINAL_PATCH     YES
)


