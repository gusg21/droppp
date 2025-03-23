cmake_minimum_required(VERSION 3.21)

project(droppp LANGUAGES CXX)

add_subdirectory(droppp_lib)

execute_process (COMMAND "${Python3_EXECUTABLE}" -m venv "venv")
set (ENV{VIRTUAL_ENV} "venv")
set (Python3_FIND_VIRTUALENV FIRST)
unset (Python3_EXECUTABLE)
find_package (Python3 COMPONENTS Interpreter Development)
add_custom_target(DropppRun ALL COMMAND python3 src/main.py droppp_config.json WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(droppp test/example.cc test/meta/_gen.cc)
target_include_directories(droppp PUBLIC test/)
target_link_libraries(droppp PUBLIC droppp_lib)
add_dependencies(droppp DropppRun)