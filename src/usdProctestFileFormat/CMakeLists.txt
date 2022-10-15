SET(target usdProctest)

add_library(${target}
  MODULE
  fileFormat.cpp
  fileFormat.h
  plugInfo.json
)
target_link_libraries(usdProctest
  usdGeom
)
target_include_directories(usdProctest
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(usdProctest
  PROPERTIES
    PREFIX ""
)

# Making plugInfo.json available to tests

configure_file(plugInfo.json ${CMAKE_CURRENT_BINARY_DIR}/usdProctest/resources/plugInfo.json
  COPYONLY
)

install(
  TARGETS usdProctest
  LIBRARY DESTINATION .
)

install(
  FILES plugInfo.json
  DESTINATION usdProctest/resources
)

# add_subdirectory(testenv)
