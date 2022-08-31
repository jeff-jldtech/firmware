#include <pb_common.h>
#include <pb_decode.h>
#include <pb_encode.h>
#include <pb.h>

#include <siot_serial.pb.h>
#include <point.pb.h>
#include <timestamp.pb.h>

#include <PacketSerial.h>

/**
 * Calculate CRC sum on block of data.
 *
 * @param block Pointer to block of data.
 * @param blockLength Length of data in bytes;
 * @param crc Initial value (zero for xmodem). If block is split into
 * multiple segments, previous CRC is used as initial.
 *
 * @return Calculated CRC.
 */
uint16_t crc16_ccitt(const uint8_t block[],
                     size_t blockLength,
                     uint16_t crc);

void crc16_ccitt_table_init(void);

class CRC16
{
public:
    CRC16();
    uint16_t processByte(uint8_t data);
    uint16_t processBuffer(const char *data_p, uint16_t length);
    uint16_t getCrc() { return crc; };

private:
    uint16_t crc;
};

class CRC16x1
{
public:
    CRC16x1();
    uint16_t processByte(uint8_t data);
    uint16_t processBuffer(const char *data_p, uint16_t length);
    uint16_t getCrc() { return crc; };

private:
    uint16_t crc;
};

// NOTE: maximum receive buffer length in Uno default serial ISR is 64 bytes.

void test_crc()
{

    CRC16 crc;
    CRC16x1 crcx1;
    uint16_t length = 0;
    uint16_t test_crc = 0;

    static uint8_t test_buf[] = {0x00, 0x0a, 0x01, 0x74, 0x12, 0x13, 0x12, 0x01, 0x31, 0x2a,
                                 0x0b, 0x08, 0x80, 0x92, 0xb8, 0xc3, 0x98, 0xfe, 0xff, 0xff,
                                 0xff, 0x01, 0x42, 0x01, 0x30, 0x12, 0x13, 0x12, 0x01, 0x32,
                                 0x2a, 0x0b, 0x08, 0x80, 0x92, 0xb8, 0xc3, 0x98, 0xfe, 0xff,
                                 0xff, 0xff, 0x01, 0x42, 0x01, 0x30};

    length = sizeof(test_buf);
    delay(5000);
    Serial.println("CRC16 test -------");

    crc.processBuffer(test_buf, length); // 92);
    Serial.print("The resulting CRC16 (0x8408) is : 0x");
    Serial.println(crc.getCrc(), HEX);

    crcx1.processBuffer(test_buf, length); // 92);
    Serial.print("The resulting CRC16x1 (0x1021) is : 0x");
    Serial.println(crcx1.getCrc(), HEX);

    test_crc = 0;
    test_crc = crc16_ccitt(test_buf, (length), 0x0000);
    Serial.print("The resulting crc16_ccitt (0x8408)is : 0x");
    Serial.println(test_crc, HEX);
}

// the setup function runs once when you press reset or power the board
void setup()
{
    Serial.begin(115200);
    uint8_t buffer[256];
    char print_string[256];
    bool status;
    int i;

    crc16_ccitt_table_init(); // init table using 0x8408 polynomial

    test_crc();

    while (1)
    {
    }
}
//
void loop()
{
}

#define POLY 0x8408
#define POLYX1 0x1021

CRC16::CRC16()
{
    crc = 0;
}

uint16_t CRC16::processByte(uint8_t data)
{
    uint8_t i;

    crc = crc ^ ((uint16_t)data << 8);
    for (i = 0; i < 8; i++)
    {
        if (crc & 0x8000)
            crc = (crc << 1) ^ POLY;
        else
            crc <<= 1;
    }
    return crc;
}

uint16_t CRC16::processBuffer(const char *data_p, uint16_t length)
{
    /*  uint8_t i;
      uint16_t data;*/

    while (length--)
    {
        processByte(*data_p++);
        /*    data = *data_p++;
            crc = crc ^ ((uint16_t)data << 8);
            for (i = 0; i < 8; i++) {
              if (crc & 0x8000)
                crc = (crc << 1) ^ POLY;
              else
                crc <<= 1;
            }*/
    }
    return crc;
}

CRC16x1::CRC16x1()
{
    crc = 0;
}

