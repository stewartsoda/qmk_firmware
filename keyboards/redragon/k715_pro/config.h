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

#define IS31FL3733_I2C_ADDRESS_1 IS31FL3733_I2C_ADDRESS_GND_GND     // i2c address 0x50
#define IS31FL3733_I2C_ADDRESS_2 IS31FL3733_I2C_ADDRESS_GND_VCC     // i2c address 0x53
#define IS31FL3733_PWM_FREQUENCY IS31FL3733_PWM_FREQUENCY_26K7_HZ   // default is 8.4 kHz according to QMK docs
#define IS31FL3733_SYNC_1 IS31FL3733_SYNC_MASTER
#define IS31FL3733_SYNC_2 IS31FL3733_SYNC_SLAVE

#define I2C_DRIVER I2CD1

#define I2C1_CLOCK_SPEED 100000
#define I2C1_SCL_PIN B6 // A2 on pinout = B0
#define I2C1_SDA_PIN B7 // A2 on pinout = B1

#define SPI_DRIVER SPID2
#define SPI_SCK_PIN  B13
#define SPI_MOSI_PIN B15
#define SPI_MISO_PIN B14

#define SPI_SLAVE_CS B12

// #define EEPROM_I2C_CAT24C512
// #define EXTERNAL_EEPROM_BYTE_COUNT 65536

#define ADC_BUFFER_DEPTH 2
#define ADC_RESOLUTION ADC_CFGR1_RES_12BIT
#define ADC_SAMPLING_RATE ADC_SMPR_SMP_41P5

//#define DEBUG_MATRIX_SCAN_RATE
