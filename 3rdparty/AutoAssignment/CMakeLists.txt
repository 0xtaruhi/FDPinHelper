cmake_minimum_required(VERSION 3.5)

project(AutoAssignment LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(verilog-parser)
include_directories(verilog-parser)

include_directories(include)

add_library(autoassignment STATIC src/VerilogPortsProbe.cc)
target_link_libraries(autoassignment verilogparser)
target_link_libraries(autoassignment Qt${QT_VERSION_MAJOR}::Core)
