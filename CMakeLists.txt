cmake_minimum_required(VERSION 3.20)
project(TerrariumPolyOctave VERSION 1.0.0)

# Hardware and Firmware Configuration
set(FIRMWARE_NAME TerrariumPolyOctave)
set(STM32_CHIP "STM32H743VIT6")

# STM32CubeH7 Configuration
# User must provide the path to STM32CubeH7 repository
# or we attempt to find it in lib/stm32h7xx_hal_driver
if(NOT DEFINED STM32_CUBE_H7_PATH)
    set(STM32_CUBE_H7_PATH "${CMAKE_SOURCE_DIR}/lib/stm32h7xx_hal_driver")
    message(STATUS "STM32_CUBE_H7_PATH not defined, defaulting to: ${STM32_CUBE_H7_PATH}")
    message(STATUS "Please ensure the HAL driver is present there or set -DSTM32_CUBE_H7_PATH=/path/to/cube")
endif()

# Gather HAL Sources
file(GLOB HAL_SOURCES
    "${STM32_CUBE_H7_PATH}/Src/stm32h7xx_hal.c"
    "${STM32_CUBE_H7_PATH}/Src/stm32h7xx_hal_cortex.c"
    "${STM32_CUBE_H7_PATH}/Src/stm32h7xx_hal_dma.c"
    "${STM32_CUBE_H7_PATH}/Src/stm32h7xx_hal_gpio.c"
    "${STM32_CUBE_H7_PATH}/Src/stm32h7xx_hal_i2s.c"
    "${STM32_CUBE_H7_PATH}/Src/stm32h7xx_hal_i2s_ex.c"
    "${STM32_CUBE_H7_PATH}/Src/stm32h7xx_hal_pwr.c"
    "${STM32_CUBE_H7_PATH}/Src/stm32h7xx_hal_pwr_ex.c"
    "${STM32_CUBE_H7_PATH}/Src/stm32h7xx_hal_rcc.c"
    "${STM32_CUBE_H7_PATH}/Src/stm32h7xx_hal_rcc_ex.c"
    # Add other drivers as needed (UART, PCD for USB, etc.)
)

# Sources
set(FIRMWARE_SOURCES
    main.cpp
    hw_config.cpp
    usb_cdc_stub.cpp
    util/Terrarium.cpp
    util/Led.cpp
    ${HAL_SOURCES}
)

# Startup code
set(STARTUP_CODE startup_stm32h743xx.s)

# Include Directories
include_directories(
    .
    util
    lib/q/q_lib/include
    lib/q/infra/include
    lib/gcem/include
    ${STM32_CUBE_H7_PATH}/Inc
    ${STM32_CUBE_H7_PATH}/Inc/Legacy
)

# Add dependencies
add_subdirectory(lib/gcem)
option(Q_BUILD_EXAMPLES "build Q library examples" OFF)
option(Q_BUILD_TEST "build Q library tests" OFF)
option(Q_BUILD_IO "build Q IO library" OFF)
add_subdirectory(lib/q)

# Compiler Flags for Cortex-M7
set(MCU_FLAGS "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MCU_FLAGS} -std=c++20 -Wall -Wextra -Wno-volatile")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MCU_FLAGS} -std=c11 -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MCU_FLAGS} -T${CMAKE_SOURCE_DIR}/stm32h743.ld -Wl,-Map=${FIRMWARE_NAME}.map -Wl,--gc-sections")

# Executable
add_executable(${FIRMWARE_NAME} ${FIRMWARE_SOURCES} ${STARTUP_CODE})

# Link Libraries
target_link_libraries(${FIRMWARE_NAME} PUBLIC libq gcem)

# Definitions
target_compile_definitions(${FIRMWARE_NAME} PRIVATE
    STM32H743xx
    USE_HAL_DRIVER
)

# Helpers to generate bin/hex
add_custom_command(TARGET ${FIRMWARE_NAME} POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary ${FIRMWARE_NAME} ${FIRMWARE_NAME}.bin
    COMMAND arm-none-eabi-objcopy -O ihex ${FIRMWARE_NAME} ${FIRMWARE_NAME}.hex
    COMMAND arm-none-eabi-size ${FIRMWARE_NAME}
)
