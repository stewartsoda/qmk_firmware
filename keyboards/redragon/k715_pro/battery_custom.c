#include "battery_driver.h"
#include "analog.h"
#include "gpio.h"

#include "k715_pro.h"
#include "config.h"


void battery_driver_init(void) {
    // Perform any initialisation here
    //gpio_set_pin_input(BATTERY_ADC_PIN);
    palSetLineMode((BATTERY_ADC_PIN), PAL_MODE_INPUT_ANALOG);
}

uint16_t battery_driver_get_mv(void) {
    /*
        Do the following:
        1. Read the Vrefint on ADC1 ch17
        2. Read the Vbatt voltage from ADC1 pin C0
        3. Vbatt = (Vbatt_raw / Vrefint_raw) * VREFINT_CAL * 1.2
    */
    uint16_t value = 0;
#if DEBUG_CODE
    // generate pseudo-random battery voltage between 3400 and 4250 mV
    const uint16_t min_mv = 3400;
    const uint16_t max_mv = 4250;
    uint32_t seed = timer_read32(); // use current timer as entropy
    // xorshift32 for simple PRNG
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    value = (uint16_t)(min_mv + (seed % (uint32_t)(max_mv - min_mv + 1)));
#else
    palSetLineMode((C0), PAL_MODE_INPUT_ANALOG);
    palSetLineMode((C2), PAL_MODE_INPUT_ANALOG);

    adcStart(&ADCD1, NULL);

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
        ADC_SMPR1_SMP_AN10(ADC_SAMPLE_28P5) | ADC_SMPR1_SMP_VREF(ADC_SAMPLE_239P5),
        0,                            /* SMPR2 */
        ADC_SQR1_NUM_CH(2),
        0,
        ADC_SQR3_SQ2_N(ADC_CHANNEL_VREFINT)   | ADC_SQR3_SQ1_N(ADC_CHANNEL_IN10)
    };

    adcStart(&ADCD1, NULL);

    adcConvert(&ADCD1, &adcgrpcfg, samples, 2);

    dprintf("[%08lu] samples: [     C2,VREFINT]\n", timer_read32());
    dprintf("[%08lu] samples: [%6d, %6d]\n", timer_read32(), samples[0], samples[1]);
    dprintf("[%08lu] samples: [0x%04x, 0x%04x]\n", timer_read32(), samples[0], samples[1]);

    value = samples[0] * 1200 / samples[1];
#endif
    dprintf("[%08lu] Get battery mV: %d mV.\n", timer_read32(), (int16_t)value);
    return 0;
}


uint8_t battery_driver_sample_percent(void) {
    uint16_t bat_mv = battery_driver_get_mv();
    uint8_t percent = 0;
    // https://github.com/zmkfirmware/zmk/blob/3f7c9d7cc4f46617faad288421025ea2a6b0bd28/app/module/drivers/sensor/battery/battery_common.c#L33
    if (bat_mv >= 4200) {
        percent = 100;
    } else if (bat_mv <= 3450) {
        percent = 0;
    } else {
        percent = bat_mv * 2 / 15 - 459;
    }
    dprintf("[%08lu] Sample battery percent: %d percent.\n", timer_read32(), (int16_t)percent);
    return percent;
}