uint16_t CRC16x1::processByte(uint8_t data)
{
    uint8_t i;

    crc = crc ^ ((uint16_t)data << 8);
    for (i = 0; i < 8; i++)
    {
        if (crc & 0x8000)
            crc = (crc << 1) ^ POLYX1;
        else
            crc <<= 1;
    }
    return crc;
}

uint16_t CRC16x1::processBuffer(const char *data_p, uint16_t length)
{
    /*  uint8_t i;
      uint16_t data;*/

    while (length--)
    {
        processByte(*data_p++);
        /*    data = *data_p++;
            crc = crc ^ ((uint16_t)data << 8);
            for (i = 0; i < 8; i++) {
              if (crc & 0x8000)
                crc = (crc << 1) ^ POLY;
              else
                crc <<= 1;
            }*/
    }
    return crc;
}

#if 1
/*
 * Calculation of CRC 16 CCITT polynomial, x^16 + x^12 + x^5 + 1.
 *
 * @file        crc16-ccitt.c
 * @ingroup     crc16-ccitt
 * @author      Janez Paternoster
 * @copyright   2012 - 2020 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#include "301/crc16-ccitt.h"

/* default configuration, see CO_config.h */
#ifndef CO_CONFIG_CRC16
#define CO_CONFIG_CRC16 (1)
#endif
#define CO_CONFIG_CRC16_ENABLE (1)

#if (CO_CONFIG_CRC16) & CO_CONFIG_CRC16_ENABLE
#if !((CO_CONFIG_CRC16)&CO_CONFIG_CRC16_EXTERNAL)

/*
 * CRC table calculated by the following algorithm:
 *
 * void crc16_ccitt_table_init(void){
 *     uint16_t i, j;
 *     for(i=0; i<256; i++){
 *         uint16_t crc = 0;
 *         uint16_t c = i << 8;
 *         for(j=0; j<8; j++){
 *             if((crc ^ c) & 0x8000) crc = (crc << 1) ^ 0x1021;
 *             else                   crc = crc << 1;
 *             c = c << 1;
 *         }
 *         crc16_ccitt_table[i] = crc;
 *     }
 * }
 */
