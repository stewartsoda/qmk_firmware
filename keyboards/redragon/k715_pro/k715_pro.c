/* Copyright 2023 temp4gh
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "spi_master.h"
#include "host_driver.h"
#include "config.h"

#include "k715_pro.h"
#include "btspi.h"

#include "analog.h"
#include "gpio.h"
#include "battery.h"

/**
 * @brief LED matrix configuration for IS31FL3733.
 */
const is31fl3733_led_t PROGMEM g_is31fl3733_leds[IS31FL3733_LED_COUNT] =
{
    {0, SW7_CS1,    SW9_CS1,    SW8_CS1},
    {0, SW7_CS3,    SW9_CS3,    SW8_CS3},
    {0, SW7_CS4,    SW9_CS4,    SW8_CS4},
    {0, SW7_CS5,    SW9_CS5,    SW8_CS5},
    {0, SW7_CS6,    SW9_CS6,    SW8_CS6},
    {0, SW7_CS7,    SW9_CS7,    SW8_CS7},
    {0, SW7_CS8,    SW9_CS8,    SW8_CS8},
    {0, SW7_CS9,    SW9_CS9,    SW8_CS9},
    {0, SW7_CS10,   SW9_CS10,   SW8_CS10},
    {0, SW7_CS11,   SW9_CS11,   SW8_CS11},
    {0, SW7_CS12,   SW9_CS12,   SW8_CS12},
    {0, SW7_CS13,   SW9_CS13,   SW8_CS13},
    {0, SW7_CS14,   SW9_CS14,   SW8_CS14},
    {0, SW7_CS15,   SW9_CS15,   SW8_CS15},
    {0, SW4_CS1,    SW6_CS1,    SW5_CS1},
    {0, SW4_CS2,    SW6_CS2,    SW5_CS2},
    {0, SW4_CS3,    SW6_CS3,    SW5_CS3},
    {0, SW4_CS4,    SW6_CS4,    SW5_CS4},
    {0, SW4_CS5,    SW6_CS5,    SW5_CS5},
    {0, SW4_CS6,    SW6_CS6,    SW5_CS6},
    {0, SW4_CS7,    SW6_CS7,    SW5_CS7},
    {0, SW4_CS8,    SW6_CS8,    SW5_CS8},
    {0, SW4_CS9,    SW6_CS9,    SW5_CS9},
    {0, SW4_CS10,   SW6_CS10,   SW5_CS10},
    {0, SW4_CS11,   SW6_CS11,   SW5_CS11},
    {0, SW4_CS12,   SW6_CS12,   SW5_CS12},
    {0, SW4_CS13,   SW6_CS13,   SW5_CS13},
    {0, SW4_CS14,   SW6_CS14,   SW5_CS14},
    {0, SW4_CS15,   SW6_CS15,   SW5_CS15},
    {0, SW1_CS1,    SW3_CS1,    SW2_CS1},
    {0, SW1_CS2,    SW3_CS2,    SW2_CS2},
    {0, SW1_CS3,    SW3_CS3,    SW2_CS3},
    {0, SW1_CS4,    SW3_CS4,    SW2_CS4},
    {0, SW1_CS5,    SW3_CS5,    SW2_CS5},
    {0, SW1_CS6,    SW3_CS6,    SW2_CS6},
    {0, SW1_CS7,    SW3_CS7,    SW2_CS7},
    {0, SW1_CS8,    SW3_CS8,    SW2_CS8},
    {0, SW1_CS9,    SW3_CS9,    SW2_CS9},
    {0, SW1_CS10,   SW3_CS10,   SW2_CS10},
    {0, SW1_CS11,   SW3_CS11,   SW2_CS11},
    {0, SW1_CS12,   SW3_CS12,   SW2_CS12},
    {0, SW1_CS13,   SW3_CS13,   SW2_CS13},
    {0, SW1_CS14,   SW3_CS14,   SW2_CS14},
    {0, SW1_CS15,   SW3_CS15,   SW2_CS15},
    {1, SW7_CS1,    SW9_CS1,    SW8_CS1},
    {1, SW7_CS2,    SW9_CS2,    SW8_CS2},
    {1, SW7_CS3,    SW9_CS3,    SW8_CS3},
    {1, SW7_CS4,    SW9_CS4,    SW8_CS4},
    {1, SW7_CS5,    SW9_CS5,    SW8_CS5},
    {1, SW7_CS6,    SW9_CS6,    SW8_CS6},
    {1, SW7_CS7,    SW9_CS7,    SW8_CS7},
    {1, SW7_CS8,    SW9_CS8,    SW8_CS8},
    {1, SW7_CS9,    SW9_CS9,    SW8_CS9},
    {1, SW7_CS10,   SW9_CS10,   SW8_CS10},
    {1, SW7_CS11,   SW9_CS11,   SW8_CS11},
    {1, SW7_CS12,   SW9_CS12,   SW8_CS12},
    {1, SW7_CS13,   SW9_CS13,   SW8_CS13},
    {1, SW7_CS14,   SW9_CS14,   SW8_CS14},
    {1, SW7_CS15,   SW9_CS15,   SW8_CS15},
    {1, SW4_CS1,    SW6_CS1,    SW5_CS1},
    {1, SW4_CS2,    SW6_CS2,    SW5_CS2},
    {1, SW4_CS3,    SW6_CS3,    SW5_CS3},
    {1, SW4_CS4,    SW6_CS4,    SW5_CS4},
    {1, SW4_CS5,    SW6_CS5,    SW5_CS5},
    {1, SW4_CS6,    SW6_CS6,    SW5_CS6},
    {1, SW4_CS7,    SW6_CS7,    SW5_CS7},
    {1, SW4_CS8,    SW6_CS8,    SW5_CS8},
    {1, SW4_CS9,    SW6_CS9,    SW5_CS9},
    {1, SW4_CS10,   SW6_CS10,   SW5_CS10},
    {1, SW4_CS11,   SW6_CS11,   SW5_CS11},
    {1, SW4_CS12,   SW6_CS12,   SW5_CS12},
    {1, SW4_CS13,   SW6_CS13,   SW5_CS13},
    {1, SW4_CS14,   SW6_CS14,   SW5_CS14},
    {1, SW4_CS15,   SW6_CS15,   SW5_CS15},
    {1, SW1_CS1,    SW3_CS1,    SW2_CS1},
    {1, SW1_CS2,    SW3_CS2,    SW2_CS2},
    {1, SW1_CS3,    SW3_CS3,    SW2_CS3},
    {1, SW1_CS5,    SW3_CS5,    SW2_CS5},
    {1, SW1_CS6,    SW3_CS6,    SW2_CS6},
    {1, SW1_CS7,    SW3_CS7,    SW2_CS7},
    {1, SW1_CS8,    SW3_CS8,    SW2_CS8},
    {1, SW1_CS9,    SW3_CS9,    SW2_CS9},
    {1, SW1_CS10,   SW3_CS10,   SW2_CS10},
    {1, SW1_CS11,   SW3_CS11,   SW2_CS11},
    {1, SW1_CS12,   SW3_CS12,   SW2_CS12},
    {1, SW1_CS13,   SW3_CS13,   SW2_CS13},
    {1, SW1_CS14,   SW3_CS14,   SW2_CS14},
    {1, SW1_CS15,   SW3_CS15,   SW2_CS15},
    {0, SW7_CS2,    SW9_CS2,    SW8_CS2},
    {1, SW1_CS4,    SW3_CS4,    SW2_CS4}
};

