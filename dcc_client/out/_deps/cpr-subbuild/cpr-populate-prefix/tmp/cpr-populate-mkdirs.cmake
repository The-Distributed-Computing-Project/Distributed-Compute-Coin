# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/samda/Code/Distributed-Compute-Coin/dcc_client/out/_deps/cpr-src"
  "C:/Users/samda/Code/Distributed-Compute-Coin/dcc_client/out/_deps/cpr-build"
  "C:/Users/samda/Code/Distributed-Compute-Coin/dcc_client/out/_deps/cpr-subbuild/cpr-populate-prefix"
  "C:/Users/samda/Code/Distributed-Compute-Coin/dcc_client/out/_deps/cpr-subbuild/cpr-populate-prefix/tmp"
  "C:/Users/samda/Code/Distributed-Compute-Coin/dcc_client/out/_deps/cpr-subbuild/cpr-populate-prefix/src/cpr-populate-stamp"
  "C:/Users/samda/Code/Distributed-Compute-Coin/dcc_client/out/_deps/cpr-subbuild/cpr-populate-prefix/src"
  "C:/Users/samda/Code/Distributed-Compute-Coin/dcc_client/out/_deps/cpr-subbuild/cpr-populate-prefix/src/cpr-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/samda/Code/Distributed-Compute-Coin/dcc_client/out/_deps/cpr-subbuild/cpr-populate-prefix/src/cpr-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/samda/Code/Distributed-Compute-Coin/dcc_client/out/_deps/cpr-subbuild/cpr-populate-prefix/src/cpr-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
