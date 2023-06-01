# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-Debug/_deps/zlib-src"
  "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-Debug/_deps/zlib-build"
  "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-Debug/_deps/zlib-subbuild/zlib-populate-prefix"
  "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-Debug/_deps/zlib-subbuild/zlib-populate-prefix/tmp"
  "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-Debug/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp"
  "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-Debug/_deps/zlib-subbuild/zlib-populate-prefix/src"
  "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-Debug/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-Debug/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-Debug/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
