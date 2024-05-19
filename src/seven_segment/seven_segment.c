#include <stdbool.h>
#include <string.h>

#include "pico/stdlib.h"

#include "seven_segment.h"

#define DP_BIT_INDEX (7)
#define DP_MASK (1 << DP_BIT_INDEX)

bool g_digit_mappings[10][7] = {
	{0, 0, 0, 0, 0, 0, 1}, // 0
	{1, 0, 0, 1, 1, 1, 1}, // 1
	{0, 0, 1, 0, 0, 1, 0}, // 2
	{0, 0, 0, 0, 1, 1, 0}, // 3
	{1, 0, 0, 1, 1, 0, 0}, // 4
	{0, 1, 0, 0, 1, 0, 0}, // 5
	{0, 1, 0, 0, 0, 0, 0}, // 6
	{0, 0, 0, 1, 1, 1, 1}, // 7
	{0, 0, 0, 0, 0, 0, 0}, // 8
	{0, 0, 0, 0, 1, 0, 0}, // 9
};

void seven_segment__init_driver(seven_segment__driver_t* driver) {
	uint8_t i;

	memset((char*)&(driver->config), 0, sizeof(driver->config));

	driver->current_digit = 0;

	for (i = 0; i < MAX_DIGITS; i++) {
		driver->digit_values[i] = 0;
	}
}

void seven_segment__configure_pins(
	seven_segment__driver_t* driver
) {
	uint8_t i;

	gpio_init(driver->config.a_pin);
	gpio_set_dir(driver->config.a_pin, GPIO_OUT);
	gpio_init(driver->config.b_pin);
	gpio_set_dir(driver->config.b_pin, GPIO_OUT);
	gpio_init(driver->config.c_pin);
	gpio_set_dir(driver->config.c_pin, GPIO_OUT);
	gpio_init(driver->config.d_pin);
	gpio_set_dir(driver->config.d_pin, GPIO_OUT);
	gpio_init(driver->config.e_pin);
	gpio_set_dir(driver->config.e_pin, GPIO_OUT);
	gpio_init(driver->config.f_pin);
	gpio_set_dir(driver->config.f_pin, GPIO_OUT);
	gpio_init(driver->config.g_pin);
	gpio_set_dir(driver->config.g_pin, GPIO_OUT);
	gpio_init(driver->config.dp_pin);
	gpio_set_dir(driver->config.dp_pin, GPIO_OUT);

	for (i = 0; i < driver->config.digit_count; i++) {
		gpio_init(driver->config.digit_pins[i]);
		gpio_set_dir(driver->config.digit_pins[i], GPIO_OUT);
	}
}

void seven_segment__display(seven_segment__driver_t* driver) {
	uint32_t pin = driver->config.digit_pins[driver->current_digit]; 	
	uint8_t digit_value = driver->digit_values[driver->current_digit];
	bool* mapping;
	uint8_t i;

	mapping = g_digit_mappings[digit_value & ~DP_MASK];

	gpio_put(driver->config.a_pin, mapping[0]);
	gpio_put(driver->config.b_pin, mapping[1]);
	gpio_put(driver->config.c_pin, mapping[2]);
	gpio_put(driver->config.d_pin, mapping[3]);
	gpio_put(driver->config.e_pin, mapping[4]);
	gpio_put(driver->config.f_pin, mapping[5]);
	gpio_put(driver->config.g_pin, mapping[6]);

	gpio_put(driver->config.dp_pin, !(digit_value & DP_MASK));

	for (i = 0; i < driver->config.digit_count; i++) {
		if (i == driver->current_digit) {
			gpio_put(driver->config.digit_pins[i], true);
		} else {
			gpio_put(driver->config.digit_pins[i], false);
		}
	}

	driver->current_digit = (driver->current_digit + 1) % driver->config.digit_count;
}

void seven_segment__set_digit_value(
	seven_segment__driver_t* driver,
	uint8_t index,
	uint8_t value,
	bool dp
) {
	if (value > 9) {
		return;
	}

	if (dp) {
		value |= DP_MASK;
	}

	driver->digit_values[index] = value;
}

void seven_segment__set_number_value(
	seven_segment__driver_t* driver,
	uint32_t value
) {
	uint8_t i;

	for (i = driver->config.digit_count; i > 0;  i--) {
		seven_segment__set_digit_value(driver, i - 1, value % 10, false);
		value = value / 10;
	}
}

void seven_segment__set_decimal_point_value(
	seven_segment__driver_t* driver,
	uint8_t index,
	bool value
) {
	if (value) {
		driver->digit_values[index] |= DP_MASK;
	} else {
		driver->digit_values[index] &= ~DP_MASK;
	}
}
