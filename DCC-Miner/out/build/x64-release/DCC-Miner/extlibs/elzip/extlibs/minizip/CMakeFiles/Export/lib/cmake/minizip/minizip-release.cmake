#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "MINIZIP::minizip" for configuration "Release"
set_property(TARGET MINIZIP::minizip APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(MINIZIP::minizip PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libminizip.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS MINIZIP::minizip )
list(APPEND _IMPORT_CHECK_FILES_FOR_MINIZIP::minizip "${_IMPORT_PREFIX}/lib/libminizip.lib" )

# Import target "MINIZIP::zlibstatic" for configuration "Release"
set_property(TARGET MINIZIP::zlibstatic APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(MINIZIP::zlibstatic PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/zlibstatic.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS MINIZIP::zlibstatic )
list(APPEND _IMPORT_CHECK_FILES_FOR_MINIZIP::zlibstatic "${_IMPORT_PREFIX}/lib/zlibstatic.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
