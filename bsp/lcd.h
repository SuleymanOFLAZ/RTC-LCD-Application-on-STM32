/*
 * lcd.h
 *
 *  Created on: Jul 11, 2021
 *      Author: suley
 */
#include "stm32f407xx.h"

#ifndef LCD_H_
#define LCD_H_

/* Application configurable items */
#define LCD_GPIO_PORT		GPIOD
#define LCD_GPIO_RS			GPIO_PIN_0
#define LCD_GPIO_RW			GPIO_PIN_1
#define LCD_GPIO_EN			GPIO_PIN_2
#define LCD_GPIO_D4			GPIO_PIN_3
#define LCD_GPIO_D5			GPIO_PIN_4
#define LCD_GPIO_D6			GPIO_PIN_5
#define LCD_GPIO_D7			GPIO_PIN_6

/* LCD Commands */
#define LCD_CMD_CLR_DSP					0x01
#define LCD_CMD_RET_HOM					0x02
#define LCD_CMD_ENT_MODSET_INC_SHI		0x07
#define LCD_CMD_ENT_MODSET_INC_NSHI		0x06
#define LCD_CMD_ENT_MODSET_DEC_SHI		0x05
#define LCD_CMD_ENT_MODSET_DEC_NSHI		0x04
#define LCD_CMD_DSPON_CURON_BLKON		0x0F
#define LCD_CMD_DSPON_CURON_BLKOFF		0x0E
#define LCD_CMD_DSPON_CUROFF_BLKOFF		0x0C
#define LCD_CMD_DSP_OFF					0x08
#define LCD_CMD_FUN_SET					0x28

void lcd_init(void);
void lcd_send_command(uint8_t cmd);
void lcd_send_char(uint8_t data);
void lcd_display_clear(void);
void lcd_print_string(char *message);
void lcd_display_return_home(void);
void lcd_set_cursor(uint8_t row, uint8_t column);

#endif /* LCD_H_ */
