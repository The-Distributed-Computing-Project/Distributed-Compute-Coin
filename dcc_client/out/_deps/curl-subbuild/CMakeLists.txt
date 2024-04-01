# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.21.4)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(curl-populate NONE)



include(ExternalProject)
ExternalProject_Add(curl-populate
                     "UPDATE_DISCONNECTED" "False" "URL" "https://github.com/curl/curl/releases/download/curl-7_80_0/curl-7.80.0.tar.xz" "URL_HASH" "SHA256=a132bd93188b938771135ac7c1f3ac1d3ce507c1fcbef8c471397639214ae2ab"
                    SOURCE_DIR          "D:/Code/DC-Cryptocurrency/dcc_client/out/_deps/curl-src"
                    BINARY_DIR          "D:/Code/DC-Cryptocurrency/dcc_client/out/_deps/curl-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)


