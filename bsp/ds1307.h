/*
 * ds1307.h
 *
 *  Created on: Jul 11, 2021
 *      Author: suley
 */

#ifndef DS1307_H_
#define DS1307_H_

#include "stm32f407xx.h"


/* Register Addresses */
#define DS1307_ADDR_SEC			0x00
#define DS1307_ADDR_MIN			0x01
#define DS1307_ADDR_HOURS		0x02
#define DS1307_ADDR_DAY			0x03
#define DS1307_ADDR_DATE		0x04
#define DS1307_ADDR_MONTH		0x05
#define DS1307_ADDR_YEAR		0x06
#define DS1307_ADDR_CONT		0x07

#define DS1307_TIME_FORMAT_12HRS_AM		0
#define DS1307_TIME_FORMAT_12HRS_PM		1
#define DS1307_TIME_FORMAT_24HRS			2

#define DS1307_I2C_ADDRESS		0x68

#define MONDAY		1
#define TUESDAY		2
#define WEDNESDAY	3
#define THURSDAY	4
#define FRIDAY		5
#define SATURDAY	6
#define SUNDAY		7

typedef struct{
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t day;
}RTC_date_t;

typedef struct{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t time_format;
}RTC_time_t;

#define DS1307_INIT_SUCCESS		0
#define DS1307_INIT_ERROR		1

/* Application Configuration Items */
#define DS1307_I2C				I2C1
#define DS1307_I2C_SDA_PIN		GPIO_PIN_7
#define DS1307_I2C_SCL_PIN		GPIO_PIN_6
#define DS1307_I2C_GPIO_PORT	GPIOB
#define DS1307_I2C_SPEED		I2C_SCLSPEED_SM
#define DS1307_I2C_PD			GPIO_PUPD_NO

/* Function Prototypes */
uint8_t ds1307_init(void);

void ds1307_set_current_time(RTC_time_t *rtc_time);
void ds1307_get_current_time(RTC_time_t *rtc_time);

void ds1307_set_current_date(RTC_date_t *rtc_date);
void ds1307_get_current_date(RTC_date_t *rtc_date);


#endif /* DS1307_H_ */
