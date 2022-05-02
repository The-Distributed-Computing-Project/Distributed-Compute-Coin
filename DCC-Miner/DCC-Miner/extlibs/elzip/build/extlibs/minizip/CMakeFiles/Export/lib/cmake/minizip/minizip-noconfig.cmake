#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "MINIZIP::minizip" for configuration ""
set_property(TARGET MINIZIP::minizip APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(MINIZIP::minizip PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libminizip.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS MINIZIP::minizip )
list(APPEND _IMPORT_CHECK_FILES_FOR_MINIZIP::minizip "${_IMPORT_PREFIX}/lib/libminizip.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
