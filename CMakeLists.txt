cmake_minimum_required(VERSION 3.23)
project(cs8_final_project_test)

set(CMAKE_CXX_STANDARD 14)

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#   Grab all cpp files from includes folder
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
set(INCLUDES_FOLDER includes)
FILE(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}   "${INCLUDES_FOLDER}/*.cpp" )
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

add_executable(cs8_final_project_test main.cpp
        ${SOURCE_FILES})
