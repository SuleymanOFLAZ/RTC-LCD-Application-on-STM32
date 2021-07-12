
#include "lcd.h"

static void write_4_bits(uint8_t value);
static void lcd_enable(void);
static void mdelay(uint32_t cnt);
static void udelay(uint32_t cnt);

void lcd_send_command(uint8_t cmd)
{
	/* RS=0 For LCD command */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, 0);
	/* RW=0, For writing the lcd */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, 0);

	// Write upper 4 bits of cmd
	write_4_bits(cmd >> 4);
	// Write lower 4 bits of cmd
	write_4_bits(cmd & 0x0F);
}

void lcd_init(void)
{
	// 1. Initialize the GPIOs
	GPIO_PeriClockControl(LCD_GPIO_PORT, ENABLE);

	GPIO_Handle_t lcd_gpio_handle;

	lcd_gpio_handle.pGPIOx = LCD_GPIO_PORT;
	lcd_gpio_handle.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUTPUT;
	lcd_gpio_handle.GPIO_PinConfig.GPIO_PinAltFunMode = 0;
	lcd_gpio_handle.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	lcd_gpio_handle.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPD_NO;
	lcd_gpio_handle.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

	lcd_gpio_handle.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_RW;
	GPIO_Init(&lcd_gpio_handle);

	lcd_gpio_handle.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_RS;
	GPIO_Init(&lcd_gpio_handle);

	lcd_gpio_handle.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_EN;
	GPIO_Init(&lcd_gpio_handle);

	lcd_gpio_handle.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D7;
	GPIO_Init(&lcd_gpio_handle);

	lcd_gpio_handle.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D6;
	GPIO_Init(&lcd_gpio_handle);

	lcd_gpio_handle.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D5;
	GPIO_Init(&lcd_gpio_handle);

	lcd_gpio_handle.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D4;
	GPIO_Init(&lcd_gpio_handle);

	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, 0);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, 0);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, 0);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, 0);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, 0);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, 0);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, 0);

	// 2. LCD Initialization steps

	mdelay(40);
	/* RS=0 For LCD command */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, 0);
	/* RW=0, For writing the lcd */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, 0);

	write_4_bits(0x3);

	mdelay(5);

	write_4_bits(0x3);

	udelay(150);

	write_4_bits(0x3);

	write_4_bits(0x2);

	// Function set command
	lcd_send_command(LCD_CMD_FUN_SET);

	// Display on cursor on blick on
	lcd_send_command(LCD_CMD_DSPON_CUROFF_BLKOFF);

	// Display Clear
	lcd_display_clear();

	// Entry mode set
	lcd_send_command(LCD_CMD_ENT_MODSET_INC_NSHI);
}

void lcd_print_string(char *message)
{
	do
	{
		lcd_send_char((uint8_t)*message++);
	}while(*message != '\0');
}

void lcd_send_char(uint8_t data)
{
	/* RS=1 For LCD data */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, 1);
	/* RW=0, For writing the lcd */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, 0);
	// Write upper 4 bits of data
	write_4_bits(data >> 4);
	// Write lower 4 bits of data
	write_4_bits(data & 0x0F);
}

/* Writes 4 bits data/command on D4-D7 */
static void write_4_bits(uint8_t value)
{
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, ((value >> 3) & 0x1));
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, ((value >> 2) & 0x1));
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, ((value >> 1) & 0x1));
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, ((value >> 0) & 0x1));

	lcd_enable();
}

static void lcd_enable(void)
{
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, 1);
	udelay(10);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, 0);
	udelay(100);	/* Execute time ~37 mikro seconds */
}

void lcd_display_clear(void)
{
	lcd_send_command(LCD_CMD_CLR_DSP);
	mdelay(2);
}

void lcd_display_return_home(void)
{
	lcd_send_command(LCD_CMD_RET_HOM);
	mdelay(2);
}

void lcd_set_cursor(uint8_t row, uint8_t column)
{
	column--;
	switch(row)
	{
	case 1:
		/* set cursor to 1st row address and add index */
		lcd_send_command(column |= 0x80);
		break;
	case 2:
		/* set cursor to 2nd row address and add index */
		lcd_send_command(column |= 0xC0);
		break;
	default:
		break;
	}
}

static void mdelay(uint32_t cnt)
{
	for(uint32_t i=0; i <(cnt*1000); i++);
}

static void udelay(uint32_t cnt)
{
	for(uint32_t i=0; i <(cnt*1); i++);
}
