#ifndef	__SETTINGS_H__
	#define __SETTINGS_H__
	
	#include "stm32f4xx.h"
	#include "usbd_cdc_if.h"

	#define AXES_NUM			(3)
	#define STEP_MAX			(999)		// 
	#define SPEED_MAX			(60/20)	// 60 pulses per 20ms is max
	#define TFRAME_MAX		(999)		// [ms]
	//#define TFRAME_MAX		(20000)	// [us]
	
	typedef struct {
		uint8_t		dir;
		uint16_t 	num;
	} val_t;
	
	extern val_t vals[AXES_NUM];
	
	typedef struct {
		GPIO_TypeDef*	port;
		uint16_t			bit;
	} pin_t;
	
	typedef struct {
		pin_t					nEn;
		pin_t					stp;
		pin_t					dir;
	} mot_t;
	
	extern mot_t axes[AXES_NUM];
	
	extern uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];
	extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
	extern USBD_HandleTypeDef hUsbDeviceFS;
	
	extern volatile uint8_t rx_flg;
	extern volatile uint32_t receive_cnt;
	
	
	// Y axis
	#define Y_EN_PORT					(GPIOE)		// PE3
	#define Y_EN							(1 << 3)
	#define Y_STP_PORT				(GPIOE)		// PE5 - TIM9_CH1
	#define Y_STP							(1 << 5)
	#define Y_DIR_PORT				(GPIOE)		// PE6
	#define Y_DIR							(1 << 6)

	// E0 axis
	#define E0_EN_PORT				(GPIOC)		// PC12
	#define E0_EN							(1 << 12)
	#define E0_STP_PORT				(GPIOB)		// PB5 - TIM3_CH2
	#define E0_STP						(1 << 5)
	#define E0_DIR_PORT				(GPIOB)		// PB6
	#define E0_DIR						(1 << 6)

	// E2 axis
	#define E2_EN_PORT				(GPIOD)		// PD0
	#define E2_EN							(1 << 0)
	#define E2_STP_PORT				(GPIOD)		// PD2 - No Timer
	#define E2_STP						(1 << 2)
	#define E2_DIR_PORT				(GPIOD)		// PD3
	#define E2_DIR						(1 << 3)

	// For additioanl Axes change AXES_NUM to appropriate vlue, add defines here
	//and modify array "axes" in "motor.c"


	// LED
	#define LED_PORT					(GPIOB)
	#define LED_PIN						(1 << 14)
	
	#define LED_ON()					LED_PORT->BSRR = LED_PIN
	#define LED_OFF()					LED_PORT->BSRR = (LED_PIN << 16)
	#define LED_OSC()					LED_PORT->ODR ^= LED_PIN
	
	// Microseconds Timer macroses (uses 32-bit TIM2/5 at 1MHz)
	#define MICROS()					(TIM2->CNT)
	#define RIGHT_TIME(x)			((MICROS() - stamp[(x)]) >= dlys[(x)])
	
	// Axes macroses
	#define AXIS_STP_LOW(x)		axes[(x)].stp.port->BSRR = (axes[(x)].stp.bit << 16)
	#define AXIS_STP_HIGH(x)	axes[(x)].stp.port->BSRR = (axes[(x)].stp.bit)
	#define AXIS_GET_DIR(x)		(1 && (axes[(x)].dir.port->ODR & axes[(x)].dir.bit))
	#define AXIS_CNG_DIR(x)		axes[(x)].dir.port->ODR ^= axes[(x)].dir.bit
	
	
	
	
	void Pins_Init(void);
	void Timer_Init(void);
	
	void Delay_us(uint32_t dly_us);
	
	void mot_step_all_low(void);
	
	uint8_t convert_input(void);
	void frame_setup(void);
	void frame_mgmt(void);


	
#endif // __SETTINGS_H__
