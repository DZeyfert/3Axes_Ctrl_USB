#include "settings.h"

volatile uint32_t receive_cnt = 0;
volatile uint8_t rx_flg = 0;

val_t 		vals[AXES_NUM];		// number of pulses and signs extracted from command
uint16_t	dlys[AXES_NUM];		// half-period of one pulse / Time to uphold High
														//	 or Low state of a STEP pin
uint16_t	cnt[AXES_NUM];		// number of remaining actions for axis, 
														//   initialized by (vals[i].num << 1), then
														//   decremented
														//	 - EVEN	- required to RAISE STEP
														//	 - ODD		- required to FALL STEP
														//	 - ZERO	- action is not required, also axis is
														//   already STOPPED
uint32_t  stamp[AXES_NUM];	// target time to next action (rise or fall)
uint32_t	tframe = 0, next_tframe = 0;			// timeframe length in [ms]
uint8_t	  axes_num = 0, next_axes_num = 0;	// name is self explanatory


/* example string:
[Interval_ms];[Axes num];[Axis1 steps];[Axis2 steps];[Axis3 steps];\n
020;3;+000;-013;+060;\n
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//   uint8_t convert_input(void)                                              //
// Checks input data. If formatting is correct and values are in acceptable   //
// range returns 1, othewise - 0                                              //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
uint8_t convert_input(void)
{
	uint32_t tmp;
	uint8_t* ptr = UserRxBufferFS;
	
	// Check for the ';' at [3] and [5]
	if (	(ptr[3] != ';') ||
				(ptr[5] != ';')
	) {
		return 0;		
	}
	
	// Time Frame / Time Interval Value
	tmp = (ptr[0] - 0x30) * 100 +
				(ptr[1] - 0x30) * 10 +
				(ptr[2] - 0x30);
	if ( tmp > TFRAME_MAX ) {
		return 0;
	}
	next_tframe = tmp;	// convert in [us], save for futher usage
	
	// Axes Number
	tmp = ptr[4] - 0x30;
	if (tmp > AXES_NUM) {
		return 0;
	}
	next_axes_num = tmp;
	
	// Parse Step Values
	ptr +=6;	// skip "020;3;" part
	
	for (uint8_t i = 0; i < next_axes_num; i++)
	{
		// "±XXX;"
		if ( ptr[4] != ';' ) {
			return 0;
		}
		// get Steps Value
		// can also check, if ptr[1:3] are in range of [1:9]
		tmp = (ptr[1] - 0x30) * 100 +
					(ptr[2] - 0x30) * 10 +
					(ptr[3] - 0x30);
		// module and speed check
		if (	(tmp > STEP_MAX) ||
					((tmp/next_tframe) > SPEED_MAX) 
		) {
			return 0;
		}

		// save module 
		vals[i].num = tmp;
		
		// get Sign
		if ( ptr[0] == '-' ) {
			vals[i].dir = 1;
		}
		else {
			vals[i].dir = 0;
		}
		
		// increment pointer
		ptr += 5; // for "±XXX;"
	}
	
	
		// Check end of string for '\n'
//	if ( *(uint16_t*)(&ptr[(AXES_NUM*5) - 1]) != 0x0A3B ) {	// ";\n" at the end of the line
	if ( UserRxBufferFS[6 + 5*next_axes_num] != '\n' ) {
		return 0;
	}
	return 1;	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//   void frame_setup(void)                                                   //
// Splits [Interval_ms] into 2*[AxisN steps] for each axis to accomodate Step //
// Rise and Fall events, also sets Direction for the duration of Frame        //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void frame_setup(void)
{
	uint32_t tmp = MICROS();
	uint32_t tframe_us;
	
	mot_step_all_low();
	if ( !next_axes_num ) {
		return;	// can be used as an emergency stop
	}
	
	// update vars (for debugging to differentiate between current and next cycle data)
	tframe = next_tframe;
	axes_num = next_axes_num;
	
	tframe_us = tframe * 1000;	// convert to [us]
	
	for (uint8_t i = 0; i < AXES_NUM; i++) {
		if ( vals[i].num ) {
			// calculate period and number of pulses in a FRAME_PERIOD
			cnt[i] = (vals[i].num << 1);					// number of actions
			dlys[i] = (tframe_us / cnt[i]);				// length of an action
			stamp[i] = tmp;												// start time
			
			// check Direction, adjust, if differs
			if ( AXIS_GET_DIR(i) != vals[i].dir ) {
				// set new direction
				AXIS_CNG_DIR(i);	// xor
			}
		}
		else {
			// clear remaining cnt
			cnt[i] = 0;
		}
	}
	
	// WAIT (at least 5us)
	Delay_us(7);
	
	// RAISE step pin for the first time
	for (uint8_t i = 0; i < AXES_NUM; i++) {
		if ( cnt[i] ) {
			AXIS_STP_HIGH(i);
			cnt[i]--;
		}
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//   void frame_mgmt(void)                                                    //
// Changes state of Step pin if the time is right                             // 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void frame_mgmt(void)
{
	for (uint8_t i = 0; i < AXES_NUM; i++) {
		if ( (cnt[i]) && RIGHT_TIME(i) ) {
			// Action
			if ( cnt[i] & 1 ) {
				// ODD - required to FALL STEP
				AXIS_STP_LOW(i);
			}
			else {
				// EVEN - required to RAISE STEP
				AXIS_STP_HIGH(i);
			}
			// decrement
			cnt[i]--;
			// move target time
			stamp[i] += dlys[i];
		}
	}
	
	if ( !cnt[0] ) {
		LED_OFF();	// dbg
	}
}




