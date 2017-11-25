#ifndef ADNS_5050
#define ADNS_5050

#include <SPI.h>
#include "adns50x0.h"

#define ADNS_5050_Product_ID  0x12
#define ADNS_5050_Product_ID2 0x26
#define ADNS_5050_Revision_ID 0x01

#define ADNS_5050_tSWR 20
#define ADNS_5050_tSRR  1  // 0.5 actually, but keep it integer
#define ADNS_5050_tSRW  1  // 0.5 actually, but keep it integer
#define ADNS_5050_tSWW 30

enum ADNS_5050_reg: unsigned char {
	Product_ID,
	Revision_ID,
	Motion,
	Delta_X,
	Delta_Y,
	SQUAL,
	Shutter_Upper,
	Shutter_Lower,
	Maximumm_Pixel,
	Pixel_Sum,
	Minimum_Pixel,
	Pixel_Grab,
	Mouse_Control,
	Mouse_Control2 = 0x19,
	LED_DC_Mode = 0x22,
	Chip_Reset = 0x3a,
	Product_ID2 = 0x3e,
	Inv_Rev_ID,
	Motion_Burst = 0x63
};

enum ADNS_5050_Mouse_Control_PD: unsigned char { PD_Normal=0, PD_Power_Down=1 };

enum ADNS_5050_Mouse_Control_RES: unsigned char { RES_500cpi=0, RES_1000cpi=1 };
enum ADNS_5050_Mouse_Control_RES2: unsigned char {
	RES2_125cpi = 001,
	RES2_250cpi = 002,
	RES2_375cpi = 003,
	RES2_500cpi = 004,
	RES2_625cpi = 005,
	RES2_750cpi = 006,
	RES2_850cpi = 007,
	RES2_1000cpi= 010,
	RES2_1125cpi= 011,
	RES2_1250cpi= 012,
	RES2_1375cpi= 013
};

#define ADNS_5050_Mouse_Control2_RES_EN 1
typedef union ADNS_5050_Mouse_Control_u {
	struct {
		unsigned char RES:1;
		unsigned char PD:1;
		unsigned char reserved0:6;
		unsigned char RES2:4;
		unsigned char RES_EN:1;
		unsigned char reserved1:3;
	};
	struct {
		unsigned char lower;
		unsigned char upper;
	};
} ADNS_5050_Mouse_Control;

class Adns5050
{
	private:

		int _ncsPin;
		int _resetPin;

		unsigned long _fSCK;
		SPISettings _spiSettings;

		unsigned int  _dSWR, _dSWW;
		char _lastOp = -1;

	public:

		Adns5050 (int ncsPin, int resetPin);

		bool begin (const unsigned long fsck=0);

		unsigned char read (const ADNS_5050_reg address);
		unsigned char write(const ADNS_5050_reg address, const unsigned char value);
};

#endif ADNS_50x0
