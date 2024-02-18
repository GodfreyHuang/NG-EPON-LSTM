#pragma once
#include <omnetpp.h>
#include <iostream>
#include <memory>
#include <inttypes.h>

using namespace omnetpp;

class IniParser{
public:
	static IniParser& GetInstance(); // Meyers singleton

// from config file
	// EPON.ned
	int downDataRate;
	int upDataRate;
	int sizeOfONU;
	int sizeOfCh;
	int pktBufferLimit;
	double distance;
	simtime_t tuningTime;
	bool enableTDMAmode;
	float interSleepThres;
	float chOpenThres;
	simtime_t maxMpcpTimer;
	int cycleTimeLen;
	double upOfferedLoad;
	double downOfferedLoad;
	double upHighRatio;
	double downHighRatio;
	double upSendRate;
	double downSendRate;
	double paretoRate;
    double paretoAlphaOn;
    double paretoAlphaOff;
    double paretoBetaOn;

// in code calculated
    int totalModuleAmount;
	int onuAmountPerCh;
	double downDataRateRec;
	double upDataRateRec;
	uint32_t mtw;

// methods
	void ShowParsed();
	void Parse();
	~IniParser(){}

private:
	IniParser() {}
};

/********************

	Definitions

********************/
#define MTW IniParser::GetInstance().mtw

// Ethernet frame size
const uint32_t maxEthFrame=1518;
const uint32_t minEthFrame=64;

// packet type
#define MPCP_TYPE 10
#define TIMEOUT_TYPE 168

// MPCP type
#define MPCP_GATE       2
#define MPCP_REPORT     3
#define MPCP_REGREQ     4       //not going to use
#define MPCP_REGISTER   5
#define MPCP_DISCOVER   7
#define MPCP_REGACK     6       //not going to use

// Power saving modes
#define ACTIVE     20
#define DOZE       21
#define SLEEP      22

// Power saving duration
#define SLEEPTIME  20
