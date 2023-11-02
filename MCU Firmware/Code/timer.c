#include "settings.h"

uint32_t stamp_us = 0;

// Use TIM2 as a time reference in poll mode
void Timer_Init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	TIM2->DIER = 0;						// disable Interrupts
	TIM2->SR   = 0;						// clear flags (rc_w0)
	TIM2->CR1  = 0;						// upcounter, etc...
	TIM2->PSC  = (90-1);			// APB1 freq is 90MHz, tim freq = 1MHz
	TIM2->ARR  = 0xFFFFFFFF;
	TIM2->EGR |= TIM_EGR_UG;	// apply PSC and ARR values (shadow regs stuff)
	
	TIM2->CR1 |= TIM_CR1_CEN;	// run, honey, run!!!
}


void Delay_us(uint32_t dly_us)
{
	//LED_ON();		// dbg
	
	stamp_us = MICROS();
	
	while ( (MICROS() - stamp_us) < dly_us )
	{ ;	}
	
	//LED_OFF();	// dbg
}

