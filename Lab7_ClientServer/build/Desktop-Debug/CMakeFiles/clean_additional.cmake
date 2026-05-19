# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "qt_client\\CMakeFiles\\client_autogen.dir\\AutogenUsed.txt"
  "qt_client\\CMakeFiles\\client_autogen.dir\\ParseCache.txt"
  "qt_client\\client_autogen"
  )
endif()
