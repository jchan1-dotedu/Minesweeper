cmake_minimum_required(VERSION 3.16)

project(minesweeper)

set(SFML_DIR "C:/Users/fooba/Documents/j's stuff/UF/2025 Fall/COP3504c/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit/SFML-2.5.1/lib/cmake/SFML")

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

file(GLOB SOURCES "*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system sfml-audio)   