cmake_minimum_required(VERSION 3.20)

# Project name and language
project(nucleo-g070rb-blink C ASM)

# ARM toolchain (adjust path if needed)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# You can set your own toolchain file instead of hardcoding:
# set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

# CPU and FPU settings
set(CPU_OPTIONS "-mcpu=cortex-m0plus -mthumb")

# Common compiler flags
set(COMMON_FLAGS "${CPU_OPTIONS} -Wall -Wextra -ffunction-sections -fdata-sections -g3 -O0")

set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu11")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS}")

# Sources
set(SOURCES
    src/main.c
    boards/nucleo-g070rb/startup_stm32g070xx.s
    boards/nucleo-g070rb/system_stm32g0xx.c

    # HAL sources (minimum pro tvůj clock + EXTI)
    external/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal.c
    external/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_cortex.c
    external/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_rcc.c
    external/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_rcc_ex.c 
    external/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_gpio.c
    external/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_exti.c
    external/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_pwr.c     
    external/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_pwr_ex.c  
)

# Include directories
include_directories(
    boards/nucleo-g070rb
    external/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Inc
    external/STM32CubeG0/Drivers/CMSIS/Device/ST/STM32G0xx/Include
    external/cmsis/CMSIS/Core/Include
)

# Create executable
add_executable(app.elf ${SOURCES})

# Preprocessor definitions
target_compile_definitions(app.elf PRIVATE
    STM32G070xx
    USE_HAL_DRIVER
)

# Linker script
target_link_options(app.elf PRIVATE
    -T ${CMAKE_SOURCE_DIR}/boards/nucleo-g070rb/STM32G070RBTX_FLASH.ld
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/app.map
)

# Post-build commands
add_custom_command(TARGET app.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} app.elf
    COMMAND ${CMAKE_OBJCOPY} -O ihex   app.elf app.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary app.elf app.bin
)
