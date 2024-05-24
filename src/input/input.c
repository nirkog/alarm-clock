#include <string.h>

#include "pico/stdlib.h"

#include "time/time.h"
#include "input.h"

#define CHANGE_STATE_BUTTON_COOLDOWN_DURATION_US (500 * 1000)
#define CHANGE_DIGIT_BUTTON_COOLDOWN_DURATION_US (100 * 1000)
#define GPIO_COUNT (32)

input__state_t g_state = STATE__NORMAL;
input__handler_t g_handler;

bool g_in_interrupt = false;

uint64_t g_cooldown_start = 0;
uint32_t g_cooldown_gpio = 0;

uint32_t g_gpio_cooldowns[GPIO_COUNT] = { 0 };

void gpio_callback(uint gpio, uint32_t events);

void input__init(
	input__config_t* config
) {
	uint8_t i;

	memset((char*)&g_handler, 0, sizeof(g_handler));
	memcpy((void*)(&g_handler.config), config, sizeof(g_handler.config));

	gpio_set_irq_enabled_with_callback(config->set_time_button_pin, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
	gpio_init(config->set_time_button_pin);
	gpio_pull_up(config->set_time_button_pin);
	gpio_set_dir(config->set_time_button_pin, GPIO_IN);
	g_gpio_cooldowns[config->set_time_button_pin] = CHANGE_STATE_BUTTON_COOLDOWN_DURATION_US;

	gpio_set_irq_enabled(config->set_alarm_button_pin, GPIO_IRQ_EDGE_FALL, true);
	gpio_init(config->set_alarm_button_pin);
	gpio_pull_up(config->set_alarm_button_pin);
	gpio_set_dir(config->set_alarm_button_pin, GPIO_IN);
	g_gpio_cooldowns[config->set_alarm_button_pin] = CHANGE_STATE_BUTTON_COOLDOWN_DURATION_US;

	for (i = 0; i < config->digit_button_count; i++) {
		gpio_init(config->digit_button_pins[i]);
		gpio_pull_up(config->digit_button_pins[i]);
		gpio_set_dir(config->digit_button_pins[i], GPIO_IN);
		gpio_set_irq_enabled(config->digit_button_pins[i], GPIO_IRQ_EDGE_FALL, true);
		g_gpio_cooldowns[config->digit_button_pins[i]] = CHANGE_DIGIT_BUTTON_COOLDOWN_DURATION_US;
	}
}

input__state_t input__get_state(void) {
	return g_state;
}

uint32_t input__get_set_time_value(void) {
	uint32_t result = 0;
	uint32_t multiplier = 1;
	uint8_t i;

	for (i = g_handler.config.digit_button_count; i > 0; i--) {
		result += multiplier * g_handler.set_time_digit_values[i - 1];
		multiplier *= 10;
	}

	return result;
}

void gpio_callback(uint gpio, uint32_t events) {
	absolute_time_t time;
	uint64_t time_us;
	uint64_t start;
	uint64_t current;
	uint8_t i, hour, minute;
	uint32_t cooldown_duration;
	uint pin;

	time = get_absolute_time();
	time_us = to_us_since_boot(time);
	cooldown_duration = g_gpio_cooldowns[gpio];

	if ((time_us - g_cooldown_start < cooldown_duration) && (g_cooldown_gpio == gpio)) {
		return;
	}

	/* Wait a bit to make sure it's a real press */

	g_cooldown_start = time_us;
	g_cooldown_gpio = gpio;

	/* Check which button was pressed */

	if (g_handler.config.set_time_button_pin == gpio) {
		if (STATE__NORMAL == g_state) {
			g_state = STATE__TIME_SET;
			g_handler.current_digit_in_time_set = 0;
			memset(g_handler.set_time_digit_values, 0, sizeof(g_handler.set_time_digit_values));
		} else if (STATE__TIME_SET == g_state) {
			g_state = STATE__NORMAL;

			minute = g_handler.set_time_digit_values[3] + (g_handler.set_time_digit_values[2] * 10);
			hour = g_handler.set_time_digit_values[1] + (g_handler.set_time_digit_values[0] * 10);

			if ((hour != 0) || (minute != 0)) {
				time__configure_time(hour, minute, 0);
			}
		}
	} else if (g_handler.config.set_alarm_button_pin == gpio) {
		if (STATE__NORMAL == g_state) {
			g_state = STATE__ALARM_SET;
			g_handler.current_digit_in_time_set = 0;
			memset(g_handler.set_time_digit_values, 0, sizeof(g_handler.set_time_digit_values));
		} else if (STATE__ALARM_SET == g_state) {
			g_state = STATE__NORMAL;

			minute = g_handler.set_time_digit_values[3] + (g_handler.set_time_digit_values[2] * 10);
			hour = g_handler.set_time_digit_values[1] + (g_handler.set_time_digit_values[0] * 10);

			if ((hour != 0) || (minute != 0)) {
				time__configure_alarm_time(hour, minute, 0);
			}
		}
	} else {
		for (i = 0; i < g_handler.config.digit_button_count; i++) {
			pin = g_handler.config.digit_button_pins[i];
			if (pin == gpio) {
				if (i == g_handler.current_digit_in_time_set) {
					g_handler.set_time_digit_values[i] = (g_handler.set_time_digit_values[i] + 1) % 10;
				} else {
					g_handler.current_digit_in_time_set = i;
				}
			}
		}
	}
}
