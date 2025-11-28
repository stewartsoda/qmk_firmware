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

#include QMK_KEYBOARD_H
#include "spi_master.h"
#include "host_driver.h"
#include "k715_pro.h"
#include "btspi.h"

#define SPI_SLAVE_NOTIFY_CMD_RETURN_VALUE (0xAAAABBBB)

#define TRACE dprintf("[%08lu] %s: %d\n", timer_read32(), __FUNCTION__, __LINE__)

/** @brief Flag indicating if SPI is initialized. */
static uint8_t spi_inited = 0;
/** @brief Last detected device mode. */
//static uint8_t last_device_mode = INVALID_DIP_DEVICE_MODE;
/** @brief Current device mode. */
static uint8_t now_mode = KBD_BT_MODE;
/** @brief Keyboard device mode from config. */
static uint8_t kbd_dev_mode;
/** @brief Detected device mode. */
static uint8_t dev_mode_detect;
/** @brief Bluetooth state mode. */
static uint8_t mode_bt_state = 0;
/** @brief Device information structure. */
tDevInfo dev_info;

static spi_read_data_t spi_salve_data;
/** @brief User settings configuration. */
user_settings_config g_config;

#define now_ms timer_read32()
#define get_time_ms() timer_read32()

/**
 * @brief Starts SPI slave communication.
 */
static void spi_slave_start(void)
{
    TRACE;
    spi_start(SPI_SLAVE_CS, false, SPI_MODE, SPI_DIVISOR);
    wait_us(200);
}

/**
 * @brief Stops SPI slave communication.
 */
static void spi_slave_stop(void)
{
    TRACE;
    spi_stop();
}

/**
 * @brief Sets the device mode.
 *
 * @param mode The mode to set.
 */
void k715_set_device_mode(uint8_t mode)
{
    TRACE;
    now_mode = mode;
    if (mode == KBD_BT_MODE) {
        mode_bt_state = 1;
    } else {
        mode_bt_state = 0;
    }
}

/**
 * @brief Gets the detected device mode.
 *
 * @return uint8_t The detected device mode.
 */
static uint8_t get_dev_mode_detected(void)
{
    TRACE;
    return(now_mode);
}

/**
 * @brief Initializes the external read SPI slave GPIO.
 */
static void init_ext_read_spi_slave_gpio(void)
{
    TRACE;
    palSetLineMode(EXT_READ_SPI_SLAVE_INT_PIN, EXT_READ_SPI_SLAVE_GPIO_INIT);
}

/**
 * @brief Sets the IS31FL3733 to normal mode.
 */
