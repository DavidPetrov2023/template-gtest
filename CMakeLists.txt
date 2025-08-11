cmake_minimum_required(VERSION 3.20)

# Project name and languages
project(nucleo-g070rb-blink C ASM)

# --- Toolchain (arm-none-eabi) ---
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# You can also use a toolchain file and pass -DCMAKE_TOOLCHAIN_FILE=...
set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY      arm-none-eabi-objcopy)
set(CMAKE_SIZE         arm-none-eabi-size)

# --- CPU & flags ---
set(CPU_OPTIONS "-mcpu=cortex-m0plus -mthumb")
set(COMMON_FLAGS "${CPU_OPTIONS} -Wall -Wextra -ffunction-sections -fdata-sections -g3 -O0")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS}")

# --- Sources ---
# Prefer explicit listing for reliability; add files here as you create them
set(BOARD_DIR ${CMAKE_SOURCE_DIR}/boards/nucleo-g070rb)
set(HAL_DIR   ${CMAKE_SOURCE_DIR}/external/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver)

set(SOURCES
    src/main.c

    # Board support files
    ${BOARD_DIR}/startup_stm32g070xx.s
    ${BOARD_DIR}/system_stm32g0xx.c
    ${BOARD_DIR}/stm32g0xx_it.c

    # Minimal HAL sources needed (GPIO + RCC + EXTI + Cortex + PWR for Vscale/PLL)
    ${HAL_DIR}/Src/stm32g0xx_hal.c
    ${HAL_DIR}/Src/stm32g0xx_hal_cortex.c
    ${HAL_DIR}/Src/stm32g0xx_hal_rcc.c
    ${HAL_DIR}/Src/stm32g0xx_hal_rcc_ex.c
    ${HAL_DIR}/Src/stm32g0xx_hal_gpio.c
    ${HAL_DIR}/Src/stm32g0xx_hal_exti.c
    ${HAL_DIR}/Src/stm32g0xx_hal_pwr.c
    ${HAL_DIR}/Src/stm32g0xx_hal_pwr_ex.c
    ${HAL_DIR}/Src/stm32g0xx_hal_tim.c
    ${HAL_DIR}/Src/stm32g0xx_hal_tim_ex.c    
)

add_executable(app.elf ${SOURCES})

# --- Preprocessor defines ---
target_compile_definitions(app.elf PRIVATE
    STM32G070xx
    USE_HAL_DRIVER
)

# --- Include directories (project + HAL/CMSIS) ---
target_include_directories(app.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/include/core            # your public headers (main.h, status.h)
    ${BOARD_DIR}                                # board headers (e.g., stm32g0xx_it.h, hal_conf)
    ${HAL_DIR}/Inc                              # HAL headers
    ${CMAKE_SOURCE_DIR}/external/STM32CubeG0/Drivers/CMSIS/Device/ST/STM32G0xx/Include
    ${CMAKE_SOURCE_DIR}/external/cmsis/CMSIS/Core/Include
)

# --- Linker script ---
target_link_options(app.elf PRIVATE
    -T ${BOARD_DIR}/STM32G070RBTX_FLASH.ld
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/app.map
)

# --- Post-build artifacts ---
add_custom_command(TARGET app.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} app.elf
    COMMAND ${CMAKE_OBJCOPY} -O ihex   app.elf app.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary app.elf app.bin
    COMMENT "Generating app.hex and app.bin"
)
