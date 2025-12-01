#include "quantum.h"
#include "battery_driver.h"
#include "analog.h"
#include "gpio.h"

#include "k715_pro.h"
#include "config.h"
#include "hal.h"
#include "hal_adc.h"


/**
 * @brief Initializes the battery driver.
 */
void battery_driver_init(void) {
    // Perform any initialisation here
    //gpio_set_pin_input(BATTERY_ADC_PIN);
    palSetLineMode((BATTERY_ADC_PIN), PAL_MODE_INPUT_ANALOG);
}

/**
 * @brief Gets the battery voltage in millivolts.
 *
 * @return uint16_t Battery voltage in mV.
 */
uint16_t battery_driver_get_mv(void) {
    /*
        Do the following:
        1. Read the Vrefint on ADC1 ch17
        2. Read the Vbatt voltage from ADC1 pin C0
        3. Vbatt = (Vbatt_raw / Vrefint_raw) * VREFINT_CAL * 1.2
    */

    if (timer_read32() < 10000) {
        // wait for 10 seconds after boot to allow supplies to stabilize and load properly
        return 0;
    }

    uint16_t value = 0;
    palSetLineMode((C0), PAL_MODE_INPUT_ANALOG);
    palSetLineMode((C2), PAL_MODE_INPUT_ANALOG);

    static adcsample_t samples[2];
    /*
    [0] = C2
    [1] = VREFINT
    */

    static const ADCConversionGroup adcgrpcfg = {
        FALSE,
        2,
        NULL,
        NULL,
        0, ADC_CR2_TSVREFE,           /* CR1, CR2 */
        ADC_SMPR1_SMP_AN12(ADC_SAMPLE_28P5) | ADC_SMPR1_SMP_VREF(ADC_SAMPLE_239P5),
        0,                            /* SMPR2 */
        ADC_SQR1_NUM_CH(2),
        0,
        ADC_SQR3_SQ1_N(ADC_CHANNEL_VREFINT)   | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN12)
    };

    adcStart(&ADCD1, NULL);
    adcConvert(&ADCD1, &adcgrpcfg, samples, 2);
    adcStop(&ADCD1);

    dprintf("[%08lu] ADC_C2=0x%04x, ADC_Vrefint=0x%04x]\n", timer_read32(), samples[1], samples[0]);

    value = samples[1] * 1200 * 2 / samples[0];
    dprintf("[%08lu] Get battery mV: ***%d mV***.\n", timer_read32(), (int16_t)value);
    return value;
}

extern int battery_percentage;

/**
 * @brief Samples the battery voltage and calculates the percentage.
 *
 * Updates the global battery_percentage variable.
 *
 * @return uint8_t Battery percentage (0-100).
 */
uint8_t battery_driver_sample_percent(void) {
    uint16_t bat_mv = battery_driver_get_mv();
    uint8_t percent = 0;
    // https://github.com/zmkfirmware/zmk/blob/3f7c9d7cc4f46617faad288421025ea2a6b0bd28/app/module/drivers/sensor/battery/battery_common.c#L33
    //if (bat_mv >= 4200) {
    if (bat_mv >= 4130) {
        percent = 100;
    } else if (bat_mv <= 3450) {
        percent = 0;
    } else {
        //percent = bat_mv * 2 / 15 - 459;
        percent = bat_mv * 2 / 13 - 531;
    }
    // want to read pin C15
    uint8_t PC15 = gpio_read_pin(USB_VBUS_PIN);
    // want to read pin C1
    uint8_t PC1 = gpio_read_pin(BATTERY_CHARGING_PIN);
    dprintf("[%08lu] USB power %s, Battery %s.\n", timer_read32(), PC15 ? "DISABLED" : "ENABLED", PC1 ? "CHARGED" : "NOT CHARGED" );

    dprintf("[%08lu] Sample battery percent: %d percent.\n", timer_read32(), (int16_t)percent);
    battery_percentage = percent;
    return percent;
}
