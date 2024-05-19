#include "pico/stdlib.h"

#include "time.h"

typedef struct timestamp_s {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} timestamp_t;

timestamp_t g_current_timestamp = { 0 };
timestamp_t g_alarm_timestamp = { 0 };

uint32_t g_alarm_duration = 0;
uint32_t g_alarm_second_counter = 0;
bool g_in_alarm = false;

bool g_running = false;

repeating_timer_t g_timer;

bool timer_callback(repeating_timer_t *rt);

void time__configure_time(
	uint8_t hour,
	uint8_t minute,
	uint8_t second
) {
	if ((hour > 23) || (minute > 59) || (second > 59)) {
		return;
	}

	g_current_timestamp.hour = hour;
	g_current_timestamp.minute = minute;
	g_current_timestamp.second = second;
}

void time__configure_alarm_duration(
	uint32_t duration
) {
	g_alarm_duration = duration;
}

void time__configure_alarm_time(
	uint8_t hour,
   	uint8_t minute,
	uint8_t second
) {
	if ((hour > 23) || (minute > 59) || (second > 59)) {
		return;
	}

	g_alarm_timestamp.hour = hour;
	g_alarm_timestamp.minute = minute;
	g_alarm_timestamp.second = second;
}

void time__start(void) {
	if (g_running) return;

	add_repeating_timer_us(-1000000, timer_callback, NULL, &g_timer);
	g_running = true;
}

void time__stop(void) {
	if (!g_running) return;

	cancel_repeating_timer(&g_timer);
	g_running = false;
}

void time__get_current_time(
	uint8_t* hour,
	uint8_t* minute,
	uint8_t* second
) {
	*hour = g_current_timestamp.hour;
	*minute = g_current_timestamp.minute;
	*second = g_current_timestamp.second;
}

bool timer_callback(repeating_timer_t *rt) {
	g_current_timestamp.second++;

	if (g_current_timestamp.second >= 60) {
		g_current_timestamp.second = 0;

		g_current_timestamp.minute++;

		if (g_current_timestamp.minute >= 60) {
			g_current_timestamp.minute = 0;

			g_current_timestamp.hour++;

			if (g_current_timestamp.hour >= 24) {
				g_current_timestamp.hour = 0;
			}
		}
	}

	if (g_in_alarm) {
		g_alarm_second_counter++;

		if (g_alarm_second_counter == g_alarm_duration) {
			g_in_alarm = false;
		}
	}

	if ((g_current_timestamp.hour == g_alarm_timestamp.hour) &&
		(g_current_timestamp.minute == g_alarm_timestamp.minute) &&
		(g_current_timestamp.second == g_alarm_timestamp.second)) {
		g_alarm_second_counter = 0;
		g_in_alarm = true;
	}

	return true;
}

bool time__is_in_alarm(void) {
	return g_in_alarm;
}
