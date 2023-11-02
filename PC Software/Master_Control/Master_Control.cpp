// Master_Control.cpp - Control software for BTT Rumba32 V1.0 board (6 steppers max)
// BIGTREETECH Rumba32 bit V1.0 (BTT Rumba32 v1.0)
// https://zona-3d.ru/catalog/elektronika/platy_upravleniya/plata_upravleniya_bigtreetech_rumba32_bit_v1_0



#include <iostream>
#include <windows.h>
#include <stdlib.h>

#include <string>
#include <cstdio>

using namespace std;

/* SETTINGS */

#define			AXES_NUM_MIN  (1)
#define			AXES_NUM_MAX  (3)	// there are 6 Axes on BTT Rumba32 v1.0, but now only 3 are currently in operation

#define			PACK_DLY_MIN	(20)
#define			PACK_DLY_MAX	(999)

#define			STEP_ABS_MAX	(999)

#define			SPEED_MAX			(60/20)	// 60 pulses per 20ms is already no good (step losses)

#define			CMD_LEN_MAX		(1000)

/* TYPEDFS */
typedef struct
{
	string		port;	// chars
	int				dly;  // [ms]
	int				axes_num;
	int				steps[AXES_NUM_MAX];
} command_t;

/* PROTOTYPES */
uint8_t Input_Parse(int argc, char* argv[]);
uint8_t Serial_Connect(void);
void		Serial_Disconnect(void);
void		Serial_Write(char* txt, uint32_t size);


/* VARIABLES */
command_t			cmd;
HANDLE				serialHandle;
DCB						serialDCB;
COMMTIMEOUTS	serialTO = {
	0,				//	ReadIntervalTimeout
	50,				//	ReadTotalTimeoutMultiplier
	50,				//	ReadTotalTimeoutConstant
	10,				//	WriteTotalTimeoutMultiplier	// Tmax = size * writeTotalTimeoutMultiplier + writeTotalTimeoutConstant
	10				//	WriteTotalTimeoutConstant
};


/* FUNCTION'S DEFINITIONS */
int main(int argc, char* argv[])
{
		printf("\n================================ MASTER CONTROL ================================\n");

	// Check input data (arguments)
	if (Input_Parse(argc, argv)) {
		//printf("\nArguments - Ok\n");	//dbg
	}
	else {
		printf("Arguments - Invalid\n");
		printf("\nBad call, use this binary with following arguments:\n");
		printf("<COM number> <Interval_ms> <Axes num> <Axis1 steps> <Axis2 steps> <AxisN steps>\n");
		return 0;
	}

/*
	// Debug output of received arguments
	if (argc > 1) {
		for (uint8_t i = 1; i < argc; i++) {
			printf("%s\n", argv[i]);
		}
	}
*/

	// -------------------------------- Open port --------------------------------
	// Potential bottleneck, don't know how fast Win will
	// process rapid open/close requests for Serial Port.
	// It'll be better to handle port separately.
	if (Serial_Connect()) {
		printf("%s - Connected!\n", cmd.port.c_str());


		// --------------------------- Compose command -----------------------------
		string buf;
		char tmp_str[10] = "";

		sprintf_s(tmp_str, "%03d;%d;", cmd.dly, cmd.axes_num);	// time interval, number of axes
		buf.append(tmp_str);

		for (uint8_t i = 0; i < cmd.axes_num; i++) {
			sprintf_s(tmp_str, "%+04d;", cmd.steps[i]);						// steps value for each axis
			buf.append(tmp_str);
		}

		buf.append("\n");																				// command end symbol
		printf(buf.c_str());

		// --------------------------- Transmit command ----------------------------
		Serial_Write((char*)buf.c_str(), buf.size());

	}

	Serial_Disconnect();	// free handle
  return 0; 

}

