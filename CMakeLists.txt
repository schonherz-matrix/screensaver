cmake_minimum_required(VERSION 3.16)

project(
  screensaver
  DESCRIPTION "Animating shaders for Schönherz Mátrix"
  HOMEPAGE_URL "kszk.bme.hu"
  LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(LIBMUEB_AS_DLL)

find_package(
  Qt5
  COMPONENTS Widgets Network
  REQUIRED)

if(NOT DEFINED MATRIX_SIMPLE)
  find_package(libmueb 2.0 REQUIRED)
endif()

add_executable(
  ${PROJECT_NAME} WIN32
  src/glslsandboxanimator.h
  src/mainwindow.h
  src/openglanimator.h
  src/glslsandboxanimator.cpp
  src/main.cpp
  src/mainwindow.cpp
  src/openglanimator.cpp
  src/mainwindow.ui
  src/shaders.qrc)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Network
                                              libmueb::muebtransmitter)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT "${TARGETS_EXPORT_NAME}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
