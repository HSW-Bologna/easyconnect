# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

add_definitions(-DI2C_DEVICES_STRUCT_TM_CONVERSION)
add_definitions(-DGEL_PARAMETER_CONFIGURATION_HEADER="gel_parameter_conf.h")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(easyconnect)

target_add_binary_data(easyconnect.elf "webapp/index.html.gzip" TEXT)