/**
 * @brief Powers on the IS31FL3733 LED controller.
 */
void init_set_IS31FL3733_poweron(void)
{
    palSetLineMode(IS31FL3733_POWERON_PIN, PAL_MODE_OUTPUT_PUSHPULL);   // BOOT1
    palSetLine(IS31FL3733_POWERON_PIN);
}

/**
 * @brief Jumps to the bootloader.
 *
 * Resets the system.
 */
void bootloader_jump(void)
{
    NVIC_SystemReset();
}

/**
 * @brief Resets the MCU.
 */
void mcu_reset(void)
{
    NVIC_SystemReset();
}

/**
 * @brief Initializes the board hardware.
 *
 * Disables JTAG/SWD, sets up pins for Caps Lock, UART, and SWD.
 * Powers on the LED controller.
 */
void board_init(void)
{
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_DISABLE;        // disable JTAG/SWD

    palSetLineMode(CAPS_LOCK_DRV_PIN, PAL_MODE_OUTPUT_PUSHPULL);
    set_caps_lock_off();

    palSetPadMode(GPIOC, 11, PAL_MODE_INPUT);           // UART4_RX
    palSetPadMode(GPIOA, 13, PAL_MODE_OUTPUT_PUSHPULL); // JTMS-SWDIO
    palSetPadMode(GPIOA, 14, PAL_MODE_OUTPUT_PUSHPULL); // JTCK-SWCLK
    palSetPadMode(GPIOA, 15, PAL_MODE_OUTPUT_PUSHPULL); // JTDI

    init_set_IS31FL3733_poweron();
}

