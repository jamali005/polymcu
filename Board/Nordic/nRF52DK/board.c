/*
 * Copyright (c) 2015-2017, Lab A Part
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this
 * o list of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <stdio.h>
#include "board.h"
#include "Driver_USART.h"
#include "boards.h"
#include "app_error.h"
#if defined(__CMSIS_RTOS) && defined(SOFTDEVICE_PRESENT)
#include "softdevice_handler.h"
#include "app_util_platform.h"
#endif

extern const ARM_DRIVER_USART Driver_UART_DEBUG;
static const uint8_t leds_list[LEDS_NUMBER] = LEDS_LIST;

// Initialize newlib
void hardware_init_hook(void) {
    // Configure LED-pins as outputs.
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);

	// Initialize UART
	Driver_UART_DEBUG.Initialize(NULL);

#if defined(__CMSIS_RTOS) && defined(SOFTDEVICE_PRESENT)
    // Initialize the SoftDevice handler module.
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    // We need to configure the interrupt before starting the kernel as we cannot call an SVC in an SVC Handler
    uint32_t err_code = sd_nvic_SetPriority(RTC1_IRQn, APP_IRQ_PRIORITY_LOW);
    assert(err_code == NRF_SUCCESS);

    err_code = sd_nvic_EnableIRQ(RTC1_IRQn);
    assert(err_code == NRF_SUCCESS);
#endif

	// Ensure SystemCoreClock is set
	SystemCoreClockUpdate();
}

void led_on(int led) {
	LEDS_ON(1 << leds_list[led]);
}

void led_off(int led) {
	LEDS_OFF(1 << leds_list[led]);
}

void led_toggle(int led) {
	if (led < LEDS_NUMBER) {
		LEDS_INVERT(1 << leds_list[led]);
	}
}

void led_set(int led, int value) {
	if (led < LEDS_NUMBER) {
		if (value) {
			LEDS_ON(1 << leds_list[led]);
		} else {
			LEDS_OFF(1 << leds_list[led]);
		}
	}
}

int led_get(int led) {
	if (led < LEDS_NUMBER) {
		uint32_t gpio_state = NRF_GPIO->OUT;
		if (gpio_state & (1 << leds_list[led])) {
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

#ifdef DEBUG
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name) {
	char error[40];
	sprintf(error, "Nordic assert: error_code:%u", (unsigned int)error_code);
	__assert_func((const char *)p_file_name, line_num, NULL, error);
}
#else
void app_error_handler_bare(ret_code_t error_code) {
	led_on(3);
	while(1);
}
#endif

/**@brief Function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num     Line number of the failing ASSERT call.
 * @param[in] p_file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * file_name) {
	APP_ERROR_HANDLER(0xDEADBEEF);
}
