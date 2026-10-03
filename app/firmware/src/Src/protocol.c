#include "protocol.h"
#include "led_strip.h"
#include "led_effects.h"

#define MY_DEVICE_ID DEV_LIGHT

static RadioParser_t radio_parser;
uint16_t Radio_CRC16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;

    for(uint16_t i = 0; i < len; i++) {
        crc ^= data[i];

        for(uint8_t bit = 0; bit < 8; bit++) {
            if(crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

void RadioParser_Init(RadioParser_t *parser)
{
    parser->state = RADIO_RX_WAIT_SOF1;
    parser->data_pos = 0;
    parser->crc_pos = 0;
}

bool RadioParser_FeedByte(RadioParser_t *parser, uint8_t byte, RadioPacket_t *out)
{
    switch(parser->state) {
        case RADIO_RX_WAIT_SOF1:
            if(byte == RADIO_SOF1) {
            	parser->dbg_frame_bytes++;
            	parser->dbg_frame_bytes = 1;
                parser->state = RADIO_RX_WAIT_SOF2;
            }
            break;

        case RADIO_RX_WAIT_SOF2:
            if(byte == RADIO_SOF2) {
                parser->state = RADIO_RX_ID;
            } else if(byte == RADIO_SOF1) {
            	parser->dbg_frame_bytes = 1;
                parser->state = RADIO_RX_WAIT_SOF2;
            } else {
            	parser->dbg_frame_bytes = 0;
                parser->state = RADIO_RX_WAIT_SOF1;
            }
            break;

        case RADIO_RX_ID:
        	parser->dbg_frame_bytes++;
            parser->packet.id = byte;
            parser->crc_buf[0] = byte;
            parser->state = RADIO_RX_CMD;
            break;

        case RADIO_RX_CMD:
        	parser->dbg_frame_bytes++;
            parser->packet.cmd = byte;
            parser->crc_buf[1] = byte;
            parser->state = RADIO_RX_LEN;
            break;

        case RADIO_RX_LEN:
        	parser->dbg_frame_bytes++;
            parser->packet.len = byte;
            parser->crc_buf[2] = byte;

            if(parser->packet.len > RADIO_MAX_DATA) {
                parser->state = RADIO_RX_WAIT_SOF1;
            } else if(parser->packet.len == 0) {
                parser->state = RADIO_RX_CRC_LO;
            } else {
                parser->data_pos = 0;
                parser->state = RADIO_RX_DATA;
            }
            break;

        case RADIO_RX_DATA:
        	parser->dbg_frame_bytes++;
            parser->packet.data[parser->data_pos] = byte;
            parser->crc_buf[3 + parser->data_pos] = byte;

            parser->data_pos++;

            if(parser->data_pos >= parser->packet.len) {
                parser->state = RADIO_RX_CRC_LO;
            }
            break;

        case RADIO_RX_CRC_LO:
        	parser->dbg_frame_bytes++;
            parser->packet.crc = byte;
            parser->state = RADIO_RX_CRC_HI;
            break;

        case RADIO_RX_CRC_HI: {
        	parser->dbg_frame_bytes++;
            parser->packet.crc |= ((uint16_t)byte << 8);

            uint16_t calc = Radio_CRC16(
                parser->crc_buf,
                3 + parser->packet.len
            );

            parser->state = RADIO_RX_WAIT_SOF1;

            if(calc == parser->packet.crc) {
                *out = parser->packet;
                return true;
            }

            break;
        }

        default:
            parser->state = RADIO_RX_WAIT_SOF1;
            break;
    }

    return false;
}


void Radio_Init(void)
{
    RadioParser_Init(&radio_parser);
}

void Radio_HandlePacket(const RadioPacket_t *pkt)
{
    if(pkt->id != MY_DEVICE_ID && pkt->id != DEV_BROADCAST) {
        return;
    }

    switch(pkt->cmd) {
        case CMD_LED_OFF:
        	LED_EFFECT_SetMode(LED_MODE_OFF);
            LED_STRIP_Clear();
            LED_STRIP_Show();
            break;

        case CMD_LED_FILL:
            if(pkt->len >= 4) {
            	LED_EFFECT_SetMode(LED_MODE_FILL);

                LED_EFFECT_SetColor(pkt->data[0], pkt->data[1], pkt->data[2]);
                LED_STRIP_SetBrightness(pkt->data[3]);
                LED_STRIP_Fill(pkt->data[0], pkt->data[1], pkt->data[2]);
                LED_STRIP_Show();
            }
            break;

        case CMD_LED_BREATHE:
            if(pkt->len >= 5) {
            	LED_EFFECT_SetMode(LED_MODE_BREATHE);
            	LED_EFFECT_SetColor(pkt->data[0], pkt->data[1], pkt->data[2]);
                LED_EFFECT_SetPeriod(pkt->data[3] | ((uint16_t)pkt->data[4] << 8));

            }
            break;

        case CMD_LED_DOT:
            if(pkt->len >= 4) {
            	LED_EFFECT_SetMode(LED_MODE_DOT);
            	LED_EFFECT_SetColor(pkt->data[0], pkt->data[1], pkt->data[2]);

                LED_EFFECT_SetSpeed(pkt->data[3]);

            }
            break;
        case CMD_LED_SCENE_BEGIN:
            if(pkt->len >= 5) {
                uint8_t brightness = pkt->data[0];
                uint8_t bg_mode    = pkt->data[1];
                uint8_t bg_r       = pkt->data[2];
                uint8_t bg_g       = pkt->data[3];
                uint8_t bg_b       = pkt->data[4];

                LED_EFFECT_SetMode(LED_MODE_SCENE);

                LED_STRIP_SetBrightness(brightness);

                if(bg_mode) {
                    LED_STRIP_Fill
					(bg_r, bg_g, bg_b);
                } else {
                    LED_STRIP_Clear();
                }
            }
            break;
        case CMD_LED_PIXEL_SET:
            if(pkt->len >= 5) {
                uint16_t index = (uint16_t)pkt->data[0] |
                                 ((uint16_t)pkt->data[1] << 8);

                uint8_t r = pkt->data[2];
                uint8_t g = pkt->data[3];
                uint8_t b = pkt->data[4];

                if(index < LED_COUNT) {
                    LED_STRIP_SetPixel(index, r, g, b);
                }
            }
            break;
        case CMD_LED_SCENE_SHOW:
            LED_STRIP_Show();
            break;
        default:
            break;
    }
}

void Radio_Task(USART_TypeDef *USARTx)
{
    RadioPacket_t pkt;

    while(USART_Available(USARTx) > 0) {
        uint8_t byte = (uint8_t)USART_ReadByte(USARTx);

        if(RadioParser_FeedByte(&radio_parser, byte, &pkt)) {
            Radio_HandlePacket(&pkt);
        }
    }
}