/**
 * @brief Sends a HID report over BLE.
 *
 * @param report_type The type of report to send (e.g., normal key, extra key).
 * @param hid_report_buf Pointer to the report data buffer.
 * @param data_len Length of the report data.
 */
static void send_ble_hid_report(uint8_t report_type, uint8_t *hid_report_buf, uint8_t data_len)
{
    switch(report_type)
    {
        case BLE_HID_REPORT_TYPE_NORMAL_KEY:
        {
            k715bt_send_spi_extend_single_packet(report_type, hid_report_buf, data_len);
            break;
        }

        case BLE_HID_REPORT_TYPE_EXTRA_KEY:
        {
            k715bt_send_spi_extend_single_packet(report_type, hid_report_buf, data_len);
            break;
        }

        default:
        {
            break;
        }
    }
}

/**
 * @brief Returns the LED state for BLE.
 *
 * @return uint8_t Always returns 0.
 */
static uint8_t k715_ble_leds(void)
{
    // TODO: implement this function to return actual LED state
    return 0;
}

/**
 * @brief Sends the keyboard report over BLE.
 *
 * @param report Pointer to the keyboard report.
 */
static void k715_send_keyboard(report_keyboard_t *report)
{
    uint8_t raw_data[KEYBOARD_REPORT_KEYS + 2];

    raw_data[0] = report->mods;
    raw_data[1] = 0;
    for(int i = 0; i < KEYBOARD_REPORT_KEYS; i++)
    {
        raw_data[2 + i] = report->keys[i];
    }

    send_ble_hid_report(BLE_HID_REPORT_TYPE_NORMAL_KEY, raw_data, KEYBOARD_REPORT_KEYS + 2);
}

/**
 * @brief Reads the MCU temperature sensor value.
 *
 * Used in debug_method.
 */
void get_temp_sensor_value(void) {
    static adcsample_t samples[2];
    static const ADCConversionGroup adcgrpcfg = {
        FALSE,
        2,
        NULL,
        NULL,
        0, ADC_CR2_TSVREFE,           /* CR1, CR2 */
        ADC_SMPR1_SMP_SENSOR(ADC_SAMPLE_28P5) | ADC_SMPR1_SMP_VREF(ADC_SAMPLE_239P5),
        0,                            /* SMPR2 */
        ADC_SQR1_NUM_CH(2),
        0,
        ADC_SQR3_SQ1_N(ADC_CHANNEL_VREFINT) | ADC_SQR3_SQ2_N(ADC_CHANNEL_SENSOR)
    };

    adcStart(&ADCD1, NULL);
    adcConvert(&ADCD1, &adcgrpcfg, samples, 2);
    adcStop(&ADCD1);

    dprintf("[%08lu] ADC_Vtemp=0x%04x, ADC_Vrefint=0x%04x]\n", timer_read32(), samples[1], samples[0]);

    //temp sensor calc:
    //temp_in_C = (V25-V_sense)/avg_slope +25
    //temp_c = (1.43 - V_sense)/.0043 + 25
    //temp_c = (1430 - (A_sense * 1200)/A_refint)/4.3 + 25
    //temp_c = (14300 - (A_sense * 12000)/A_refint)/43 + 25
    int temperature = (14300 - (samples[1] * 12000)/samples[0])/43 + 25;
    dprintf("[%08lu] Temperature: %d C\n", timer_read32(), temperature);
}

/**
 * @brief Debug function to print status information.
 *
 * Used in keymaps/default/keymap.c.
 *
 * @param loops Number of loops (unused in function body).
 */
