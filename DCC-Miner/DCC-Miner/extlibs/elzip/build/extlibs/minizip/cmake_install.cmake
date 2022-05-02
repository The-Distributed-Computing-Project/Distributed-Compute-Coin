# Install script for directory: /mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/build/extlibs/minizip/libminizip.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip/minizip.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip/minizip.cmake"
         "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/build/extlibs/minizip/CMakeFiles/Export/lib/cmake/minizip/minizip.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip/minizip-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip/minizip.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip" TYPE FILE FILES "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/build/extlibs/minizip/CMakeFiles/Export/lib/cmake/minizip/minizip.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip" TYPE FILE FILES "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/build/extlibs/minizip/CMakeFiles/Export/lib/cmake/minizip/minizip-noconfig.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip" TYPE FILE FILES
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/build/extlibs/minizip/minizip-config-version.cmake"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/build/extlibs/minizip/minizip-config.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz.h"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_os.h"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_crypt.h"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_strm.h"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_strm_buf.h"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_strm_mem.h"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_strm_split.h"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_strm_os.h"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_zip.h"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_zip_rw.h"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_strm_zlib.h"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_compat.h"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/zip.h"
    "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/unzip.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/mnt/d/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/build/extlibs/minizip/minizip.pc")
endif()

