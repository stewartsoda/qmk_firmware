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

#define IS31FL3733_I2C_ADDRESS_1 IS31FL3733_I2C_ADDRESS_GND_GND
#define IS31FL3733_I2C_ADDRESS_2 IS31FL3733_I2C_ADDRESS_GND_VCC

#define I2C1_CLOCK_SPEED 100000

#define SPI_DRIVER SPID2
#define SPI_SCK_PIN  B13
#define SPI_MOSI_PIN B15
#define SPI_MISO_PIN B14

#define SPI_SLAVE_CS B12

#define EEPROM_I2C_CAT24C512
//#define EXTERNAL_EEPROM_BYTE_COUNT 65536


#define ADC_BUFFER_DEPTH 2
#define ADC_RESOLUTION ADC_CFGR1_RES_12BIT
#define ADC_SAMPLING_RATE ADC_SMPR_SMP_239P5

//#define DEBUG_MATRIX_SCAN_RATE
