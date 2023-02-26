cmake_minimum_required(VERSION 3.5)
project(FDPinHelper VERSION 0.1 LANGUAGES CXX)

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

set(QRC_FILES
    "assets/res.qrc"
)

set(TS_LOCATION ${CMAKE_SOURCE_DIR}/assets/translation)
set(SUPPORT_LANGUAGE zh_CN)

foreach(LANG ${SUPPORT_LANGUAGE})
    set(TS_FILES ${TS_FILES} ${TS_LOCATION}/${PROJECT_NAME}_${LANG}.ts)
endforeach()

qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${QRC_FILES}
)

# qt_add_translations(${PROJECT_NAME} TS_FILES ${TS_FILES})
qt_add_lupdate(${PROJECT_NAME} TS_FILES ${TS_FILES})
qt_add_lrelease(${PROJECT_NAME} TS_FILES ${TS_FILES} QM_FILES_OUTPUT_VARIABLE QM_FILES)
qt_add_resources(${PROJECT_NAME} "translations" PREFIX "/translations" BASE ${CMAKE_CURRENT_BINARY_DIR} FILES ${QM_FILES})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets flatui autoassignment)
target_include_directories(${PROJECT_NAME} PRIVATE include 3rdparty/FlatUI 3rdparty/AutoAssignment/include 3rdparty/AutoAssignment/verilog-parser)
target_include_directories(${PROJECT_NAME} PRIVATE ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER fudan.edu.cn
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
