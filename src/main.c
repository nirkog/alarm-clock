#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/sync.h"

#include "audio/wav.h"
#include "time/time.h"
#include "seven_segment/seven_segment.h"

#define AUDIO_PIN (28)

seven_segment__driver_t g_driver;

int wav_position = 0;

void pwm_interrupt_handler() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));    
    if (wav_position < (WAV_DATA_LENGTH<<3) - 1) { 
        // set pwm level 
        // allow the pwm value to repeat for 8 cycles this is >>3 
        pwm_set_gpio_level(AUDIO_PIN, WAV_DATA[wav_position>>3]);  
        wav_position++;
    } else {
        // reset to start
        wav_position = 0;
    }
}

int main() {
	const uint LED_PIN = PICO_DEFAULT_LED_PIN;

	/* Overclocking for fun but then also so the system clock is a 
     * multiple of typical audio sampling rates.
     */
    stdio_init_all();
    set_sys_clock_khz(176000, true); 
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);

    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);

    // Setup PWM interrupt to fire when PWM cycle is complete
    pwm_clear_irq(audio_pin_slice);
    pwm_set_irq_enabled(audio_pin_slice, true);
    // set the handle function above
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler); 
    //irq_set_enabled(PWM_IRQ_WRAP, true);

    // Setup PWM for audio output
    pwm_config config = pwm_get_default_config();
    /* Base clock 176,000,000 Hz divide by wrap 250 then the clock divider further divides
     * to set the interrupt rate. 
     * 
     * 11 KHz is fine for speech. Phone lines generally sample at 8 KHz
     * 
     * 
     * So clkdiv should be as follows for given sample rate
     *  8.0f for 11 KHz
     *  4.0f for 22 KHz
     *  2.0f for 44 KHz etc
     */
    pwm_config_set_clkdiv(&config, 8.0f); 
    pwm_config_set_wrap(&config, 250); 
    pwm_init(audio_pin_slice, &config, true);

    pwm_set_gpio_level(AUDIO_PIN, 0);

	seven_segment__init_driver(&g_driver);

	g_driver.config.digit_count = 4;
	g_driver.config.a_pin = 5;
	g_driver.config.b_pin = 6;
	g_driver.config.c_pin = 7;
	g_driver.config.d_pin = 8;
	g_driver.config.e_pin = 9;
	g_driver.config.f_pin = 10;
	g_driver.config.g_pin = 11;
	g_driver.config.dp_pin = 12;
	g_driver.config.digit_pins[0] = 13;
	g_driver.config.digit_pins[1] = 14;
	g_driver.config.digit_pins[2] = 15;
	g_driver.config.digit_pins[3] = 16;

	seven_segment__configure_pins(&g_driver);

	time__configure_time(23, 59, 50);
	time__configure_alarm_duration(5);
	time__configure_alarm_time(0, 0, 0);
	time__start();

	uint8_t second, minute, hour;
	uint32_t display_number = 0;

	while (true) {
		time__get_current_time(&hour, &minute, &second);
		display_number = hour * 100 + minute;
		seven_segment__set_number_value(&g_driver, display_number);
		seven_segment__set_decimal_point_value(&g_driver, 1, true);
		
		seven_segment__display(&g_driver);

		if (time__is_in_alarm()) {
			irq_set_enabled(PWM_IRQ_WRAP, true);
		} else {
			irq_set_enabled(PWM_IRQ_WRAP, false);
			pwm_set_gpio_level(AUDIO_PIN, 255);
		}

		sleep_ms(1);
	}
	return 0;
}
