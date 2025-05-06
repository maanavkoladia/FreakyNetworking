set(LIB_NAME ValvanoWare)

set(SRC_FILES 
  Timer0A.c
  Timer1A.c
  Timer2A.c
  Timer3A.c
  Timer4A.c
  Timer5A.c
  PLL.c
  LaunchPad.c
  UART.c
  SysTick.c
)

add_library(${LIB_NAME} STATIC ${SRC_FILES})

target_compile_options(${LIB_NAME} PUBLIC -g)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})