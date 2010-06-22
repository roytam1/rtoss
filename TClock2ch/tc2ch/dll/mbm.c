
// interface to work with shared memory for MBM5
// original code by
//                  Chris Zahrt techn0@iastate.edu (visual c alterations)
//                  Anders@Majland.org (author of original c code)
//                  Bill Nalen bill@nalens.com (Update for MBM 5.1.9)

#include "tcdll.h"

#define NrTemperature 32
#define NrVoltage 16
#define NrFan 16
#define NrCPU 4

//    enum Bus
#define BusType		char
#define ISA			0
#define SMBus		1
#define VIA686Bus	2
#define DirectIO	3

//    enum SMB
#define SMBType			char
#define smtSMBIntel		0
#define smtSMBAMD		1
#define smtSMBALi		2
#define smtSMBNForce	3
#define smtSMBSIS		4

// enum Sensor Types
#define SensorType		char
#define stUnknown		0
#define stTemperature	1
#define stVoltage		2
#define stFan			3
#define stMhz			4
#define stPercentage	5

typedef struct {
	SensorType iType;			// type of sensor
	int Count;					// number of sensor for that type
} SharedIndex;

typedef struct {
	SensorType ssType;			// type of sensor
	unsigned char ssName[12];	// name of sensor
	char sspadding1[3];			// padding of 3 byte
	double ssCurrent;			// current value
	double ssLow;				// lowest readout
	double ssHigh;				// highest readout
	long ssCount;				// total number of readout
	char sspadding2[4];			// padding of 4 byte
	long double	ssTotal;		// total amout of all readouts
	char sspadding3[6];			// padding of 6 byte
	double ssAlarm1;			// temp & fan: high alarm; voltage: % off;
	double ssAlarm2;			// temp: low alarm
} SharedSensor;

typedef struct {
	short siSMB_Base;			// SMBus base address
	BusType siSMB_Type;			// SMBus/Isa bus used to access chip
	SMBType siSMB_Code;			// SMBus sub type, Intel, AMD or ALi
	char siSMB_Addr;			// Address of sensor chip on SMBus
	unsigned char siSMB_Name[41]; // Nice name for SMBus
	short siISA_Base;			// ISA base address of sensor chip on ISA
	int siChipType;				// Chip nr, connects with Chipinfo.ini
	char siVoltageSubType;		// Subvoltage option selected
} SharedInfo;

typedef struct {
	double sdVersion;			// version number (example: 51090)
	SharedIndex sdIndex[10];	// Sensor index
	SharedSensor sdSensor[100];	// sensor info
	SharedInfo sdInfo;			// misc. info
	unsigned char sdStart[41];	// start time
	unsigned char sdCurrent[41];// current time
	unsigned char sdPath[256];	// MBM path
} SharedData;

static int tempCount, voltCount, fanCount;
double temperatures[NrTemperature];
double voltages[NrVoltage];
double fans[NrFan];

int MBMSharedDataUpdate(void)
{
	HANDLE hSData;
	SharedData *ptr;
	int i;
	int totalCount;

	for(i = 0; i < NrTemperature; i++) temperatures[i] = 0;
	for(i = 0; i < NrVoltage;     i++) voltages[i] = 0;
	for(i = 0; i < NrFan;         i++) fans[i] = 0;

	hSData = OpenFileMapping(FILE_MAP_READ, FALSE, "$M$B$M$5$S$D$");
	if (hSData == 0)
		return 1;

	ptr = (SharedData *)MapViewOfFile(hSData, FILE_MAP_READ, 0, 0, 0);
	if (ptr == 0)
	{
		CloseHandle(hSData);
		return 1;
	}

	totalCount = 0;
	for (i = 0; i < 5; i++)
	{
		totalCount += ptr->sdIndex[i].Count;
	}

	tempCount = 0;
	voltCount = 0;
	fanCount = 0;
	for (i = 0; i < totalCount; i++)
	{
		switch (ptr->sdSensor[i].ssType)
		{
		case stUnknown:
			break;

		case stTemperature:
			temperatures[tempCount] = ptr->sdSensor[i].ssCurrent;
			if (tempCount < (NrTemperature-1))
				tempCount++;
			break;

		case stVoltage:
			voltages[voltCount] = ptr->sdSensor[i].ssCurrent;
			if (voltCount < (NrVoltage-1))
				voltCount++;
			break;

		case stFan:
			fans[fanCount] = ptr->sdSensor[i].ssCurrent;
			if (fanCount < (NrFan-1))
				fanCount++;
			break;

		case stMhz:
			break;

		case stPercentage:
			break;
		}
	}

	UnmapViewOfFile(ptr);
	CloseHandle(hSData);

	return 0;
}