uint8_t Input_Parse(int argc, char* argv[])
{
	int32_t tmp = 0;

	// ------------------------- Minimal Argument Number -------------------------
	// there should be, at least, 5 arguments (case of single Axis)
  if (argc < 5) {
    printf("\nERROR: Arguments Number - Not enough!\n");
    return 0;
  }

	// ----------------------------- COM Port Number -----------------------------
	// Check COM port number
	try {
		tmp = stoi(argv[1]);
	}
	catch (...) {
		printf("\nERROR: Port Number - Invalid Argument!\n");
		return 0;
	}

	if (tmp < 1) {
		printf("\nERROR: Port Number - Negative Value!\n");
		return 0;
	}

	cmd.port = "\\\\.\\COM";
	cmd.port.append(argv[1]);
	//printf("%s\n", cmd.port.c_str());	// dbg

	// ------------------------------- Packet Delay -------------------------------
	try {
		tmp = stoi(argv[2]);
	}
	catch (...) {
		printf("\nERROR: Packet Delay - Invalid Argument!\n");
		return 0;
	}

	if ( (tmp < PACK_DLY_MIN) || (tmp > PACK_DLY_MAX) ) {
		printf("\nERROR: Packet Delay - Out of Range!\n");
		return 0;
	}
	cmd.dly = tmp;	// [ms]

	// ------------------------------- Axes Number --------------------------------
	try {
		tmp = stoi(argv[3]);
	}
	catch (...) {
		printf("\nERROR: Axes Number - Invalid Argument!\n");
		return 0;
	}

	if ((tmp < AXES_NUM_MIN) || (tmp > AXES_NUM_MAX)) {
		printf("\nERROR: Axes Number - Out of Range!\n");
		return 0;
	}
	cmd.axes_num = tmp;

	// --------------------------- Steps Number & Value ---------------------------
	if (argc < (4 + cmd.axes_num)) {
		printf("\nERROR: Axes Number - Not Enough Arguments!\n");
		return 0;
	}
	else if (argc > (4 + cmd.axes_num)) {
		printf("\nERROR: Axes Number - Too Much Arguments!\n");
		return 0;
	}

	for (int i = 0; i < cmd.axes_num; i++) {
		try {
			tmp = stoi(argv[4 + i]);
		}
		catch (...) {
			printf("\nERROR: Axis (%d) Steps Number - Invalid Argument!\n", (i + 1));
			return 0;
		}

		if (abs(tmp) > STEP_ABS_MAX) {
			printf("\nERROR: Axis (%d) Steps Number - Out of Range!\n", (i + 1));
			return 0;
		}

		if ((abs(tmp) / cmd.dly) > SPEED_MAX) {
			printf("\nERROR: Axis (%d) Speed - Too high!\n", (i + 1));
			return 0;
		}

		cmd.steps[i] = tmp;
	}

	return 1;	// success!
}

uint8_t Serial_Connect(void)
{
	try {
		// use "A" variation to avoid WCHAR strings
		serialHandle = CreateFileA(
			cmd.port.c_str(),
			GENERIC_WRITE,
			0,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0
		);
		if (serialHandle == INVALID_HANDLE_VALUE) {
			throw -1;
		}

		string settings = "baud=115200 parity=N data=8 stop=1";
		BuildCommDCBA(settings.c_str(), &serialDCB);		// pointer to device-control block (DCB)
		SetCommState(serialHandle, &serialDCB);					// handle, DCB
		SetCommTimeouts(serialHandle, &serialTO);				// set timings/timeouts
	}
	catch (...) {
		printf("\nERROR: %s - Can't Open Port\n", cmd.port.c_str());
		return 0;
	}

	return 1;
}

void Serial_Disconnect(void)
{
	if (serialHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(serialHandle);
		printf("%s - Disconnected!\n", cmd.port.c_str());
	}
}

void Serial_Write(char* txt, uint32_t size)
{
	DWORD bytes_Tx;

	if (size > CMD_LEN_MAX) {
		return; // failsafe just in case
	}
	WriteFile(serialHandle, txt, size, &bytes_Tx, NULL);
}