void debug_method()
{
    uint8_t PC15 = gpio_read_pin(USB_VBUS_PIN);
    uint8_t PC1 = gpio_read_pin(BATTERY_CHARGING_PIN);
    uprintf("[%08lu] USB Vbus Pin PC15=%d, Battery Charging Pin PC1=%d\n", timer_read32(), PC15, PC1);

    battery_driver_sample_percent();
    get_temp_sensor_value();
}

/**
 * @brief QMK callback for DIP switch updates.
 *
 * @param index The index of the DIP switch.
 * @param active The state of the DIP switch.
 * @return bool True if the update was handled, false otherwise. TODO: check this return value
 */
bool dip_switch_update_kb(uint8_t index, bool active) {
    dprintf("[%08lu] dip_switch_update_kb: index=%d, active=%d\n", timer_read32(), index, active);
    switch (index) {
        case 0:
            dprintf("[%08lu] BT MODE %s\n", timer_read32(), active ? "ON" : "OFF");
            k715_set_device_mode(active ? KBD_BT_MODE : KBD_POWEROFF_MODE);
            break;
        case 1:
            dprintf("[%08lu] USB MODE %s\n", timer_read32(), active ? "ON" : "OFF");
            if (active) k715_set_device_mode(KBD_USB_MODE);
            break;
        case 2:
            dprintf("[%08lu] %s MODE\n", timer_read32(), active ? "WIN" : "MAC");
            break;
    }
    if (!dip_switch_update_user(index, active)) {
        return false;
    }
    return true;
}

#ifdef NKRO_ENABLE
/**
 * @brief Sends the NKRO report over BLE.
 *
 * @param report Pointer to the NKRO report.
 */
static void k715_send_nkro(report_nkro_t *report)
{
    uint8_t raw_data[NKRO_REPORT_BITS + 2];

    raw_data[0] = report->mods;
    raw_data[1] = 0;
    for(int i = 0; i < NKRO_REPORT_BITS; i++)
    {
        raw_data[2 + i] = report->bits[i];
    }

    send_ble_hid_report(BLE_HID_REPORT_TYPE_ALL_KEY, raw_data, NKRO_REPORT_BITS + 2);
}
#endif

/**
 * @brief Sends the mouse report over BLE.
 *
 * Currently empty implementation.
 *
 * @param report Pointer to the mouse report.
 */
static void k715_send_mouse(report_mouse_t *report)
{
}

/**
 * @brief Sends the extra keys (consumer) report over BLE.
 *
 * @param report Pointer to the extra keys report.
 */
static void k715_send_extra(report_extra_t *report)
{
    if(report->report_id == REPORT_ID_CONSUMER)
    {
        uint8_t extra_buf[6];

        memset(extra_buf, 0, sizeof(extra_buf));
        memcpy(extra_buf, &report->usage, 2);
        send_ble_hid_report(BLE_HID_REPORT_TYPE_EXTRA_KEY, extra_buf, sizeof(extra_buf));
    }
}

/**
 * @brief BLE host driver structure.
 */
static host_driver_t k715_ble_driver =
{
#ifdef NKRO_ENABLE
    k715_ble_leds, k715_send_keyboard, k715_send_nkro, k715_send_mouse,  k715_send_extra
#else
    k715_ble_leds, k715_send_keyboard, NULL, k715_send_mouse,  k715_send_extra
#endif
};

/**
 * @brief Switches the host driver to BLE.
 *
 * @return bool True if switched, false if already on BLE driver.
 */
static bool _swtich_bt_driver(void)
{
    if(host_get_driver() == &k715_ble_driver)
    {
        return false;
    }

    clear_keyboard();

#ifdef NKRO_ENABLE
    keymap_config.nkro = true;
#else
    keymap_config.nkro = false;
#endif

    host_set_driver(&k715_ble_driver);
    return true;
}

/**
 * @brief Initializes the Bluetooth subsystem.
 *
 * Used in keyboard_post_init_kb.
 */
void k715_bt_init(void)
{
    k715_ble_spi_init();
    rgb_matrix_enable();
    rgb_matrix_set_flags(NORMAL_LED_FLAG_BIT);
}

/**
 * @brief QMK callback for LED updates.
 *
 * @param led_state The state of the LEDs.
 * @return bool True if the update was handled, false otherwise.
 */
