cmake_minimum_required(VERSION 3.21)

project(droppp LANGUAGES CXX)

find_package (Python3 COMPONENTS Interpreter)

add_subdirectory(droppp_lib)

add_executable(droppp test/example.cc test/meta/_gen.cc)
target_include_directories(droppp PUBLIC test/)

execute_process (COMMAND "${Python3_EXECUTABLE}" -m venv "venv")
set (ENV{VIRTUAL_ENV} "venv")
set (Python3_FIND_VIRTUALENV FIRST)
unset (Python3_EXECUTABLE)
find_package (Python3 COMPONENTS Interpreter Development)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/test/meta/_gen.cc COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/src/main.py ${CMAKE_CURRENT_SOURCE_DIR}/droppp_config.json)

target_link_libraries(droppp PUBLIC droppp_lib)