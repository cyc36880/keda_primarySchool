cmake_minimum_required(VERSION 3.15)
project(main)

set(component_dir "../../main_board/myCode/component")
set(udcheck_src_dir ${component_dir}/udcheck)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(${udcheck_src_dir} ${CMAKE_CURRENT_BINARY_DIR}/udcheck_bin)
target_include_directories(udcheck PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(main main.c udc_init.c)
target_link_libraries(main udcheck)

