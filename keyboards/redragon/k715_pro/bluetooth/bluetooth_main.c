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

/**
 * @brief Weak function for pre-task operations.
 *
 * Can be overridden by the user.
 */
__attribute__((weak)) void bluetooth_pre_task(void) {}

/**
 * @brief Weak function for task operations.
 *
 * Can be overridden by the user.
 */
__attribute__((weak)) void bluetooth_task(void) {}

/**
 * @brief Weak function for post-task operations.
 *
 * Can be overridden by the user.
 */
__attribute__((weak)) void bluetooth_post_task(void) {}

/**
 * @brief Runs all Bluetooth tasks (pre, main, post).
 */
void bluetooth_tasks(void) {
    bluetooth_pre_task();
    bluetooth_task();
    bluetooth_post_task();
}

/**
 * @brief QMK callback for housekeeping tasks.
 *
 * Calls bluetooth_tasks.
 */
void housekeeping_task_kb(void) {
    bluetooth_tasks();
}
