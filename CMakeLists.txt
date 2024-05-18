cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_FETCH_FROM_GIT on)

include(cmake/pico_sdk_import.cmake)

project(alarm)

pico_sdk_init()

# rest of your project
add_executable(alarm
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
)

target_link_libraries(alarm pico_stdlib)

pico_add_extra_outputs(alarm)
