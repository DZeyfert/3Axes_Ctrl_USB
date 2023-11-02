#include "settings.h"

/*
	sets pins for axes Y, E0, E2 and User LED
	one executed, axes are in Hold (Active) mode
*/
void Pins_Init()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN;	// GPIOB,C,D,E
	
	// LED
	GPIOB->ODR &= ~(GPIO_ODR_OD14);								// set output to Low
	MODIFY_REG( GPIOB->MODER,
							(GPIO_MODER_MODE14),
							(GPIO_MODER_MODE14_0)
	);																						// General Purpose output
	GPIOB->OTYPER	&= ~(GPIO_OTYPER_OT14);					// clear Open Drain mode
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR14);	// Speed -> max
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD14);					// no pull
	
	// Axis Y
	GPIOE->ODR &= ~(GPIO_ODR_OD3 | GPIO_ODR_OD5 | GPIO_ODR_OD6);									// set output to Low
	MODIFY_REG( GPIOE->MODER,
							(GPIO_MODER_MODE3 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6),
							(GPIO_MODER_MODE3_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0)
	);																																						// General Purpose output
	MODIFY_REG( GPIOE->OTYPER,
							(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6),
							GPIO_OTYPER_OT3
	);			// clear Open Drain mode except for PE3
	GPIOE->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR3 | GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR6);	// Speed -> max
	GPIOE->PUPDR &= ~(GPIO_PUPDR_PUPD3 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6);		// no pull


	// Axis E0
	GPIOC->ODR &= ~(GPIO_ODR_OD12);								// set output to Low
	MODIFY_REG( GPIOC->MODER,
							(GPIO_MODER_MODE12),
							(GPIO_MODER_MODE12_0)
	);																						// General Purpose output
	GPIOC->OTYPER	|= GPIO_OTYPER_OT12;						// set Open Drain mode for PC12
	GPIOC->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR12);	// Speed -> max
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD12);					// no pull
	
	GPIOB->ODR &= ~(GPIO_ODR_OD5 | GPIO_ODR_OD6);									// set output to Low
	MODIFY_REG( GPIOB->MODER,
							(GPIO_MODER_MODE5 | GPIO_MODER_MODE6),
							(GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0)
	);																														// General Purpose output
	GPIOB->OTYPER	&= ~(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6);				// clear Open Drain mode
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR6);	// Speed -> max
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6);				// no pull


	// Axis E2
	GPIOD->ODR &= ~(GPIO_ODR_OD0 | GPIO_ODR_OD2 | GPIO_ODR_OD3);									// set output to Low
	MODIFY_REG( GPIOD->MODER,
							(GPIO_MODER_MODE0 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3),
							(GPIO_MODER_MODE0_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0)
	);																																						// General Purpose output
	MODIFY_REG( GPIOD->OTYPER,
							(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3),
							GPIO_OTYPER_OT0
	);			// clear Open Drain mode except for PD0
	GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3);	// Speed -> max
	GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);		// no pull
	
//	// Disable motors
//	GPIOE->ODR |= (1 << 3);
//	GPIOC->ODR |= (1 << 12);
//	GPIOD->ODR |= (1 << 0);
}
