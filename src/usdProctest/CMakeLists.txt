add_library(usdProcTest
  MODULE
  api.h
  generatedSchema.usda
  myProcMesh.cpp
  myProcMesh.h
  plugInfo.json
  tokens.cpp
  tokens.h
  # wrapMyProcMesh.cpp
  # wrapTokens.cpp
)
target_link_libraries(usdProcTest
  usdGeom
)
target_include_directories(usdProcTest
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(usdProcTest
  PROPERTIES
    PREFIX ""
)

# Making plugInfo.json available to tests

configure_file(plugInfo.json ${CMAKE_CURRENT_BINARY_DIR}/usdProcTest/resources/plugInfo.json
  COPYONLY
)

install(
  TARGETS usdProcTest
  LIBRARY DESTINATION .
)

install(
  FILES plugInfo.json
  DESTINATION usdProcTest/resources
)

# add_subdirectory(testenv)
