#pragma once

#include <stdint.h>

#define MAX_DIGITS (10)

typedef struct seven_segment__config_s {
	uint32_t a_pin;
	uint32_t b_pin;
	uint32_t c_pin;
	uint32_t d_pin;
	uint32_t e_pin;
	uint32_t f_pin;
	uint32_t g_pin;
	uint32_t dp_pin;
	uint32_t digit_pins[MAX_DIGITS];
	uint8_t digit_count;
} seven_segment__config_t;

typedef struct seven_segment__driver_s {
	seven_segment__config_t config;
	uint8_t current_digit;
	uint8_t digit_values[MAX_DIGITS];
} seven_segment__driver_t;

/*
 * @brief Init a driver struct
 *
 * @param driver The driver struct
 * */
void seven_segment__init_driver(
	seven_segment__driver_t* driver
);

/*
 * @brief Configure the output pins of the display
 *
 * @param driver The driver struct
 * */
void seven_segment__configure_pins(
	seven_segment__driver_t* driver
);

/*
 * @brief Display the current state of the display, should be called periodically
 *
 * @param driver The display driver
 * */
void seven_segment__display(
	seven_segment__driver_t* driver
);

/*
 * @brief Set the value of a specific digit
 *
 * @param driver The display driver
 * @param index Index of the digit
 * @param value The value of the digit (should be between 0 and 9)
 * @param dp Should display decimal point
 * */
void seven_segment__set_digit_value(
	seven_segment__driver_t* driver,
	uint8_t index,
	uint8_t value,
	bool dp
);

/*
 * @brief Set the display digit values to some number representation
 *
 * @param driver The driver struct
 * @param value The numeric value
 * */
void seven_segment__set_number_value(
	seven_segment__driver_t* driver,
	uint32_t value
);

/*
 * @brief Set the value of a decimal point in a specific index
 *
 * @param driver The driver struct
 * @param index The decimal point index
 * @param value New value of the decimal point
 * */
void seven_segment__set_decimal_point_value(
	seven_segment__driver_t* driver,
	uint8_t index,
	bool value
);