bool led_update_kb(led_t led_state)
{
    bool res = led_update_user(led_state);

    if(res)
    {
        update_caps_led();
    }

    return res;
}

/**
 * @brief Handles Bluetooth specific keycodes.
 *
 * Used in process_record_kb.
 *
 * @param keycode The keycode to process.
 * @param record The key record.
 * @return bool True if processing should continue, false otherwise.
 */
bool process_record_kb_bt(uint16_t keycode, keyrecord_t *record)
{
    static uint8_t chn = 0;

    switch(keycode)
    {
        case BT_CHN1:
        case BT_CHN2:
        case BT_CHN3:
            if(is_bt_mode_enabled())
            {
                uprintf("[%08lu] BT key pressed: kc: 0x%04X\n", timer_read32(), keycode);
                if(record->event.pressed)
                {
                    chn = keycode - BT_CHN1 + 1;
                    k715_bt_start_pair(chn);
                }
                else
                {
                    chn = 0;
                }
            }
            break;

        default:
            break;
    }

    return true;
}

/**
 * @brief QMK callback for key processing.
 *
 * @param keycode The keycode to process.
 * @param record The key record.
 * @return bool True if processing should continue, false otherwise.
 */
bool process_record_kb(uint16_t keycode, keyrecord_t *record)
{
    if(!process_record_user(keycode, record))
    {
        return false;
    }

    return process_record_kb_bt(keycode, record);
}

/**
 * @brief Task to handle Bluetooth operations.
 *
 * Called from housekeeping_task_kb (via bluetooth_tasks).
 */
void bluetooth_task(void)
{
    if(is_bt_mode_enabled())
    {
        _swtich_bt_driver();
    }
    check_read_spi_data();
}

/**
 * @brief QMK callback for housekeeping tasks.
 *
 * Calls bluetooth_task.
 */
void housekeeping_task_kb(void) {
    bluetooth_task();
}

/**
 * @brief QMK callback after keyboard initialization.
 */
void keyboard_post_init_kb(void)
{
    gpio_set_pin_input(USB_VBUS_PIN);
    k715_bt_init();

    keyboard_post_init_user();
}

/**
 * @brief QMK callback for suspend power down.
 */
void suspend_power_down_kb(void) {
    // code will run multiple times while keyboard is suspended
    uprintf("[%08lu] Going to sleep\n", timer_read32());

    suspend_power_down_user();
}

/**
 * @brief QMK callback for suspend wakeup initialization.
 */
void suspend_wakeup_init_kb(void) {
    // code will run on keyboard wakeup
    uprintf("[%08lu] Waking up\n", timer_read32());

    suspend_wakeup_init_user();
}

/**
 * @brief Global variable for battery percentage.
 *
 * Used in battery_custom.c and k715_pro.c.
 */
int battery_percentage;

/**
 * @brief QMK callback for advanced RGB indicators.
 *
 * @param led_min Minimum LED index.
 * @param led_max Maximum LED index.
 * @return bool True if handled, false otherwise.
 */
bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max){
    if (!rgb_matrix_indicators_advanced_user(led_min, led_max)) {
        return false;
    }

    if (led_min <= 89 && led_max >= 88) {
        if (host_keyboard_led_state().num_lock) {
            rgb_matrix_set_color(88, 255,0,0);  // "M" indicator
        } else {
            rgb_matrix_set_color(88, 0,0,0);  // "M" indicator
        }
        /* for battery indicator:
            0-50 : R = 0xFF, G = 0x00->0xFF
            51-100 : R = 0xFF->0x00, G = 0xFF
        */
        uint8_t red, green = 0;
        if (battery_percentage <= 50) {
            red = 255;
            green = (uint8_t)((battery_percentage * 255) / 50);
        } else {
            red = (uint8_t)(((100 - battery_percentage) * 255) / 50);
            green = 255;
        }
        rgb_matrix_set_color(89, red, green, 0);    // battery indicator
    }

    return true;
}

/**
 * @brief QMK callback for VBUS state.
 *
 * @return bool True if VBUS is high, false otherwise.
 */
bool usb_vbus_state(void) {
    return gpio_read_pin(USB_VBUS_PIN);
}