static const uint16_t xcrc16_ccitt_table[256] = {
    0x0000U, 0x1021U, 0x2042U, 0x3063U, 0x4084U, 0x50A5U, 0x60C6U, 0x70E7U,
    0x8108U, 0x9129U, 0xA14AU, 0xB16BU, 0xC18CU, 0xD1ADU, 0xE1CEU, 0xF1EFU,
    0x1231U, 0x0210U, 0x3273U, 0x2252U, 0x52B5U, 0x4294U, 0x72F7U, 0x62D6U,
    0x9339U, 0x8318U, 0xB37BU, 0xA35AU, 0xD3BDU, 0xC39CU, 0xF3FFU, 0xE3DEU,
    0x2462U, 0x3443U, 0x0420U, 0x1401U, 0x64E6U, 0x74C7U, 0x44A4U, 0x5485U,
    0xA56AU, 0xB54BU, 0x8528U, 0x9509U, 0xE5EEU, 0xF5CFU, 0xC5ACU, 0xD58DU,
    0x3653U, 0x2672U, 0x1611U, 0x0630U, 0x76D7U, 0x66F6U, 0x5695U, 0x46B4U,
    0xB75BU, 0xA77AU, 0x9719U, 0x8738U, 0xF7DFU, 0xE7FEU, 0xD79DU, 0xC7BCU,
    0x48C4U, 0x58E5U, 0x6886U, 0x78A7U, 0x0840U, 0x1861U, 0x2802U, 0x3823U,
    0xC9CCU, 0xD9EDU, 0xE98EU, 0xF9AFU, 0x8948U, 0x9969U, 0xA90AU, 0xB92BU,
    0x5AF5U, 0x4AD4U, 0x7AB7U, 0x6A96U, 0x1A71U, 0x0A50U, 0x3A33U, 0x2A12U,
    0xDBFDU, 0xCBDCU, 0xFBBFU, 0xEB9EU, 0x9B79U, 0x8B58U, 0xBB3BU, 0xAB1AU,
    0x6CA6U, 0x7C87U, 0x4CE4U, 0x5CC5U, 0x2C22U, 0x3C03U, 0x0C60U, 0x1C41U,
    0xEDAEU, 0xFD8FU, 0xCDECU, 0xDDCDU, 0xAD2AU, 0xBD0BU, 0x8D68U, 0x9D49U,
    0x7E97U, 0x6EB6U, 0x5ED5U, 0x4EF4U, 0x3E13U, 0x2E32U, 0x1E51U, 0x0E70U,
    0xFF9FU, 0xEFBEU, 0xDFDDU, 0xCFFCU, 0xBF1BU, 0xAF3AU, 0x9F59U, 0x8F78U,
    0x9188U, 0x81A9U, 0xB1CAU, 0xA1EBU, 0xD10CU, 0xC12DU, 0xF14EU, 0xE16FU,
    0x1080U, 0x00A1U, 0x30C2U, 0x20E3U, 0x5004U, 0x4025U, 0x7046U, 0x6067U,
    0x83B9U, 0x9398U, 0xA3FBU, 0xB3DAU, 0xC33DU, 0xD31CU, 0xE37FU, 0xF35EU,
    0x02B1U, 0x1290U, 0x22F3U, 0x32D2U, 0x4235U, 0x5214U, 0x6277U, 0x7256U,
    0xB5EAU, 0xA5CBU, 0x95A8U, 0x8589U, 0xF56EU, 0xE54FU, 0xD52CU, 0xC50DU,
    0x34E2U, 0x24C3U, 0x14A0U, 0x0481U, 0x7466U, 0x6447U, 0x5424U, 0x4405U,
    0xA7DBU, 0xB7FAU, 0x8799U, 0x97B8U, 0xE75FU, 0xF77EU, 0xC71DU, 0xD73CU,
    0x26D3U, 0x36F2U, 0x0691U, 0x16B0U, 0x6657U, 0x7676U, 0x4615U, 0x5634U,
    0xD94CU, 0xC96DU, 0xF90EU, 0xE92FU, 0x99C8U, 0x89E9U, 0xB98AU, 0xA9ABU,
    0x5844U, 0x4865U, 0x7806U, 0x6827U, 0x18C0U, 0x08E1U, 0x3882U, 0x28A3U,
    0xCB7DU, 0xDB5CU, 0xEB3FU, 0xFB1EU, 0x8BF9U, 0x9BD8U, 0xABBBU, 0xBB9AU,
    0x4A75U, 0x5A54U, 0x6A37U, 0x7A16U, 0x0AF1U, 0x1AD0U, 0x2AB3U, 0x3A92U,
    0xFD2EU, 0xED0FU, 0xDD6CU, 0xCD4DU, 0xBDAAU, 0xAD8BU, 0x9DE8U, 0x8DC9U,
    0x7C26U, 0x6C07U, 0x5C64U, 0x4C45U, 0x3CA2U, 0x2C83U, 0x1CE0U, 0x0CC1U,
    0xEF1FU, 0xFF3EU, 0xCF5DU, 0xDF7CU, 0xAF9BU, 0xBFBAU, 0x8FD9U, 0x9FF8U,
    0x6E17U, 0x7E36U, 0x4E55U, 0x5E74U, 0x2E93U, 0x3EB2U, 0x0ED1U, 0x1EF0U};

/******************************************************************************/

uint16_t crc16_ccitt_table[256];

void crc16_ccitt_table_init(void)
{
    uint16_t i, j;
    for (i = 0; i < 256; i++)
    {
        uint16_t crc = 0;
        uint16_t c = i << 8;
        for (j = 0; j < 8; j++)
        {
            if ((crc ^ c) & 0x8000)
                crc = (crc << 1) ^ 0x8408;
            else
                crc = crc << 1;
            c = c << 1;
        }
        crc16_ccitt_table[i] = crc;
    }
}

void crc16_ccitt_single(uint16_t *crc, const uint8_t chr)
{
    uint8_t tmp = (uint8_t)(*crc >> 8U) ^ chr;
    *crc = (*crc << 8U) ^ crc16_ccitt_table[tmp];
}

/******************************************************************************/
uint16_t crc16_ccitt(const uint8_t block[],
                     size_t blockLength,
                     uint16_t crc)
{
    size_t i;

    for (i = 0U; i < blockLength; i++)
    {
        uint8_t tmp = (uint8_t)(crc >> 8U) ^ block[i];
        crc = (crc << 8U) ^ crc16_ccitt_table[tmp];
    }
    return crc;
}

#endif /* !((CO_CONFIG_CRC16) & CO_CONFIG_CRC16_EXTERNAL) */
#endif /* (CO_CONFIG_CRC16) & CO_CONFIG_CRC16_ENABLE */

#endif
