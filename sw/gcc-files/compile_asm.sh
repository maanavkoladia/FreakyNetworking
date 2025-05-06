arm-none-eabi-as -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -g -o objs/add.o ../RTOS_Lab2_RTOSkernel/add.s

arm-none-eabi-as -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -g -mthumb -o objs/osasm_gcc.o ../RTOS_Labs_common/osasm_gcc.s
