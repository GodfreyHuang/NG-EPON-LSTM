#pragma once
#include <fstream>
#include <queue>
#include <omnetpp.h>
#include "../traffic_gen/Messages/EPON_messages_m.h"
#include "../common/LogResults.h"
#include "../common/IniParser.h"
#include "PowerControler.h"
#include "Scheduler.h"

using namespace std;

class PowerControler;

class Classifier : public cSimpleModule {
public:
	Classifier();
	~Classifier();
	void CheckHighPriBuffer();
	int prevTriMode;

private:
	vector<cGate*> upO;
	cGate *upI, *downI, *downO, *up2I, *down2I, *down2O, *up3I, *down3I, *down3O, *up4I, *down4I, *down4O;
	cQueue pktBuffer[2], readyQueue;
	cMessage * sendUpstreamEvent;
	cMessage * checkHPQueue;
	cMessage * iiDozeWakeEvent;
	cMessage * iiWakeEvent;

	PowerControler * pwCtrler;

	uint32_t bufferLimit, highPriBufferSize, lowPriBufferSize;
	uint32_t onuSize;
	uint32_t totalUploadedBytes;
	uint32_t cycle;
	///Added
	uint32_t total_upQueueTotalLen;
	uint32_t grantUpOld;
	uint32_t singleTotalSize;
	///
	double upRateRec;
	double totalBufferSize;
	bool needTuning;
	int reportCycles;
	int onuStayChannel;
	int maxQueueSize;
	simtime_t lastReportTime;
	simtime_t tuningTimeLen;
	uint32_t tuningCount;
	int64_t bufferOld;
	int64_t actualAi;

    uint64_t totalLostPKT;

// method
	void PushIntoBuffer(MyPacket *pkt);
	uint32_t PopBuffer(uint32_t grantSize);
	MyPacket * SendReport(uint16_t mode, int64_t bufferSize);
	MyPacket * PktCount(MyPacket *pkt, int length);
	void IntraInterToActive();
	void IntraInterToDoze();
	void ProcessGate(MPCPGate *gt);
	void GotHighPriWakeUp();

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void finish();
};
