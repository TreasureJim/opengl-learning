cmake_minimum_required(VERSION 3.26)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # does not produce the json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") # works

project(opengl_test VERSION 1.0)

file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include ${OPENGL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

add_subdirectory(libs)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 GL)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
