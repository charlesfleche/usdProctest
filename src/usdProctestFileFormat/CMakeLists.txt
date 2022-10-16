SET(target usdProctestFileFormat)

add_library(${target}
  MODULE
  fileFormat.cpp
  fileFormat.h
  plugInfo.json
)
target_link_libraries(usdProctestFileFormat
  usdGeom
)
target_include_directories(usdProctestFileFormat
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(usdProctestFileFormat
  PROPERTIES
    PREFIX ""
)

# Making plugInfo.json available to tests

configure_file(plugInfo.json ${CMAKE_CURRENT_BINARY_DIR}/usdProctestFileFormat/resources/plugInfo.json
  COPYONLY
)

install(
  TARGETS usdProctestFileFormat
  LIBRARY DESTINATION .
)

install(
  FILES plugInfo.json
  DESTINATION usdProctestFileFormat/resources
)
