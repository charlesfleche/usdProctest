cmake_minimum_required(VERSION 3.12)

project(usdProcTest)

include(CTest)

set(PXR_CONFIG_CMAKE ""
    CACHE
    STRING
    "Path to the pxrConfig.cmake file, located in the USD install folder by default."
)
include(${PXR_CONFIG_CMAKE})

add_subdirectory(src)