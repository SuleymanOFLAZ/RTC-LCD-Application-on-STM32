/*
 * ds1307.c
 *
 *  Created on: Jul 11, 2021
 *      Author: suley
 */
#include "ds1307.h"
#include <string.h>

I2C_Handle_t g_ds1307I2CHandle;

#define I2C_MY_ADDR  0x66;

static void ds1307_i2c_pin_config(void);
static void ds1307_i2c_init(void);
static uint8_t get_bcd_value(uint8_t value);
static uint8_t reconvert_from_bcf(uint8_t bcd);

uint8_t ds1307_init(void)
{
	uint8_t pTxBuffer[2] = {DS1307_ADDR_SEC, 0};
	uint8_t pRxBuffer = 0;

	// Initialize the I2C Pins
	ds1307_i2c_pin_config();

	// Initialize the I2C Peripheral
	I2C_PeriClockControl(DS1307_I2C, ENABLE);
	ds1307_i2c_init();

	// Enable the I2C Peripheral
	I2C_PeripheralControl(DS1307_I2C, ENABLE);

	// Set the CH bit on DS1307 for enabling clock count
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&pTxBuffer, 2, DS1307_I2C_ADDRESS, I2C_SR_NO);

	// Read back to CH (Clock Halt) bit for understand that operation was successful
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&pTxBuffer, 1, DS1307_I2C_ADDRESS, I2C_SR_YES);
	I2C_MasterReceiveData(g_ds1307I2CHandle.pI2C, &pRxBuffer, 1, DS1307_I2C_ADDRESS, I2C_SR_NO);

	pRxBuffer = (pRxBuffer >> 7) & 1;

	return pRxBuffer;
	// Retrun 1: Init failed
	// Return 0: Init success
}

void ds1307_set_current_time(RTC_time_t *rtc_time)
{
	uint8_t pTxBuffer[2] = {0, 0};

	// 1. Write seconds data
	pTxBuffer[0] = DS1307_ADDR_SEC;
	pTxBuffer[1] = get_bcd_value(rtc_time->seconds) & 0xDF;
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&pTxBuffer, 2, DS1307_I2C_ADDRESS, I2C_SR_NO);

	// 2. Write minutes data
	pTxBuffer[0] = DS1307_ADDR_MIN;
	pTxBuffer[1] = get_bcd_value(rtc_time->minutes) & 0x7F;
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&pTxBuffer, 2, DS1307_I2C_ADDRESS, I2C_SR_NO);

	// 3. Write hours data
	pTxBuffer[0] = DS1307_ADDR_HOURS;
	pTxBuffer[1] = get_bcd_value(rtc_time->hours) & 0x3F;
	if(rtc_time->time_format == DS1307_TIME_FORMAT_24HRS)
	{
		; // Already ready to write
	}
	else
	{
		pTxBuffer[1] |= (1 << 6);
		if(rtc_time->time_format == DS1307_TIME_FORMAT_12HRS_AM)
		{
			pTxBuffer[1] &= (1 << 5);
		}
		else if(rtc_time->time_format == DS1307_TIME_FORMAT_12HRS_PM)
		{
			pTxBuffer[1] |= ~(1 << 5);
		}
	}
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&pTxBuffer, 2, DS1307_I2C_ADDRESS, I2C_SR_NO);

}
void ds1307_get_current_time(RTC_time_t *rtc_time)
{
	uint8_t RxData = 0;
	uint8_t TxData = 0;

	// 1. Read seconds data
	TxData = DS1307_ADDR_SEC;
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&TxData, 1, DS1307_I2C_ADDRESS, I2C_SR_YES);
	I2C_MasterReceiveData(g_ds1307I2CHandle.pI2C, &RxData, 1, DS1307_I2C_ADDRESS, I2C_SR_NO);
	rtc_time->seconds = reconvert_from_bcf((RxData & 0x7F));

	// 2. Read minutes data
	TxData = DS1307_ADDR_MIN;
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&TxData, 1, DS1307_I2C_ADDRESS, I2C_SR_YES);
	I2C_MasterReceiveData(g_ds1307I2CHandle.pI2C, &RxData, 1, DS1307_I2C_ADDRESS, I2C_SR_NO);
	rtc_time->minutes = reconvert_from_bcf(RxData);

	// 3. Read hours data
	TxData = DS1307_ADDR_HOURS;
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&TxData, 1, DS1307_I2C_ADDRESS, I2C_SR_YES);
	I2C_MasterReceiveData(g_ds1307I2CHandle.pI2C, &RxData, 1, DS1307_I2C_ADDRESS, I2C_SR_NO);
	if(!(RxData & (1 << 6)))
	{
		rtc_time->hours= reconvert_from_bcf((RxData & 0x3F));
		rtc_time->time_format = DS1307_TIME_FORMAT_24HRS;
	}
	else
	{
		if(RxData & (1 << 5))
		{
			rtc_time->hours= reconvert_from_bcf((RxData & 0x1F));
			rtc_time->time_format = DS1307_TIME_FORMAT_12HRS_PM;
		}
		else
		{
			rtc_time->hours= reconvert_from_bcf((RxData & 0x1F));
			rtc_time->time_format = DS1307_TIME_FORMAT_12HRS_AM;
		}
	}
}

