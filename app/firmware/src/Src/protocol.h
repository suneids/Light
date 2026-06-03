#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>
#include "../Inc/HAL_STM32F103C6T6/inc/usart.h"
#define RADIO_SOF1       0xAA
#define RADIO_SOF2       0x55

#define RADIO_MAX_DATA   128

#define DEV_LIGHT        0x01
#define DEV_GREENHOUSE   0x02
#define DEV_BROADCAST    0xFF

#define CMD_LED_OFF      0x10
#define CMD_LED_FILL     0x11
#define CMD_LED_BREATHE  0x12
#define CMD_LED_DOT      0x13
#define CMD_LED_SCENE_BEGIN  0x14
#define CMD_LED_PIXEL_SET    0x15
#define CMD_LED_SCENE_SHOW   0x16


typedef struct {
    uint8_t id;
    uint8_t cmd;
    uint8_t len;
    uint8_t data[RADIO_MAX_DATA];
    uint16_t crc;
} RadioPacket_t;

typedef enum {
    RADIO_RX_WAIT_SOF1,
    RADIO_RX_WAIT_SOF2,
    RADIO_RX_ID,
    RADIO_RX_CMD,
    RADIO_RX_LEN,
    RADIO_RX_DATA,
    RADIO_RX_CRC_LO,
    RADIO_RX_CRC_HI
} RadioRxState_t;

typedef struct {
    RadioRxState_t state;

    RadioPacket_t packet;

    uint8_t data_pos;

    uint8_t crc_buf[3 + RADIO_MAX_DATA];
    uint8_t crc_pos;
    uint16_t dbg_frame_bytes;
} RadioParser_t;

void RadioParser_Init(RadioParser_t *parser);
bool RadioParser_FeedByte(RadioParser_t *parser, uint8_t byte, RadioPacket_t *out);
void Radio_Init(void);
uint16_t Radio_CRC16(const uint8_t *data, uint16_t len);
void Radio_HandlePacket(const RadioPacket_t *pkt);
void Radio_Task(USART_TypeDef *USARTx);
#endif
