cmake_minimum_required(VERSION 3.5)
project(flatui LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_CXX_COMPILER "D:/Program/Qt/Tools/mingw1120_64/bin/g++.exe")
# set(CMAKE_PROJECT_INCLUDE_BEFORE "D:/Program/Qt/Tools/QtCreator/share/qtcreator/package-manager/auto-setup.cmake")
# set(CMAKE_PREFIX_PATH "D:/Program/Qt/6.4.0/mingw_64")

find_package(QT NAMES Qt6 REQUIRED Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

add_library(${PROJECT_NAME} STATIC FlatUI.cc)
set(${PROJECT_NAME}INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

target_link_libraries(flatui Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(flatui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
