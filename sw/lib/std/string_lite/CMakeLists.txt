set(LIB_NAME string_lite)

add_library(${LIB_NAME} STATIC string_lite.c)

#target_compile_options(${LIB_NAME} PUBLIC -Wall -g -O0)

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

