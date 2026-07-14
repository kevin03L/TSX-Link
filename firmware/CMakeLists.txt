cmake_minimum_required(VERSION 3.16)

project(tsx-link-firmware C CXX ASM)

set(EXTRA_COMPONENT_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/components
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project_info(PROJECT_VER "0.1.0")
