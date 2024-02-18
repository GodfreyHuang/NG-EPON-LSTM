#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <omnetpp.h>
#include "../traffic_gen/Messages/EPON_messages_m.h"
#include "../common/LogResults.h"

using namespace std;

class Classifier;
class Scheduler;

class PowerControler : public cSimpleModule {
public:
	PowerControler();
	~PowerControler();

	void ChangeMode(uint16_t mode, simtime_t startTime, simtime_t duration, int info = -1);
	void IntraInterToActive();
	void IntraInterToDoze();
	void GeneralWakeUp();

	uint32_t GetEnergyMode() {
	    currentMode = ACTIVE;/// Added!
		return currentMode;
	}
	void SetIsIntra() {
		isIntra = true;
	}
	void SetIsInter() {
		isInter = true;
	}
	bool GetIsIntra() {
		return isIntra;
	}
	bool GetIsInter() {
		return isInter;
	}

	bool inTriSleep;
	int modeInfo; // use this only for debug purpose

private:
	uint16_t onuIdx;
	uint32_t currentMode;
	uint32_t onuSize;
	simtime_t dozeStartTime, sleepStartTime;
	simtime_t maxMpcpTimer;
	simtime_t activeStartTime;
	simtime_t doze_accu, sleep_accu, active_accu, intra_accu, inter_accu;
	bool isInter, isIntra;
	cMessage * sleepTimeUpEvent, *startSleepEvent, *startDozeEvent, *maxMpcpEvent;
	Scheduler* scheduler;
	Classifier* classifier;

	void ToActive();
	void ToDoze();
	void ToSleep();

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void finish();
};
