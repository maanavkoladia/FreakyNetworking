cmake_minimum_required(VERSION 3.31)

# Set the project name
project(Networking LANGUAGES C ASM)

enable_language(ASM)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Specify the cross-compiler
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-as)

# Set the CPU and FPU options
set(CPU_FLAGS "-mcpu=cortex-m4")
set(FPU_FLAGS "-mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# Include Newlib directories for standard C library headers (stdio.h, etc.)


# Set the compiler flags
# Add your custom flags safely
add_compile_options(
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -mthumb
    -std=c99
    #-Wall
    #-Wextra
    -w
    -ffunction-sections
    -fdata-sections
    -O0
    -g
    -MD
    -Dgcc
    -DPART_TM4C123GH6PM
    -DTARGET_IS_BLIZZARD_RA1
)

set(CMAKE_ASM_FLAGS "-mcpu=cortex-m4 -mthumb -g")
#set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}")

# Set the linker script
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/gcc-files/tm4c123gh6pm.ld)

# Set the linker flags to link Newlib
set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} ${FPU_FLAGS}  -Wl,--gc-sections --entry ResetISR -T ${LINKER_SCRIPT}")
#set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} ${FPU_FLAGS} -mthumb -Wl,--gc-sections -T ${LINKER_SCRIPT} -lc -specs=nosys.specs -specs=nano.specs")
#set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} ${FPU_FLAGS} -mthumb -Wl,--gc-sections -T ${LINKER_SCRIPT} -lc -specs=nosys.specs -specs=nano.specs -Wl,--undefined=_write")
#set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} ${FPU_FLAGS} -mthumb -Wl,--gc-sections -T ${LINKER_SCRIPT}")


# Include directories
add_subdirectory(inc)
add_subdirectory(nm)
add_subdirectory(driverlib)
#add_subdirectory(RTOS_Labs_common)
add_subdirectory(lib/std/stdio_lite)
add_subdirectory(lib/std/string_lite)
#add_subdirectory(lib/std/printf)

# Define the source files
set(SRC_FILES
    gcc-files/startup_gcc.c
#gcc-files/syscalls.c

    src/main.c
    src/Networking.c
    src/SensorIn.c
#lib/std/stdio_lite/stdio_lite.h
#    inc/Timer0A.c
#    inc/Timer1A.c
#    inc/Timer2A.c
#    inc/Timer3A.c
#    inc/Timer4A.c
#    inc/Timer5A.c
#    inc/PLL.c
#    inc/LaunchPad.c
#    inc/UART.c
#    inc/SysTick.c

#    nm/bsp/nm_bsp.c
#    nm/bus_wrapper/nm_bus_wrapper.c
#    nm/common/nm_common.c
#
#    nm/driver/source/m2m_wifi.c
#    nm/driver/source/m2m_hif.c
#    nm/driver/source/nmdrv.c
#    nm/driver/source/nmasic.c
#    nm/driver/source/nmbus.c
#    nm/driver/source/nmspi.c
#    nm/driver/source/spi_flash.c

#    lib/std/string_lite/string_lite.c
#    lib/std/stdio_lite/stdio_lite.c

    # nm/nm_bsp_tm4c.c
    # nm/nm_bus_wrapper_tm4c.c
    # nm/nmbus.c

    #stm dsp files
)

# Create the executable
add_executable(exe.elf ${SRC_FILES})

# Link necessary libraries
target_link_libraries(exe.elf PRIVATE
    -Wl,--start-group
    ValvanoWare
    nm
    driverlib
#    RTOS_Labs_common
    stdio_lite
    string_lite
    #printf
    -lc        # Link the C standard library (Newlib)
    #-lnosys    # Link system call stubs
    -Wl,--end-group
)

target_include_directories(exe.elf PRIVATE
    src
    inc
    nm
    lib/std/stdio_lite
    lib/std/string_lite
    #RTOS_Labs_common
)


## Convert the ELF to BIN
#add_custom_command(TARGET exe.elf POST_BUILD
#    COMMAND arm-none-eabi-objcopy -O binary ${CMAKE_BINARY_DIR}/exe.elf ${CMAKE_BINARY_DIR}/exe.bin
#    COMMENT "Converting ELF to BIN"
#)

## Clean up object and dependency files after build
#add_custom_command(TARGET exe.elf POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/*.o ${CMAKE_BINARY_DIR}/*.d
#    COMMENT "Removing object and dependency files"
#)

