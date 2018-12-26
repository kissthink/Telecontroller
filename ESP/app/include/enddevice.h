/*
 * HC-SR501.h
 *
 *  Created on: 2015��5��15��
 *      Author: Administrator
 */

#ifndef APP_USER_HCSR501_H_
#define APP_USER_HCSR501_H_

#include "osapi.h"
#include "eagle_soc.h"
#include "c_types.h"
#include "gpio.h"
#include "c_types.h"
#include "mem.h" //ϵͳ������Ҫ��ͷ�ļ�

#define   GPIONUM                   4
#define   GPIO_EN_INPUT(gpio_no) 	gpio_output_set(0,0,0,gpio_no)



extern uint8 GetHCSR501(void);


#endif /* APP_USER_HCSR501_H_*/
