# Checkers for the Gamebuino AKA, an ESP-IDF 5.5 project. With ESP-IDF exported:
#   idf.py build
# gives build/checkers.bin, which goes on the SD card as CHECKERS/firmware.bin.
#
# The official Gamebuino AKA library (https://github.com/jmp42/Gamebuino_AKA_lib) is used where
# it is checked out, it is not copied here: point AKA_LIB_DIR at it, with -DAKA_LIB_DIR=<folder>
# or the AKA_LIB_DIR environment variable, when it is not in C:/github/Gamebuino_AKA_lib.
cmake_minimum_required(VERSION 3.16)

if(NOT AKA_LIB_DIR AND DEFINED ENV{AKA_LIB_DIR})
    set(AKA_LIB_DIR "$ENV{AKA_LIB_DIR}")
endif()
if(NOT AKA_LIB_DIR)
    set(AKA_LIB_DIR "C:/github/Gamebuino_AKA_lib")
endif()
set(AKA_LIB_DIR "${AKA_LIB_DIR}" CACHE PATH "a checkout of Gamebuino_AKA_lib")
if(NOT EXISTS "${AKA_LIB_DIR}/components/gamebuino/include_lib/gamebuino.h")
    message(FATAL_ERROR "the Gamebuino AKA library was not found in ${AKA_LIB_DIR}, "
                        "pass -DAKA_LIB_DIR=<folder> or set the AKA_LIB_DIR environment variable")
endif()
set(EXTRA_COMPONENT_DIRS "${AKA_LIB_DIR}/components/gamebuino")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(checkers)

# the firmware also goes into the SD card folder, ready to be copied to the card
add_custom_target(sd_firmware ALL
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/checkers.bin" "${CMAKE_SOURCE_DIR}/SD_files/CHECKERS/firmware.bin"
    COMMENT "Copying checkers.bin to SD_files/CHECKERS/firmware.bin")
add_dependencies(sd_firmware gen_project_binary)
