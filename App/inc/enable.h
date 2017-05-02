#ifndef __ENABLE_H__
#define __ENABLE_H__

#define RED_ENABLE_PIN         GPIO_Pin_8
#define GERRN_ENABLE_PIN       GPIO_Pin_11 
#define BLUE_ENABLE_PIN        GPIO_Pin_12
#define GERRN532_ENABLE_PIN    GPIO_Pin_15

#define ENABLE_PORT            GPIOA

// enable 
#define RED_ENABLE()           GPIO_SetBits(ENABLE_PORT, RED_ENABLE_PIN)
#define GREEN_ENABLE()         GPIO_SetBits(ENABLE_PORT, GERRN_ENABLE_PIN)
#define BLUE_ENABLE()          GPIO_SetBits(ENABLE_PORT, BLUE_ENABLE_PIN)
#define GREEN532_ENABLE()      GPIO_SetBits(ENABLE_PORT, GERRN532_ENABLE_PIN)
#define ALL_POWER_ENABLE()     GPIO_SetBits(ENABLE_PORT, RED_ENABLE_PIN | GERRN_ENABLE_PIN | BLUE_ENABLE_PIN | GERRN532_ENABLE_PIN)

// disable
#define RED_DISABLE()          GPIO_ResetBits(ENABLE_PORT, RED_ENABLE_PIN)
#define GREEN_DISABLE()        GPIO_ResetBits(ENABLE_PORT, GERRN_ENABLE_PIN)
#define BLUE_DISABLE()         GPIO_ResetBits(ENABLE_PORT, BLUE_ENABLE_PIN)
#define GREEN532_DISABLE()     GPIO_ResetBits(ENABLE_PORT, GERRN532_ENABLE_PIN)
#define ALL_POWER_DISABLE()    GPIO_ResetBits(ENABLE_PORT, RED_ENABLE_PIN | GERRN_ENABLE_PIN | BLUE_ENABLE_PIN | GERRN532_ENABLE_PIN)

void enable_port_init(void);


#endif
