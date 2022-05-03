# Install script for directory: D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/Code/DC-Cryptocurrency/DCC-Miner/out/install/x64-debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-debug/DCC-Miner/extlibs/elzip/extlibs/minizip/libminizip.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-debug/_deps/zlib-build/zlibstaticd.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip/minizip.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip/minizip.cmake"
         "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-debug/DCC-Miner/extlibs/elzip/extlibs/minizip/CMakeFiles/Export/lib/cmake/minizip/minizip.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip/minizip-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip/minizip.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip" TYPE FILE FILES "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-debug/DCC-Miner/extlibs/elzip/extlibs/minizip/CMakeFiles/Export/lib/cmake/minizip/minizip.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip" TYPE FILE FILES "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-debug/DCC-Miner/extlibs/elzip/extlibs/minizip/CMakeFiles/Export/lib/cmake/minizip/minizip-debug.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/minizip" TYPE FILE FILES
    "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-debug/DCC-Miner/extlibs/elzip/extlibs/minizip/minizip-config-version.cmake"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-debug/DCC-Miner/extlibs/elzip/extlibs/minizip/minizip-config.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz.h"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_os.h"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_crypt.h"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_strm.h"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_strm_buf.h"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_strm_mem.h"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_strm_split.h"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_strm_os.h"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_zip.h"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_zip_rw.h"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_strm_zlib.h"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/mz_compat.h"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/zip.h"
    "D:/Code/DC-Cryptocurrency/DCC-Miner/DCC-Miner/extlibs/elzip/extlibs/minizip/unzip.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "D:/Code/DC-Cryptocurrency/DCC-Miner/out/install/x64-debug/share/pkgconfig/minizip.pc")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "D:/Code/DC-Cryptocurrency/DCC-Miner/out/install/x64-debug/share/pkgconfig" TYPE FILE FILES "D:/Code/DC-Cryptocurrency/DCC-Miner/out/build/x64-debug/DCC-Miner/extlibs/elzip/extlibs/minizip/minizip.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.

endif()

