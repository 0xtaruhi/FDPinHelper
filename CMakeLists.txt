cmake_minimum_required(VERSION 3.5)
project(FDPinHelper VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)

# macOS
if(APPLE)
    set(CMAKE_PROJECT_INCLUDE_BEFORE "${QT_ROOT_DIR}/Qt Creator.app/Contents/Resources/package-manager/auto-setup.cmake")
else()
    set(CMAKE_PROJECT_INCLUDE_BEFORE "${QT_ROOT_DIR}/Tools/QtCreator/share/qtcreator/package-manager/auto-setup.cmake")
endif()

set(CMAKE_PREFIX_PATH "${QT_ROOT_DIR}/6.4.0/macos")

find_package(QT NAMES Qt6 REQUIRED Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

add_subdirectory(3rdparty/FlatUI)

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

configure_file(${CMAKE_SOURCE_DIR}/include/Config.h.in ${CMAKE_SOURCE_DIR}/include/Config.h)

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE mac_icon.icns)
    set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon/mac_icon.icns")
    set_source_files_properties(${app_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION MACOSX_BUNDLE ${PROJECT_SOURCES} ${QRC_FILES} ${app_icon_macos})
else()
    qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES} ${QRC_FILES})
endif()

# qt_add_translations(${PROJECT_NAME} TS_FILES ${TS_FILES})
qt_add_lupdate(${PROJECT_NAME} TS_FILES ${TS_FILES})
qt_add_lrelease(${PROJECT_NAME} TS_FILES ${TS_FILES} QM_FILES_OUTPUT_VARIABLE QM_FILES)
qt_add_resources(${PROJECT_NAME} "translations" PREFIX "/translations" BASE ${CMAKE_CURRENT_BINARY_DIR} FILES ${QM_FILES})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets flatui)
target_include_directories(${PROJECT_NAME} PRIVATE include 3rdparty/FlatUI)
target_include_directories(${PROJECT_NAME} PRIVATE ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)

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
