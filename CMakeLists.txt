cmake_minimum_required(VERSION 3.15)
project(NucleoG070_ThreadX C ASM)

# Set language standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)

# Toolchain setup (ARM GCC)
# You can put this into a separate file like cmake/toolchain-arm-gcc.cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(ARM_TOOLCHAIN_PREFIX arm-none-eabi-)
set(CMAKE_C_COMPILER ${ARM_TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${ARM_TOOLCHAIN_PREFIX}gcc)
set(CMAKE_OBJCOPY ${ARM_TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${ARM_TOOLCHAIN_PREFIX}size)

# MCU and FPU settings
set(MCU_FLAGS "-mcpu=cortex-m0plus -mthumb -mfloat-abi=soft")

# Optimization and warnings
set(CMAKE_C_FLAGS "${MCU_FLAGS} -O2 -Wall -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -Wl,--gc-sections -Wl,-Map=${PROJECT_NAME}.map")

# Include external libraries
add_subdirectory(external)

# Application sources
set(SRC
    boards/nucleo-g070rb/startup_stm32g070xx.s
    boards/nucleo-g070rb/system_stm32g0xx.c
    src/main.c
)

# Create executable
add_executable(${PROJECT_NAME}.elf ${SRC})

# Link against HAL, CMSIS core, and ThreadX
target_link_libraries(${PROJECT_NAME}.elf
    stm32g0_hal
    cmsis_core
    threadx
)

# Include paths for application
target_include_directories(${PROJECT_NAME}.elf PRIVATE
    src
    boards/nucleo-g070rb
)

# Post-build: create HEX and BIN files, show size
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
)
