cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_FETCH_FROM_GIT on)

include(cmake/pico_sdk_import.cmake)

project(alarm)

pico_sdk_init()

# rest of your project
add_executable(alarm
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
	${CMAKE_CURRENT_LIST_DIR}/src/seven_segment/seven_segment.c
	${CMAKE_CURRENT_LIST_DIR}/src/audio/audio.c
	${CMAKE_CURRENT_LIST_DIR}/src/time/time.c
	${CMAKE_CURRENT_LIST_DIR}/src/input/input.c
)

target_link_libraries(alarm
	pico_stdlib
	hardware_dma
	hardware_pio
	hardware_timer
	hardware_clocks
	hardware_pwm
)

include_directories(
	${CMAKE_CURRENT_LIST_DIR}/src/
)

pico_add_extra_outputs(alarm)
