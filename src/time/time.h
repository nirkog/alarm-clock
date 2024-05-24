#pragma once

#include <stdint.h>

/*
 * @brief Configure the current time
 *
 * @param hour Current hour
 * @param minute Current minute
 * @param second Current second
 * */
void time__configure_time(
	uint8_t hour,
	uint8_t minute,
	uint8_t second
);

/*
 * @brief Configure the duration of an alarm
 *
 * @param duration The duration in seconds
 * */
void time__configure_alarm_duration(
	uint32_t duration
);

/*
 * @brief Configure the alarm time
 *
 * @param hour The alarm hour
 * @param minute The alarm minute
 * @param second The alarm second
 * */
void time__configure_alarm_time(
	uint8_t hour,
   	uint8_t minute,
	uint8_t second
);

/*
 * @brief Start counting
 * */
void time__start(void);

/*
 * @brief Stop counting
 * */
void time__stop(void);

/*
 * @brief Get the current time
 *
 * @param hour Current hour
 * @param minute Current minute
 * @param second Current second
 * */
void time__get_current_time(
	uint8_t* hour,
	uint8_t* minute,
	uint8_t* second
);

/*
 * @brief Check whether the alarm is currently on
 *
 * @return Is the alarm on
 * */
bool time__is_in_alarm(void);
