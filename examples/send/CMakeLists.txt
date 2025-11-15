cmake_minimum_required(VERSION 3.16)

# Include ESP-IDF CMake helpers
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Add parent directory (twai-idf-can component) and components/ to search path
set(EXTRA_COMPONENT_DIRS ${CMAKE_SOURCE_DIR}/../.. ${CMAKE_SOURCE_DIR}/../../components)

# Project name
project(twai_send_example)

