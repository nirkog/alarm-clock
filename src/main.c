#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/sync.h"

#include "audio/wav.h"
#include "seven_segment/seven_segment.h"

#define AUDIO_PIN (28)

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
    irq_set_enabled(PWM_IRQ_WRAP, true);

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

	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	seven_segment__driver_t driver;

	seven_segment__init_driver(&driver);

	driver.config.digit_count = 4;
	driver.config.a_pin = 5;
	driver.config.b_pin = 6;
	driver.config.c_pin = 7;
	driver.config.d_pin = 8;
	driver.config.e_pin = 9;
	driver.config.f_pin = 10;
	driver.config.g_pin = 11;
	driver.config.dp_pin = 12;
	driver.config.digit_pins[0] = 13;
	driver.config.digit_pins[1] = 14;
	driver.config.digit_pins[2] = 15;
	driver.config.digit_pins[3] = 16;

	seven_segment__configure_pins(&driver);

	//seven_segment__set_digit_value(&driver, 0, 1, false);
	//seven_segment__set_digit_value(&driver, 1, 3, false);
	//seven_segment__set_digit_value(&driver, 2, 3, false);
	//seven_segment__set_digit_value(&driver, 3, 7, false);
	
	uint32_t number = 1337;

	seven_segment__set_number_value(&driver, number);

	printf("Init finished\n");
	uint32_t count = 0;

	seven_segment__display(&driver);
	while (true) {
		//gpio_put(LED_PIN, 1);
		//sleep_ms(5000);
		//gpio_put(LED_PIN, 0);
		//sleep_ms(250);
		
		seven_segment__display(&driver);

		sleep_ms(1);

		count += 1;
		//printf("Update\n");
		
		if (count % 1000 == 0) {
			number++;
			seven_segment__set_number_value(&driver, number);
		}
	}
	return 0;
}
