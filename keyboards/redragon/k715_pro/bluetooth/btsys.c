/* Copyright 2024 temp4gh
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

#include "quantum.h"
#include "config.h"
#include "btspi.h"

#define TRACE dprintf("[%08lu] %s: %d\n", timer_read32(), __FUNCTION__, __LINE__)


/**
 * @brief Powers on the IS31FL3733 LED controller.
 */
void init_set_IS31FL3733_poweron(void)
{
    TRACE;
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
    TRACE;
    NVIC_SystemReset();
}

/**
 * @brief Resets the MCU.
 */
void mcu_reset(void)
{
    TRACE;
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
    TRACE;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_DISABLE;        // disable JTAG/SWD

    palSetLineMode(CAPS_LOCK_DRV_PIN, PAL_MODE_OUTPUT_PUSHPULL);
    set_caps_lock_off();

    palSetPadMode(GPIOC, 11, PAL_MODE_INPUT);           // UART4_RX
    palSetPadMode(GPIOA, 13, PAL_MODE_OUTPUT_PUSHPULL); // JTMS-SWDIO
    palSetPadMode(GPIOA, 14, PAL_MODE_OUTPUT_PUSHPULL); // JTCK-SWCLK
    palSetPadMode(GPIOA, 15, PAL_MODE_OUTPUT_PUSHPULL); // JTDI

    init_set_IS31FL3733_poweron();
}
