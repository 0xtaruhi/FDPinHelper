cmake_minimum_required(VERSION 3.5)
project(FDPinHelper LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PROJECT_INCLUDE_BEFORE "D:/Program/Qt/Tools/QtCreator/share/qtcreator/package-manager/auto-setup.cmake")
set(CMAKE_PREFIX_PATH "D:/Program/Qt/6.4.0/mingw_64")

find_package(QT NAMES Qt6 REQUIRED Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

add_subdirectory(3rdparty/FlatUI)
add_subdirectory(3rdparty/AutoAssignment)

file(GLOB PROJECT_SOURCES
    "src/*.cc"
    "include/*.h"
)

qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets flatui autoassignment)
target_include_directories(${PROJECT_NAME} PRIVATE include 3rdparty/FlatUI 3rdparty/AutoAssignment/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS})