void ds1307_set_current_date(RTC_date_t *rtc_date)
{
	uint8_t pTxBuffer[2] = {0, 0};

	// 1. Write day data
	pTxBuffer[0] = DS1307_ADDR_DAY;
	pTxBuffer[1] = rtc_date->day & 0x07;
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&pTxBuffer, 2, DS1307_I2C_ADDRESS, I2C_SR_NO);

	// 2. Write date data
	pTxBuffer[0] = DS1307_ADDR_DATE;
	pTxBuffer[1] = get_bcd_value(rtc_date->date) & 0x3F;
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&pTxBuffer, 2, DS1307_I2C_ADDRESS, I2C_SR_NO);

	// 3. Write month data
	pTxBuffer[0] = DS1307_ADDR_MONTH;
	pTxBuffer[1] = get_bcd_value(rtc_date->month) & 0x1F;
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&pTxBuffer, 2, DS1307_I2C_ADDRESS, I2C_SR_NO);

	// 4. Write year data
	pTxBuffer[0] = DS1307_ADDR_YEAR;
	pTxBuffer[1] = get_bcd_value(rtc_date->year);
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&pTxBuffer, 2, DS1307_I2C_ADDRESS, I2C_SR_NO);
}
void ds1307_get_current_date(RTC_date_t *rtc_date)
{
	uint8_t RxData = 0;
	uint8_t TxData = 0;

	// 1. Read day data
	TxData = DS1307_ADDR_DAY;
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&TxData, 1, DS1307_I2C_ADDRESS, I2C_SR_YES);
	I2C_MasterReceiveData(g_ds1307I2CHandle.pI2C, &RxData, 1, DS1307_I2C_ADDRESS, I2C_SR_NO);
	rtc_date->day = RxData & 0x07;

	// 2. Read date data
	TxData = DS1307_ADDR_DATE;
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&TxData, 1, DS1307_I2C_ADDRESS, I2C_SR_YES);
	I2C_MasterReceiveData(g_ds1307I2CHandle.pI2C, &RxData, 1, DS1307_I2C_ADDRESS, I2C_SR_NO);
	rtc_date->date = reconvert_from_bcf((RxData & 0x3F));

	// 3. Read month data
	TxData = DS1307_ADDR_MONTH;
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&TxData, 1, DS1307_I2C_ADDRESS, I2C_SR_YES);
	I2C_MasterReceiveData(g_ds1307I2CHandle.pI2C, &RxData, 1, DS1307_I2C_ADDRESS, I2C_SR_NO);
	rtc_date->month = reconvert_from_bcf((RxData & 0x1F));

	// 4. Read year data
	TxData = DS1307_ADDR_YEAR;
	I2C_MasterSendData(&g_ds1307I2CHandle, (uint8_t *)&TxData, 1, DS1307_I2C_ADDRESS, I2C_SR_YES);
	I2C_MasterReceiveData(g_ds1307I2CHandle.pI2C, &RxData, 1, DS1307_I2C_ADDRESS, I2C_SR_NO);
	rtc_date->year = reconvert_from_bcf(RxData);
}

static void ds1307_i2c_pin_config(void)
{
	// Enable GPIO clock
	GPIO_PeriClockControl(DS1307_I2C_GPIO_PORT, ENABLE);

	GPIO_Handle_t i2c;
	memset(&i2c, 0, sizeof(i2c));

	/*
	 * I2C_SCL -> PB6
	 * I2C_SDA -> PB7
	 */

	i2c.pGPIOx = DS1307_I2C_GPIO_PORT;
	i2c.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_ALTFUN_AF4;
	i2c.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	i2c.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_OPENDRAIN;
	i2c.GPIO_PinConfig.GPIO_PinPuPdControl = DS1307_I2C_PD;
	i2c.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

	i2c.GPIO_PinConfig.GPIO_PinNumber = DS1307_I2C_SDA_PIN;
	GPIO_Init(&i2c);

	i2c.GPIO_PinConfig.GPIO_PinNumber = DS1307_I2C_SCL_PIN;
	GPIO_Init(&i2c);
}

static void ds1307_i2c_init(void)
{
	g_ds1307I2CHandle.pI2C = DS1307_I2C;

	g_ds1307I2CHandle.I2C_Config.I2C_ACKControl = I2C_ACK_EN;
	g_ds1307I2CHandle.I2C_Config.I2C_DeviceAddress = I2C_MY_ADDR
	g_ds1307I2CHandle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
	g_ds1307I2CHandle.I2C_Config.I2C_SCLSpeed = DS1307_I2C_SPEED;

	I2C_Init(&g_ds1307I2CHandle);
}

static uint8_t get_bcd_value(uint8_t value)
{
	uint8_t temp = 0;
	temp = value/10;
	value %= 10;
	value |= (temp << 4);
	return value;
}

static uint8_t reconvert_from_bcf(uint8_t bcd)
{
	uint8_t temp = 0;

	temp = (bcd >> 4);
	temp *= 10;

	bcd &= 0x0F;
	temp += bcd;

	return temp;
}
