cmake_minimum_required(VERSION 3.24)

set(PROJ_NAME "d1000_bios")
project(${PROJ_NAME})

set(CMAKE_C_STANDARD 11)
enable_language(C CXX ASM)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(UTIL_SEARCH_CMD which)

set(TOOLCHAIN_PREFIX arm-none-eabi-)

execute_process(
  COMMAND ${UTIL_SEARCH_CMD} ${TOOLCHAIN_PREFIX}gcc
  OUTPUT_VARIABLE BINUTILS_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

get_filename_component(ARM_TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)

set(CMAKE_OBJCOPY ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_SIZE_UTIL ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}size CACHE INTERNAL "size tool")

set(CMAKE_FIND_ROOT_PATH ${BINUTILS_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CM4_CORE_FLAGS -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
set(CM7_CORE_FLAGS -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard)

set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")

file(GLOB_RECURSE CM4_CORE_SRC ${CMAKE_SOURCE_DIR}/CM4/Core/Src/**.c)
file(GLOB_RECURSE DRIVER_FILES ${CMAKE_SOURCE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/**.c)
set(COMMON_INCLUDES
    ${CMAKE_SOURCE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Include
)

add_executable(${PROJ_NAME}_CM4
    ${CM4_CORE_SRC}
    ${DRIVER_FILES}
    ${CMAKE_SOURCE_DIR}/Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c
    ${CMAKE_SOURCE_DIR}/CM4/Core/Startup/startup_stm32h745bitx.s)

target_compile_definitions(${PROJ_NAME}_CM4 PRIVATE
    -DCORE_CM4
    -DUSE_HAL_DRIVER
    -DSTM32H745xx)

target_include_directories(${PROJ_NAME}_CM4 PRIVATE
    ${CMAKE_SOURCE_DIR}/CM4/Core/Inc
    ${COMMON_INCLUDES})

target_compile_options(${PROJ_NAME}_CM4 PRIVATE
    ${CM4_CORE_FLAGS}
    -fdata-sections
    -ffunction-sections
    -Wall
    -Wextra)

target_link_libraries(${PROJ_NAME}_CM4 PRIVATE c m)
target_link_options(${PROJ_NAME}_CM4 PRIVATE
    ${CM4_CORE_FLAGS}
    -specs=nano.specs
    -T${CMAKE_SOURCE_DIR}/CM4/STM32H745BITX_FLASH.ld
    -Wl,-Map=${PROJ_NAME}.map,--cref
    -Wl,--gc-sections)

add_custom_command(TARGET ${PROJ_NAME}_CM4
    POST_BUILD
    COMMAND ln -sf ${PROJ_NAME}_CM4 "${PROJ_NAME}_CM4.elf"
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJ_NAME}_CM4 ${PROJ_NAME}_CM4.hex ${CUSTOM_MAKE_HEX_FLAGS} 
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJ_NAME}_CM4 ${PROJ_NAME}_CM4.bin ${CUSTOM_MAKE_BIN_FLAGS}
)

file(GLOB_RECURSE CM7_CORE_SRC ${CMAKE_SOURCE_DIR}/CM7/Core/Src/**.c)
add_executable(${PROJ_NAME}_CM7
    ${CM7_CORE_SRC}
    ${DRIVER_FILES}
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/list.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/queue.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/timers.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    ${CMAKE_SOURCE_DIR}/Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c
    ${CMAKE_SOURCE_DIR}/CM7/Core/Startup/startup_stm32h745bitx.s)

target_compile_definitions(${PROJ_NAME}_CM7 PRIVATE
    -DCORE_CM7
    -DUSE_HAL_DRIVER
    -DSTM32H745xx)

target_include_directories(${PROJ_NAME}_CM7 PRIVATE
    ${CMAKE_SOURCE_DIR}/CM7/Core/Inc
    ${COMMON_INCLUDES}
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F)

target_compile_options(${PROJ_NAME}_CM7 PRIVATE
    ${CM7_CORE_FLAGS}
    -fdata-sections
    -ffunction-sections
    -Wall
    -Wextra)

target_link_libraries(${PROJ_NAME}_CM7 PRIVATE c m)
target_link_options(${PROJ_NAME}_CM7 PRIVATE
    ${CM7_CORE_FLAGS}
    -specs=nano.specs
    -T${CMAKE_SOURCE_DIR}/CM7/STM32H745BITX_FLASH.ld
    -Wl,-Map=${PROJ_NAME}.map,--cref
    -Wl,--gc-sections)

add_custom_command(TARGET ${PROJ_NAME}_CM7
    POST_BUILD
    COMMAND ln -sf ${PROJ_NAME}_CM7 "${PROJ_NAME}_CM7.elf"
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJ_NAME}_CM7 ${PROJ_NAME}_CM7.hex ${CUSTOM_MAKE_HEX_FLAGS} 
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJ_NAME}_CM7 ${PROJ_NAME}_CM7.bin ${CUSTOM_MAKE_BIN_FLAGS}
)