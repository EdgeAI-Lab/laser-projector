#include "stm32f10x_conf.h"
#include "systick.h"

/*
 * systick init
 *
 */
static __IO uint32_t uwTick;
void init_tick(void)
{

  /* Use systick as time base source and configure 1ms tick (default clock after Reset is MSI) */
    /*Configure the SysTick to have interrupt in 1ms time basis*/
  SysTick_Config(SystemCoreClock/1000000);

  /*Configure the SysTick IRQ priority */
  NVIC_SetPriority(SysTick_IRQn,NVIC_EncodePriority(NVIC_PriorityGroup_2, 3, 0));
	
}


/*
 * global var uwTick plus one
 *
 */
void HAL_IncTick(void)
{
  uwTick++;
}


/*
 * get the value of global var uwTick
 *
 */
uint32_t HAL_GetTick(void)
{
  return uwTick;
}

/*
 * delay n ms
 *
 */
void HAL_Delay(__IO uint32_t Delay)
{
  uint32_t tickstart = 0;
  tickstart = HAL_GetTick();
  while((HAL_GetTick() - tickstart) < Delay)
  {
  }
}

