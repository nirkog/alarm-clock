#pragma once

#include <stdint.h>

#include "common/common.h"

typedef enum input__state_e {
	STATE__NORMAL,
	STATE__TIME_SET,
	STATE__ALARM_SET
} input__state_t;

typedef struct input__config_s {
	uint8_t digit_button_count;
	uint32_t digit_button_pins[MAX_DIGITS];
	uint32_t set_time_button_pin;
	uint32_t set_alarm_button_pin;
} input__config_t;

typedef struct input__handler_s {
	input__config_t config;
	uint8_t current_digit_in_time_set;
	uint8_t set_time_digit_values[MAX_DIGITS];
} input__handler_t;

/*
 * @brief Initialize inpute GPIO according to a configuration
 *
 * @param config Input configuration struct
 * */
void input__init(
	input__config_t* config
);

/*
 * @brief Get the current input state
 * */
input__state_t input__get_state(void);

/*
 * @brief If in set time mode, get current value
 * */
uint32_t input__get_set_time_value(void);
