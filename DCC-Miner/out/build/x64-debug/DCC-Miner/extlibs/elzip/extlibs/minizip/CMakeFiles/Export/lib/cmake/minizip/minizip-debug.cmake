#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "MINIZIP::minizip" for configuration "Debug"
set_property(TARGET MINIZIP::minizip APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(MINIZIP::minizip PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/libminizip.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/libminizip.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS MINIZIP::minizip )
list(APPEND _IMPORT_CHECK_FILES_FOR_MINIZIP::minizip "${_IMPORT_PREFIX}/lib/libminizip.lib" "${_IMPORT_PREFIX}/bin/libminizip.dll" )

# Import target "MINIZIP::zlib" for configuration "Debug"
set_property(TARGET MINIZIP::zlib APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(MINIZIP::zlib PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/zlib.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/zlib.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS MINIZIP::zlib )
list(APPEND _IMPORT_CHECK_FILES_FOR_MINIZIP::zlib "${_IMPORT_PREFIX}/lib/zlib.lib" "${_IMPORT_PREFIX}/bin/zlib.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
