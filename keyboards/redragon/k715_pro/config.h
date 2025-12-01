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

#pragma once

// I2C SETTINGS
#define I2C_DRIVER I2CD1
#define I2C1_CLOCK_SPEED 100000
#define I2C1_SCL_PIN B6
#define I2C1_SDA_PIN B7

// SPI SETTINGS
#define SPI_DRIVER SPID2
#define SPI_SCK_PIN  B13
#define SPI_MOSI_PIN B15
#define SPI_MISO_PIN B14
#define SPI_SLAVE_CS B12

// ADC SETTINGS
#define ADC_BUFFER_DEPTH 2
#define ADC_RESOLUTION ADC_CFGR1_RES_10BIT  //Same as Redragon FW
#define ADC_SAMPLING_RATE ADC_SMPR_SMP_1P5

// PIN DEFINITIONS
#define BATTERY_ADC_PIN C0
#define BATTERY_CHARGING_PIN C1
#define USB_VBUS_PIN C15

// DYNAMIC KEYMAP/MACRO SETTINGS
#define DYNAMIC_KEYMAP_MACRO_EEPROM_SIZE 512
#define DYNAMIC_KEYMAP_MACRO_COUNT 20

// RGB MATRIX SETTINGS
#define RGB_MATRIX_TIMEOUT 900000  // ms to wait until rgb matrix time out, 900K ms is 15min.
#define RGB_MATRIX_SLEEP // turn off effects when suspended
#define RGB_MATRIX_LED_FLUSH_LIMIT 16 // limits in milliseconds how frequently an animation will update the LEDs. 16 (16ms) is equivalent to limiting to 60fps (increases keyboard responsiveness)

// LED DRIVER SETTINGS
#define IS31FL3733_I2C_ADDRESS_1 IS31FL3733_I2C_ADDRESS_GND_GND     // i2c address 0x50
#define IS31FL3733_I2C_ADDRESS_2 IS31FL3733_I2C_ADDRESS_GND_VCC     // i2c address 0x53
#define IS31FL3733_PWM_FREQUENCY IS31FL3733_PWM_FREQUENCY_26K7_HZ   // default is 8.4 kHz according to QMK docs
#define IS31FL3733_SYNC_1 IS31FL3733_SYNC_MASTER
#define IS31FL3733_SYNC_2 IS31FL3733_SYNC_SLAVE

// POWER SETTINGS
#define BLUETOOTH_POWER_DOWN_TIMEOUT 600000 // ms to wait until bluetooth powers down, 600K ms is 10min.
