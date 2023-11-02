#include "settings.h"

mot_t axes[AXES_NUM] = {
//	{ .nEn.port =, .nEn.pin =, .stp.port = , .stp.bit = , .dir.port = , .dir.bit = },
	{ .nEn.port = Y_EN_PORT,  .nEn.bit = Y_EN,  .stp.port = Y_STP_PORT,  .stp.bit = Y_STP,  .dir.port = Y_DIR_PORT,  .dir.bit = Y_DIR  },
	{ .nEn.port = E0_EN_PORT, .nEn.bit = E0_EN, .stp.port = E0_STP_PORT, .stp.bit = E0_STP, .dir.port = E0_DIR_PORT, .dir.bit = E0_DIR },
	{ .nEn.port = E2_EN_PORT, .nEn.bit = E2_EN, .stp.port = E2_STP_PORT, .stp.bit = E2_STP, .dir.port = E2_DIR_PORT, .dir.bit = E2_DIR }
};

void mot_step_all_low(void)
{
	// end cycles
	for (uint8_t i = 0; i < AXES_NUM; i++) {
		// set STP pin LOW
		AXIS_STP_LOW(i);
		// modify flags
		
	}
}


