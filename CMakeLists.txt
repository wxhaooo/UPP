cmake_minimum_required(VERSION 3.0)
project(UPP)

set(CMAKE_CXX_STANDARD 17)

set(DELEGATE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Delegate)

include_directories(${DELEGATE_INCLUDE_DIR})

add_subdirectory(${PROJECT_SOURCE_DIR}/Examples)