static void set_IS31FL3733_normal_mode(void)
{
    TRACE;
    palSetLineMode(EXT_IS31FL3733_SUSPEND_PIN, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLine(EXT_IS31FL3733_SUSPEND_PIN);
}

/**
 * @brief Initializes the device information structure.
 */
static void init_dev_info(void)
{
    TRACE;
    memset(&dev_info, 0, sizeof(dev_info));
}

static uint8_t ble_spi_notify_cmd_buf[MAX_BLE_SPI_NOTIFY_CMD_BUF_SIZE + 4];

/**
 * @brief Gets the BLE SPI notify command.
 *
 * @return uint8_t The notify command.
 */
static uint8_t get_ble_spi_notify_cmd(void)
{
    TRACE;
    return(ble_spi_notify_cmd_buf[2]);
}

/**
 * @brief Gets the BLE SPI notify command data.
 *
 * @return uint8_t* Pointer to the command data.
 */
static uint8_t *get_ble_spi_notify_cmd_data(void)
{
    TRACE;
    return(&(ble_spi_notify_cmd_buf[3]));
}

/**
 * @brief Sets the BLE SPI notify command data header.
 *
 * @param cmd The command.
 * @param len The length of the data.
 */
static void set_ble_spi_notify_cmd_data_header(uint8_t cmd, uint8_t len)
{
    TRACE;
    ble_spi_notify_cmd_buf[0] = SPI_BLE_PACKET_HEADER;
    ble_spi_notify_cmd_buf[1] = len + 1;
    ble_spi_notify_cmd_buf[2] = cmd;
}

/**
 * @brief Gets the BLE SPI notify command data length.
 *
 * @return uint8_t The length of the data.
 */
uint8_t get_ble_spi_notify_cmd_data_length(void)
{
    TRACE;
    return(ble_spi_notify_cmd_buf[1]);
}

/**
 * @brief Gets the BLE SPI notify data buffer.
 *
 * @return uint8_t* Pointer to the buffer.
 */
uint8_t *get_ble_spi_notify_data(void)
{
    TRACE;
    return(ble_spi_notify_cmd_buf);
}

/**
 * @brief Calculates the checksum of a buffer.
 *
 * @param buf Pointer to the buffer.
 * @param len Length of the buffer.
 * @return uint8_t The checksum.
 */
uint8_t buffer_get_sum(uint8_t *buf, int len)
{
    TRACE;
    int i;
    uint8_t chksum = 0;

    for(i = 1; i < len; i++)
    {
        chksum += buf[i];
    }

    return chksum;
}

/**
 * @brief Requests Bluetooth information from the BLE module.
 */
void k715bt_send_ble_req_bt_info(void)
{
    k715bt_send_spi_extend_single_packet(KBD_CMD_GET_INFO, NULL, 0);
}

/**
 * @brief Requests the Bluetooth name from the BLE module.
 */
void k715bt_send_ble_req_bt_name(void)
{
    k715bt_send_spi_extend_single_packet(KBD_CMD_BT_GETNAME, NULL, 0);
}

/**
 * @brief Requests the BLE firmware version from the MCU.
 */
void k715bt_send_mcu_req_ble_fwver(void)
{
    k715bt_send_spi_extend_single_packet(KBD_CMD_MCU_REQ_BT_FWVER, NULL, 0);
}

/**
 * @brief Switches the device mode on the BLE module.
 *
 * @param mode The mode to switch to.
 */
void k715bt_send_ble_switch_device_mode(uint8_t mode)
{
    k715bt_send_spi_extend_single_packet(KBD_CMD_MODE_SET, &mode, 1);
}

/**
 * @brief Starts the pairing process on the BLE module.
 *
 * @param pair_timeout Timeout for pairing.
 * @param adv_data Advertising data.
 * @param adv_data_len Length of advertising data.
 */
void k715bt_send_ble_pair(uint8_t pair_timeout, uint8_t *adv_data, uint8_t adv_data_len)
{
    k715bt_send_spi_extend_single_packet(KBD_CMD_BT_PAIR, NULL, 0);
}

/**
 * @brief Reconnects to the last connected Bluetooth device.
 *
 * @param reconn_timeout Timeout for reconnection.
 */
void k715bt_send_ble_reconnect_bt(uint8_t reconn_timeout)
{
    k715bt_send_spi_extend_single_packet(KBD_CMD_BT_BACK, &reconn_timeout, 1);
}

/**
 * @brief Disconnects the current Bluetooth connection.
 */
void k715bt_send_ble_disconnect_bt(void)
{
    k715bt_send_spi_extend_single_packet(KBD_CMD_BT_DISCONNECT, NULL, 0);
}

/**
 * @brief Sets the device name on the BLE module.
 *
 * @param dev_name Pointer to the device name string.
 * @param dev_name_len Length of the device name.
 */
void k715bt_send_ble_set_device_name(uint8_t *dev_name, uint8_t dev_name_len)
{
    k715bt_send_spi_extend_single_packet(KBD_CMD_BT_SETNAME, dev_name, dev_name_len);
}

/**
 * @brief Sets the Bluetooth channel on the BLE module.
 *
 * @param bt_channel The channel to set.
 */
void k715bt_send_ble_set_bt_channel(uint8_t bt_channel)
{
    k715bt_send_spi_extend_single_packet(KBD_CMD_BT_SETCHN, &bt_channel, 1);
}

/**
 * @brief Switches the Bluetooth channel.
 *
 * Updates the global configuration if the channel changes.
 *
 * @param bt_channel The channel to switch to.
 */
void k715bt_switch_channel(uint8_t bt_channel)
{
    if(g_config.bt_ch != bt_channel)
    {
        k715bt_send_ble_set_bt_channel(bt_channel);
        g_config.bt_ch = bt_channel;
    }
}

/**
 * @brief Sends a request and receives a response via SPI.
 *
 * @param cmd The command to send.
 * @param buf Buffer to store the response.
 * @param len Length of the buffer.
 * @param send_req_flag Flag to indicate if a request should be sent.
 * @return int The command received in response, or 0 on error.
 */
int ble_spi_req_recv(uint8_t cmd, uint8_t *buf, uint8_t len, uint8_t send_req_flag)
{
    TRACE;
    uint8_t data[MAX_BLE_SPI_RX_FRAME_MAX_SIZE];
    int ret;
    int i = 0;
    uint8_t next_sn, rx_len;
    int sum_err = 0;
    uint8_t length;

    spi_use_begin();
    if(buf)
    {
        memset(buf, 0, len);
    }

    if(cmd == KBD_CMD_GET_INFO)
    {
        k715bt_send_ble_req_bt_info();
        wait_ms(10);
    }
    else if(cmd == KBD_CMD_BT_GETNAME)
    {
        k715bt_send_ble_req_bt_name();
        wait_ms(10);
    }
    else if(cmd == KBD_CMD_MCU_REQ_BT_FWVER)
    {
        k715bt_send_mcu_req_ble_fwver();
        wait_ms(10);
    }
    else
    {
        k715bt_send_ble_req_bt_info();
        wait_ms(10);
    }

    spi_slave_start();

    length =  MAX_BLE_SPI_RX_FRAME_MAX_SIZE;
    next_sn = 1;
    rx_len = 0;

__SPI_RECEIVE:
    memset(data, 0, sizeof(data));
    ret = -1;

    ret = spi_receive(data, length);
    if(ret == SPI_STATUS_SUCCESS)
    {
        i = length;
        if(data[0] == SPI_BLE_PACKET_HEADER)
        {
            if((data[1] == 0) || (data[1] > MAX_SPI_BLE_SINGLE_PACKET_LOAD_LENGTH))
            {
                sum_err = 0xF0;
            }
            else
            {
                if(data[length - 1] == buffer_get_sum(&data[0], length - 1))
                {
                    if((cmd == KBD_CMD_UNKNOW) || ((data[2] & 0x7f) == cmd))
                    {
                        if((data[2] & 0x80) == 0x80)
                        {
                            if((data[3] & 0x7F) == next_sn)
                            {
                                next_sn += 1;

                                i = data[1] - 2;
                                if((rx_len + i) >= len)
                                {
                                    sum_err = 0;
                                    goto __EXIT;
                                }

                                if((i > 0) && (i < MAX_BLE_SPI_RX_FRAME_MAX_SIZE))
                                {
                                    memcpy(&buf[rx_len], &data[4], i);
                                }

                                rx_len += i;

                                if((data[3] & 0x80) == 0x80)
                                {
                                    cmd = data[2] & 0x7F;
                                }
                                else
                                {
                                    goto __SPI_RECEIVE;
                                }
                            }
                            else
                            {
                                sum_err = 0xF2;
                                goto __EXIT;
                            }
                        }
                        else
                        {
                            len = MIN(len, (data[1] - 1));
                            if(len)
                            {

                                memcpy(buf, &data[3], len);
                                rx_len += len;
                            }

                            if(cmd == KBD_CMD_UNKNOW)
                            {
                                cmd = data[2];
                            }

                        }

                        sum_err = 0;
                    }
                    else
                    {
                        sum_err = 0xF1;
                    }
                }
                else
                {
                    sum_err = 2;
                }
            }
        }
        else
        {
            sum_err = 0xFF;
        }
    }
    else
    {
        sum_err = 3;
    }

__EXIT:
    spi_slave_stop();
    spi_use_end();

    if(sum_err)
    {
        return 0;
    }
    else
    {
        set_ble_spi_notify_cmd_data_header((data[2] & 0x7f), rx_len);
        return(data[2] & 0x7f);
    }
}

static volatile uint8_t ble_spi_rx_reading = 0;

/**
 * @brief Reads device information from BLE SPI.
 *
 * @param cmd The command to send.
 * @param send_req_flag Flag to indicate if a request should be sent.
 * @return uint8_t The command received.
 */
uint8_t ble_spi_read_dev_info(uint8_t cmd, uint8_t send_req_flag)
{
    TRACE;
    uint8_t len = MAX_SPI_BLE_SINGLE_PACKET_LENGTH;
    uint8_t cnt = 0;
    spi_slave_dev_info_t dinf;
    int ret = 0;

    ble_spi_rx_reading = 1;

    memset(&dinf, 0, sizeof(dinf));
    if(cmd == KBD_CMD_GET_INFO)
    {
        ret = 0;
        len = MAX_SPI_BLE_SINGLE_PACKET_LENGTH;
        while(ret != cmd)
        {
            ret = ble_spi_req_recv(cmd, (uint8_t *)&dinf, len, send_req_flag);
            if(ret == cmd)
            {
                break;
            }

            if(++cnt < 20)
            {
                wait_ms(20);
            }
            else
            {
                cmd = KBD_CMD_INVALID;
                goto __BLE_RX_EXIT;
            }
        }

        if(cmd == KBD_CMD_GET_INFO)
        {
            goto __SPI_RX_DEV_INFO;
        }
        else
        {
            goto __BLE_RX_EXIT;
        }
    }
    else
    {
        uint8_t *rx_ptr;

        ret = 0;
        memset(ble_spi_notify_cmd_buf, 0, sizeof(ble_spi_notify_cmd_buf));
        rx_ptr = (uint8_t *) & (ble_spi_notify_cmd_buf[3]);
        len = MAX_BLE_SPI_NOTIFY_CMD_BUF_SIZE - 3;
        while(ret == 0)
        {
            ret = ble_spi_req_recv(cmd, rx_ptr, len, send_req_flag);
            if(ret)
            {
                break;
            }

            if(++cnt < 20)
            {
                wait_ms(20);
            }
            else
            {
                cmd = KBD_CMD_INVALID;
                goto __BLE_RX_EXIT;
            }
        }

        if(get_ble_spi_notify_cmd() == KBD_CMD_GET_INFO)
        {
            memcpy((uint8_t *)&dinf, rx_ptr, MAX_SPI_BLE_SINGLE_PACKET_LENGTH);
            goto __SPI_RX_DEV_INFO;
        }

        cmd = get_ble_spi_notify_cmd();
        goto __BLE_RX_EXIT;
    }

__SPI_RX_DEV_INFO:
    spi_salve_data.spi_read_length = sizeof(dinf);
    memcpy(spi_salve_data.spi_read_buf, (uint8_t *)&dinf, sizeof(dinf));

    if(is_bt_mode_enabled())
    {
        dev_info.CapsLock = dinf.CapsLock;
        dev_info.NumLock = dinf.NumLock;
        dev_info.ScrollLock = dinf.ScrollLock;

        update_caps_led();
    }

    dev_info.usbstate = dinf.state_usb;
    if(dinf.mode != 0xff)
    {
        dev_info.btstate = dinf.state_bt;
        dev_info.devmode = dinf.mode;
    }

    if(dinf.state_bt == BTSTATECONNECTED)
    {
        dev_info.btconnected = 1;
    }
    else
    {
        dev_info.btconnected = 0;
    }

    if(dinf.ch != 0xff)
    {
        if(g_config.bt_ch != dinf.ch)
        {
            g_config.bt_ch = dinf.ch;
        }
    }

    cmd = KBD_CMD_GET_INFO;

__BLE_RX_EXIT:
    ble_spi_rx_reading = 0;
    return(cmd);
}

/**
 * @brief Gets the Caps Lock state.
 *
 * @return int 1 if on, 0 if off, -1 if unknown/poweroff.
 */
static int get_caps_state(void)
{
    TRACE;
    int onoff;

    onoff = -1;
    if(now_mode == KBD_POWEROFF_MODE)
    {
        return 0;
    }

    if(is_bt_mode_enabled())
    {
        onoff = dev_info.CapsLock;
    }
    else
    {
        if(is_usb_mode_enabled())
        {
            if(host_keyboard_led_state().caps_lock)
            {
                onoff = 1;
            }
            else
            {
                onoff = 0;
            }
        }
    }

    return(onoff);
}

/**
 * @brief Updates the Caps Lock LED.
 */
void update_caps_led(void)
{
    TRACE;
    int onoff;

    if(now_mode == KBD_POWEROFF_MODE)
    {
        return;
    }

    onoff = get_caps_state();
    if(onoff == 1)
    {
        set_caps_lock_on();
    }
    else if(onoff == 0)
    {
        set_caps_lock_off();
    }
}

/**
 * @brief Reads and updates the device mode from BLE SPI.
 *
 * @return int 0 on success, negative on error.
 */
int ble_spi_read_and_update_dev_mode(void)
{
    TRACE;
    int ret;
    unsigned short cnt;

    cnt = 0;
    while(1)
    {
        ret = ble_spi_read_dev_info(KBD_CMD_GET_INFO, 1);
        if(ret == KBD_CMD_INVALID)
        {
            wait_ms(100);
        }
        else
        {
            uint8_t mode;

            mode = get_dev_mode_detected();
            if(dev_info.devmode != mode)
            {
                k715bt_send_ble_switch_device_mode(mode);
                wait_ms(200);
                cnt &= 0xff00;
                cnt += 0x100;
                if(cnt > 0xa00)
                {
                    ret = -1;
                    break;
                }
            }
            else
            {
                ret = 0;
                break;
            }
        }

        if(((++cnt) & 0xff) >= 10)
        {

            ret = -2;
            break;
        }
    }

    return ret;
}

/**
 * @brief Checks if poweroff mode is enabled.
 *
 * @return bool True if poweroff mode is enabled, false otherwise.
 */
bool is_poweroff_mode_enabled(void)
{
    TRACE;
    if(now_mode == KBD_POWEROFF_MODE)
    {
        return(true);
    }

    return false;
}

/**
 * @brief Sends data via BLE SPI.
 *
 * @param cmd The command to send.
 * @param buf Pointer to the data buffer.
 * @param len Length of the data.
 * @return int 0 on success.
 */
static int ble_spi_send(uint8_t cmd, uint8_t *buf, uint8_t len)
{
    TRACE;
    int mul = 0;
    uint8_t data[MAX_BLE_SPI_TX_FRAME_MAX_SIZE], s = len;

    memset(data, 0, sizeof(data));
    while(len || (buf == NULL && len == 0))
    {
        int i = 0;

        data[i++] = SPI_BLE_PACKET_HEADER;
        if(len > MAX_SPI_BLE_SINGLE_PACKET_LENGTH || mul)
        {
            mul++;
            if(len > MAX_SPI_BLE_MUL_PACKET_LENGTH)
            {
                data[i++] = MAX_SPI_BLE_MUL_PACKET_LENGTH + 2;
                data[i++] = cmd | 0x80;
                data[i++] = mul;
                s = MAX_SPI_BLE_MUL_PACKET_LENGTH;
            }
            else
            {
                data[i++] = len + 2;
                data[i++] = cmd | 0x80;
                data[i++] = mul | 0x80;
                s = len;
            }
        }
        else
        {
            data[i++] = len + 1;
            data[i++] = cmd;
            s = len;
        }

        if(buf && s)
        {
            memcpy(&data[i], buf, s);
        }

        data[s + i] = buffer_get_sum(data, s + i);

        spi_slave_start();
        spi_transmit(data, s + i + 1);
        spi_slave_stop();

        if(len >= s)
        {
            len -= s;
            if(len)
            {
                wait_us(200);
            }
        }

        if(len == 0)
        {
            break;
        }

        if(buf)
        {
            buf += s;
        }
    }

    return 0;
}

/**
 * @brief Sends an extended single packet via BLE SPI.
 *
 * @param param_type The parameter type (command).
 * @param param_data Pointer to the parameter data.
 * @param param_len Length of the parameter data.
 */
void k715bt_send_spi_extend_single_packet(uint8_t param_type, uint8_t *param_data, uint8_t param_len)
{
    TRACE;
    if(is_poweroff_mode_enabled())
    {
        return;
    }

    if(ble_spi_rx_reading)
    {
        uint8_t loop_count = 0;
        while(1)
        {
            wait_ms(2);
            if(!ble_spi_rx_reading || (++loop_count >= 20))
            {
                break;
            }
        }
    }

    if(spi_inited)
    {
        ble_spi_send(param_type, param_data, param_len);
    }
}

/**
 * @brief Initializes user settings.
 */
static void init_user_settings(void)
{
    TRACE;
    memset(&g_config, 0, sizeof(g_config));

    g_config.magic_num = KB_MAGIC_NUMBER;
    g_config.kbd_dev_mode = 0xFF;
    g_config.bt_ch = BT_CHANNEL_1;
    g_config.bt_last_connected = BT_CHANNEL_INVALID;
}

/**
 * @brief Requests a BLE exchange.
 *
 * @param req_cmd The request command.
 * @param ack_cmd The expected acknowledge command.
 * @param data Buffer to store the response.
 * @param len Length of the buffer.
 * @return int 1 on success, 0 on failure.
 */
static int req_ble_exchange(uint8_t req_cmd, uint8_t ack_cmd, uint8_t *data, uint8_t len)
{
    TRACE;
    int ret = 0;
    uint8_t read_count = 0;

__LOOP_READ_CMD:
    memset(data, 0, len);
    ret = ble_spi_req_recv(req_cmd, data, len, 1);
    if(ret != ack_cmd)
    {
        read_count++;
        if(read_count < 20)
        {
            wait_ms(20);
            goto __LOOP_READ_CMD;
        }
        else
        {
            memset(data, 0, len);
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

#define BT_FW_VERINFO_LEN 19
static uint8_t bt_mac_addr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t bt_fw_verinfo[BT_FW_VERINFO_LEN + 1] = {0};

/**
 * @brief Gets the Bluetooth MAC address.
 *
 * @return uint8_t* Pointer to the MAC address.
 */
uint8_t *get_bt_mac_addr(void)
{
    TRACE;
    return(bt_mac_addr);
}

/**
 * @brief Gets the Bluetooth firmware version info.
 *
 * @param len Pointer to store the length of the version info.
 * @return uint8_t* Pointer to the version info.
 */
uint8_t *get_bt_fw_verinfo(uint8_t *len)
{
    TRACE;
    *len = BT_FW_VERINFO_LEN;
    return(bt_fw_verinfo);
}

/**
 * @brief Initializes and requests the Bluetooth version.
 *
 * @return int 0 on success.
 */
int init_req_bt_ver(void)
{
    TRACE;
    int ret;
    uint8_t data[48];

    ret = req_ble_exchange(KBD_CMD_MCU_REQ_BT_FWVER, KBD_CMD_MCU_REQ_BT_FWVER, data, sizeof(data));
    if(ret)
    {
        memset(bt_fw_verinfo, 0, sizeof(bt_fw_verinfo));
        memcpy(bt_fw_verinfo, data, BT_FW_VERINFO_LEN);
        memcpy(bt_mac_addr, &(data[BT_FW_VERINFO_LEN]), 6);

        dprintf("[%08lu] BT fw version:%s\r\n", timer_read32(), bt_fw_verinfo);
        dprintf("[%08lu] BT addr:%02X:%02X:%02X:%02X:%02X:%02X\r\n", timer_read32(), bt_mac_addr[0], bt_mac_addr[1], \
                  bt_mac_addr[2], bt_mac_addr[3], \
                  bt_mac_addr[4], bt_mac_addr[5]);
    }

    return 0;
}

/**
 * @brief Gets the active Bluetooth channel.
 *
 * @return uint8_t The active channel.
 */
uint8_t get_active_bt_chn(void)
{
    TRACE;
    return(g_config.bt_ch);
}

#define DEFAULT_BT_NAME "BLE Keyboard"
/**
 * @brief Sends the Bluetooth name with channel.
 *
 * @param chn The channel (unused).
 */
void send_bt_name_with_chn(uint8_t chn)
{
    TRACE;
    k715bt_send_ble_set_device_name((uint8_t *)DEFAULT_BT_NAME, strlen(DEFAULT_BT_NAME));
}

/**
 * @brief Marks Bluetooth as disconnected.
 */
void ble_mark_bt_disconnected(void)
{
    TRACE;
    dev_info.btconnected = 0;
}

/**
 * @brief Sets the device info Bluetooth timeout.
 *
 * @param timeout The timeout value.
 * @param flag Flag to indicate if timeout is absolute or relative.
 */
void set_dev_info_bt_timeout(uint32_t timeout, int flag)
{
    TRACE;
    if(timeout == 0)
    {
        if(flag)
        {
            dev_info.bt_timeout = timeout;
        }
        else
        {
            dev_info.bt_timeout = BT_BP_TIMEOUT - RF_TIMEOUT_OFFSET + get_time_ms();
        }
    }
    else
    {
        dev_info.bt_timeout = timeout;
    }
}

/**
 * @brief Sets the Bluetooth pair timeout.
 */
void set_dev_info_bt_pair_timeout(void)
{
    TRACE;
    set_dev_info_bt_timeout(0, 0);
}

/**
 * @brief Sets the Bluetooth reconnect timeout.
 */
void set_dev_info_bt_reconn_timeout(void)
{
    TRACE;
    set_dev_info_bt_timeout(BT_BACK_TIMEOUT - RF_TIMEOUT_OFFSET + get_time_ms(), 1);
}

/**
 * @brief Calculates the system tick difference.
 *
 * @param now Current time.
 * @param start_time Start time.
 * @param flag Flag (unused).
 * @return int 1 if difference >= 10, 0 otherwise.
 */
int sys_tk_diff(uint32_t now, uint32_t start_time, int flag)
{
    TRACE;
    int ret;

    if(now > start_time)
    {
        ret = 10;
    }
    else
    {
        ret = timer_elapsed32(start_time);
    }

    if(ret >= 10)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Checks if Bluetooth mode is enabled.
 *
 * @return bool True if enabled, false otherwise.
 */
bool is_bt_mode_enabled(void)
{
    TRACE;
    if(mode_bt_state)
    {
        return true;
    }

    return false;
}

/**
 * @brief Checks if USB mode is enabled.
 *
 * @return bool True if enabled, false otherwise.
 */
bool is_usb_mode_enabled(void)
{
    TRACE;
    if(now_mode == KBD_USB_MODE)
    {
        return true;
    }

    return false;
}

static uint8_t last_spi_notify_line_level = 0xFF;

/**
 * @brief Invalidates the SPI slave read line.
 */
void invlaid_read_spi_slave_line(void)
{
    TRACE;
    last_spi_notify_line_level = EXT_READ_SPI_SLAVE_READY;
}

/**
 * @brief Checks the SPI slave read line.
 *
 * @return int 1 if ready, 0 otherwise.
 */
int check_read_spi_slave_line(void)
{
    TRACE;
    uint8_t line_level;
    int ret = 0;

    line_level = palReadLine(EXT_READ_SPI_SLAVE_INT_PIN);
    if(line_level != last_spi_notify_line_level)
    {
        if(last_spi_notify_line_level != 0xFF)
        {
            last_spi_notify_line_level = line_level;
            if(line_level == EXT_READ_SPI_SLAVE_READY)
            {
                ret = 1;
            }
        }
        else
        {
            last_spi_notify_line_level = line_level;
            if(line_level == EXT_READ_SPI_SLAVE_READY)
            {
                ret = 1;
            }
        }
    }

    return ret;
}

/**
 * @brief Sends the MCU firmware version to BLE.
 */
static void send_mcu_fwver_to_ble(void)
{
    TRACE;
    uint8_t buf[14];

    memset(buf, 0, sizeof(buf));

    sprintf((char *)buf, "%d.%d.%d", (DEVICE_VER >> 8) & 0x000F, (DEVICE_VER >> 4) & 0x000F, DEVICE_VER & 0x000F);
    k715bt_send_spi_extend_single_packet(KBD_CMD_SEND_BT_MCU_FWVER, buf, strlen((char *)buf));
}

/**
 * @brief Handles BLE SPI slave command notifications.
 */
void ble_spi_slave_cmd_notify(void)
{
    TRACE;
    uint8_t rx_cmd;
    uint8_t *rx_data;

    rx_cmd = get_ble_spi_notify_cmd();
    rx_data = get_ble_spi_notify_cmd_data();

    switch(rx_cmd)
    {
        case KBD_CMD_MCU_RECV_BT_FWVER:
        {
            uint8_t offset = BT_FW_VERINFO_LEN;

            memset(bt_fw_verinfo, 0, sizeof(bt_fw_verinfo));
            memcpy(bt_fw_verinfo, rx_data, sizeof(bt_fw_verinfo) - 1);
            memcpy(bt_mac_addr, &(rx_data[offset]), 6);

            dprintf("[%08lu] BT fw version:%s\r\n", timer_read32(), bt_fw_verinfo);
            dprintf("[%08lu] BT addr:%02X:%02X:%02X:%02X:%02X:%02X\r\n", timer_read32(), bt_mac_addr[0], bt_mac_addr[1], \
                      bt_mac_addr[2], bt_mac_addr[3], \
                      bt_mac_addr[4], bt_mac_addr[5]);

            break;
        }

        case KBD_CMD_BT_REQ_MCU_FWVER:
        {
            send_mcu_fwver_to_ble();
            break;
        }

        default:
        {
            break;
        }
    }
}

static void handle_bt_disconnect(void) {
    char b = dev_info.btback;
    char p = dev_info.btpair;

    if(g_config.bt_ch == 0)
    {
        g_config.bt_ch = BT_CHANNEL_1;
    }

    ble_mark_bt_disconnected();
    if(dev_info.bt_timeout && sys_tk_diff(now_ms, dev_info.bt_timeout, 0) && (p || b))
    {
        dev_info.bt_timeout = 0;
        if(dev_info.btback)
        {
            dev_info.btback = 0;
        }

        if(dev_info.btpair)
        {
            dev_info.btpair = 0;
            if(g_config.bt_last_connected == g_config.bt_ch)
            {
                if(g_config.bt_used & (1 << (g_config.bt_ch - 1)))
                {
                    dev_info.btback = 1;
                    k715bt_send_ble_reconnect_bt(BT_BACK_TIMEOUT / 1000);
                    set_dev_info_bt_reconn_timeout();
                }
            }
        }
    }
    else if(p && dev_info.bt_timeout)
    {
        dev_info.btpair = 1;
        k715bt_send_ble_pair(BT_BP_TIMEOUT / 1000, NULL, 0);
    }
    else if(b && dev_info.bt_timeout)
    {
        dev_info.btback = 1;
        k715bt_send_ble_reconnect_bt(BT_BACK_TIMEOUT / 1000);
    }
    else
    {
        if(g_config.bt_used & (1 << (g_config.bt_ch - 1)))
        {
            dev_info.btback = 1;
            dev_info.btpair = 0;
            k715bt_send_ble_reconnect_bt(BT_BACK_TIMEOUT / 1000);
            set_dev_info_bt_reconn_timeout();
        }
        else
        {
            dev_info.btpair = 1;
            dev_info.btback = 0;
            k715bt_send_ble_pair(BT_BP_TIMEOUT / 1000, NULL, 0);
            set_dev_info_bt_pair_timeout();
        }
    }
}

static void handle_bt_discover(void) {
    ble_mark_bt_disconnected();

    if(dev_info.btpair == 0)
    {
        dev_info.btpair = 1;
        if(dev_info.bt_timeout == 0)
        {
            set_dev_info_bt_pair_timeout();
        }
    }

    dev_info.btback = 0;
}

static void handle_bt_connected(void) {
    if((g_config.bt_used & (1 << (g_config.bt_ch - 1))) == 0
            && dev_info.btback == 0
            && dev_info.btpair == 0)
    {
        ble_mark_bt_disconnected();

        dev_info.btpair = 1;
        k715bt_send_ble_pair(BT_BP_TIMEOUT / 1000, NULL, 0);
        set_dev_info_bt_pair_timeout();
    }
    else
    {
        dev_info.btback = 0;
        dev_info.btpair = 0;
        dev_info.bt_timeout = 0;
        dev_info.btconnected = 1;
        g_config.bt_last_connected = g_config.bt_ch;
        g_config.bt_used |= (1 << (g_config.bt_ch - 1));
    }
}

static void handle_bt_reconnect(void) {
    ble_mark_bt_disconnected();

    if(dev_info.btback == 0)
    {
        if((g_config.bt_used & (1 << (g_config.bt_ch - 1))) == 0)
        {
            dev_info.btpair = 1;
            k715bt_send_ble_pair(BT_BP_TIMEOUT / 1000, NULL, 0);
            set_dev_info_bt_pair_timeout();
        }
        else
        {
            dev_info.btback = 1;
            if(dev_info.bt_timeout == 0)
            {
                set_dev_info_bt_reconn_timeout();
            }

            dev_info.btpair = 0;
        }
    }
}

/**
 * @brief Decodes BLE SPI slave data.
 *
 * @return uint32_t Return value indicating status or command.
 */
uint32_t ble_spi_slave_data_decode(void)
{
    TRACE;
    uint8_t cmd;

    cmd = ble_spi_read_dev_info(KBD_CMD_UNKNOW, 0);

    if(cmd == KBD_CMD_INVALID)
    {
        return 0xFFFFFFFF;
    }

    if(cmd != KBD_CMD_GET_INFO)
    {
        return SPI_SLAVE_NOTIFY_CMD_RETURN_VALUE;
    }

    if(dev_info.devmode == KBD_BT_MODE)
    {
        if(dev_info.btstate == BTSTATEDISCONN)
        {
            handle_bt_disconnect();
        }
        else if(dev_info.btstate == BTSTATEDISCOVER)
        {
            handle_bt_discover();
        }
        else if(dev_info.btstate == BTSTATECONNECTED)
        {
            handle_bt_connected();
        }
        else if(dev_info.btstate == BTSTATERECONN)
        {
            handle_bt_reconnect();
        }
    }

    return 0;
}

/**
 * @brief Checks and reads SPI data.
 *
 * Called from bluetooth_task.
 */
void check_read_spi_data(void)
{
    TRACE;
    if(!is_bt_mode_enabled())
    {
        return;
    }

    if(check_read_spi_slave_line())
    {
        uint32_t ret = 0;

        ret = ble_spi_slave_data_decode();
        if(ret == SPI_SLAVE_NOTIFY_CMD_RETURN_VALUE)
        {
            ble_spi_slave_cmd_notify();
            invlaid_read_spi_slave_line();
        }
        else if(ret != 0xFFFFFFFF)
        {
            invlaid_read_spi_slave_line();
        }
    }
}

/**
 * @brief Starts Bluetooth pairing on a specific channel.
 *
 * @param chn The channel to pair on.
 */
void k715_bt_start_pair(uint8_t chn)
{
    TRACE;
    ble_mark_bt_disconnected();

    k715bt_switch_channel(chn);

    dev_info.btpair = 1;
    dev_info.btback = 0;
    set_dev_info_bt_pair_timeout();
    k715bt_send_ble_pair(BT_BP_TIMEOUT_MS, NULL, 0);
}

/**
 * @brief Initializes the BLE SPI interface and device state.
 *
 * Called from k715_bt_init.
 */
void k715_ble_spi_init(void)
{
    TRACE;
    spi_init();
    spi_inited = 1;

    wait_ms(500);

    dev_mode_detect = now_mode;

    init_ext_read_spi_slave_gpio();

    init_user_settings();
    kbd_dev_mode = g_config.kbd_dev_mode;

    if(now_mode == KBD_BT_MODE)
    {
        mode_bt_state = 1;
    }
    else
    {
        mode_bt_state = 0;
    }

    init_dev_info();
    wait_ms(500);
    set_IS31FL3733_normal_mode();

    if(now_mode != KBD_POWEROFF_MODE)
    {
        ble_spi_read_and_update_dev_mode();
    }

    init_req_bt_ver();

    if(now_mode == KBD_BT_MODE)
    {
        send_bt_name_with_chn(get_active_bt_chn());
        wait_ms(20);
    }

    if(now_mode != KBD_POWEROFF_MODE)
    {
        k715bt_send_ble_switch_device_mode(now_mode);
        if(now_mode != kbd_dev_mode)
        {
            if(now_mode == KBD_BT_MODE)
            {
                wait_ms(500);
            }
        }
    }

    if(now_mode != kbd_dev_mode)
    {
        kbd_dev_mode = now_mode;
        g_config.kbd_dev_mode = kbd_dev_mode;
    }

    if(now_mode != KBD_USB_MODE)
    {
        if(now_mode == KBD_BT_MODE)
        {
            if(g_config.bt_used & (1 << (g_config.bt_ch - 1)))
            {
                if(!dev_info.btconnected)
                {
                    ble_mark_bt_disconnected();

                    k715bt_send_ble_set_bt_channel(g_config.bt_ch);
                    wait_ms(20);

                    dev_info.btback = 1;
                    dev_info.btpair = 0;
                    k715bt_send_ble_reconnect_bt(BT_BACK_TIMEOUT / 1000);
                    set_dev_info_bt_reconn_timeout();
                }
                else
                {
                    uint8_t b;

                    b = g_config.bt_used;
                    dev_info.btback = 0;
                    dev_info.btpair = 0;
                    dev_info.bt_timeout = 0;
                    dev_info.btconnected = 1;
                    g_config.bt_last_connected = g_config.bt_ch;
                    g_config.bt_used |= (1 << (g_config.bt_ch - 1));

                    if(b != g_config.bt_used)
                    {
                    }
                }
            }
            else
            {
                ble_mark_bt_disconnected();

                k715bt_send_ble_set_bt_channel(g_config.bt_ch);
                wait_ms(20);

                dev_info.btpair = 1;
                dev_info.btback = 0;
                k715bt_send_ble_pair(BT_BP_TIMEOUT / 1000, NULL, 0);
                set_dev_info_bt_pair_timeout();
            }
        }
    }
}
