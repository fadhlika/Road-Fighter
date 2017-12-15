#ifndef __ST7735_H
#define __ST7735_H

#include "stm32f4xx_hal.h"
#include "spi.h"
#include "gpio.h"

// Screen resolution in normal orientation
#define scr_w         128
#define scr_h         160

// ST7735 A0 (Data/Command select) pin
#define ST7735_A0_PORT     GPIOB
#define ST7735_A0_PIN      GPIO_PIN_6    // PB12

// ST7735 RST (Reset) pin
#define ST7735_RST_PORT    GPIOC
#define ST7735_RST_PIN     GPIO_PIN_7    // PB14

// ST7735 CS (Chip Select) pin
#define ST7735_CS_PORT     GPIOB
#define ST7735_CS_PIN      GPIO_PIN_9    // PB13

// CS pin macros
#define CS_L() ST7735_CS_PORT->BSRR = (uint32_t)ST7735_CS_PIN << 16U
#define CS_H() ST7735_CS_PORT->BSRR = ST7735_CS_PIN

// A0 pin macros
#define A0_L() ST7735_A0_PORT->BSRR = (uint32_t)ST7735_A0_PIN << 16U
#define A0_H() ST7735_A0_PORT->BSRR = ST7735_A0_PIN

// RESET pin macros
#define RST_L() ST7735_RST_PORT->BSRR = (uint32_t)ST7735_RST_PIN << 16U
#define RST_H() ST7735_RST_PORT->BSRR = ST7735_RST_PIN

typedef enum {
	scr_normal = 0,
	scr_CW     = 1,
	scr_CCW    = 2,
	scr_180    = 3
} ScrOrientation_TypeDef;


extern uint16_t scr_width;
extern uint16_t scr_height;


void ST7735_write(uint8_t data);

uint16_t RGB565(uint8_t R,uint8_t G,uint8_t B);

void ST7735_Init(void);
void ST7735_AddrSet(uint16_t XS, uint16_t YS, uint16_t XE, uint16_t YE);
void ST7735_Orientation(ScrOrientation_TypeDef orientation);
void ST7735_Clear(uint16_t color);

void ST7735_Pixel(uint16_t X, uint16_t Y, uint16_t color);
void ST7735_HLine(uint16_t X1, uint16_t X2, uint16_t Y, uint16_t color);
void ST7735_VLine(uint16_t X, uint16_t Y1, uint16_t Y2, uint16_t color);
void ST7735_Line(int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t color);
void ST7735_Rect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t color);
void ST7735_FillRect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t color);

void ST7735_PutChar5x7(uint16_t X, uint16_t Y, uint8_t chr, uint16_t color);
void ST7735_PutStr5x7(uint8_t X, uint8_t Y, char *str, uint16_t color);
void ST7735_PutChar7x11(uint16_t X, uint16_t Y, uint8_t chr, uint16_t color, uint16_t bgcolor);
void ST7735_PutStr7x11(uint8_t X, uint8_t Y, char *str, uint16_t color, uint16_t bgcolor);

#endif
