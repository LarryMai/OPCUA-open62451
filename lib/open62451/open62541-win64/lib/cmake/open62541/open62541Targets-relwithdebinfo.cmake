#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "open62541::open62541" for configuration "RelWithDebInfo"
set_property(TARGET open62541::open62541 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(open62541::open62541 PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libopen62541.dll.a"
  IMPORTED_LOCATION_RELWITHDEBINFO "/home/travis/build/open62541/open62541/open62541-win64/libopen62541.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS open62541::open62541 )
list(APPEND _IMPORT_CHECK_FILES_FOR_open62541::open62541 "${_IMPORT_PREFIX}/lib/libopen62541.dll.a" "/home/travis/build/open62541/open62541/open62541-win64/libopen62541.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
