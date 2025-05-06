set(LIB_NAME RTOS_Labs_common)
set (SRC_FILES 
  ADC.c
  ST7735.c
  OS.c
  UART0int.c
  Interpreter.c
  FIFOsimple.c
  eDisk.c
)

add_library(${LIB_NAME} STATIC ${SRC_FILES})

target_compile_options(${LIB_NAME} PUBLIC -g)

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

