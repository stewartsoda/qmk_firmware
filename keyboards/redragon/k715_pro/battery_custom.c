#include "battery_driver.h"
#include "analog.h"
#include "gpio.h"

#include "k715_pro.h"


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
    adc_mux vrefint_mux = TO_MUX(17, 0);
    adc_mux vbatt_mux = pinToMux(BATTERY_ADC_PIN);
    int32_t vrefint_raw = adc_read(vrefint_mux);
    int32_t vbatt_raw = adc_read(vbatt_mux);
    value = vbatt_raw * 1200 / vrefint_raw;     // value is in millivolts
    dprintf("[%08lu] Vbatt_raw: %ld, Vrefint_raw: %ld.\n", timer_read32(), vbatt_raw, vrefint_raw);
#endif
    dprintf("[%08lu] Get battery mV: %d mV.\n", timer_read32(), (int16_t)value);
    return value;
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
