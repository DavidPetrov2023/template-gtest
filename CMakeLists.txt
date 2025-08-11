cmake_minimum_required(VERSION 3.20)

project(nucleo-g070rb-blink-threadx C ASM)

# --- Toolchain (ARM GNU) ---
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
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

# --- Paths ---
set(BOARD_DIR        ${CMAKE_SOURCE_DIR}/boards/nucleo-g070rb)
set(HAL_DIR          ${CMAKE_SOURCE_DIR}/external/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver)
set(ST_DEVICE_DIR    ${CMAKE_SOURCE_DIR}/external/STM32CubeG0/Drivers/CMSIS/Device/ST/STM32G0xx)
set(CMSIS_CORE_DIR   ${CMAKE_SOURCE_DIR}/external/cmsis/CMSIS/Core)
set(THREADX_DIR      ${CMAKE_SOURCE_DIR}/external/threadx)
set(TX_PORT_DIR      ${THREADX_DIR}/ports/cortex_m0/gnu)   # M0 port (funguje i pro M0+)

# --- ThreadX sources (vše z common + port M0/gnu) ---
file(GLOB TX_COMMON_SRC CONFIGURE_DEPENDS ${THREADX_DIR}/common/src/*.c)
file(GLOB TX_PORT_C     CONFIGURE_DEPENDS ${TX_PORT_DIR}/src/*.c)
file(GLOB TX_PORT_S     CONFIGURE_DEPENDS ${TX_PORT_DIR}/src/*.[sS])

# --- App / HAL sources ---
set(SOURCES
    src/main.c

    # Board / system
    ${BOARD_DIR}/startup_stm32g070xx.s
    ${BOARD_DIR}/system_stm32g0xx.c
    ${BOARD_DIR}/stm32g0xx_it.c
    ${BOARD_DIR}/tx_initialize_low_level.c   # <- naše C implementace _tx_initialize_low_level
    ${BOARD_DIR}/syscalls.c
    ${BOARD_DIR}/tx_port_hook.c

    # HAL (rozšiř dle potřeby periferií)
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

    # ThreadX
    ${TX_COMMON_SRC}
    ${TX_PORT_C}
    ${TX_PORT_S}
)

add_executable(app.elf ${SOURCES})

# --- Preprocessor defines ---
target_compile_definitions(app.elf PRIVATE
    STM32G070xx
    USE_HAL_DRIVER
    TX_INCLUDE_USER_DEFINE_FILE     # očekává tx_user.h v projektových includes
)

# --- Include directories ---
target_include_directories(app.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/include/core          # main.h, tx_user.h, ...
    ${BOARD_DIR}                              # stm32g0xx_hal_conf.h atd.
    ${HAL_DIR}/Inc
    ${ST_DEVICE_DIR}/Include                  # stm32g0xx.h
    ${CMSIS_CORE_DIR}/Include                 # CMSIS Core
    ${THREADX_DIR}/common/inc
    ${TX_PORT_DIR}/inc
)

# --- Linker options ---
target_link_options(app.elf PRIVATE
    -T ${BOARD_DIR}/STM32G070RBTX_FLASH.ld
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/app.map
    -specs=nosys.specs
    -specs=nano.specs
)

# --- Post-build artifacts ---
add_custom_command(TARGET app.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} app.elf
    COMMAND ${CMAKE_OBJCOPY} -O ihex   app.elf app.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary app.elf app.bin
    COMMENT "Generating app.hex and app.bin"
)
