#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f10x.h"

extern uint16_t adc_value[50][16];
extern uint16_t after_filter[16];


void adc_init(void);

static void adc_gpio_init(void);

static void adc_configure(void);

static void adc_dma_configure(void);

void adc_value_filter(void);

uint16_t get_voltage(u16 advalue);

#endif

