set(CURRENT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${CURRENT_INCLUDE_DIR})

add_executable(DelegateExample ${CMAKE_CURRENT_SOURCE_DIR}/DelegateExample.cpp)