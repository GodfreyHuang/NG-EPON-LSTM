#pragma once
#include <vector>
#include <fstream>
#include <omnetpp.h>
#include "ONUTableEntry.h"
#include "../common/IniParser.h"
#include "../traffic_gen/Messages/EPON_messages_m.h"

using namespace std;

typedef struct InterSleepOnu {
	uint16_t index;
	float dataSize; // X times mtw
} InterSleepOnu;

typedef struct Onu {
	uint16_t index;
	uint32_t dataSize; // bytes
} Onu;

class ONUTable : public cSimpleModule {
public:
	ONUTable();
	~ONUTable();

	ONUTableEntry * GetEntry(uint32_t idx);

	int GetTableSize() {
		return tbl.size();
	}
	void RegisterONU(RTTReg * msg);  //register the onu that didn't connect to olt.
	void UpdateTable(MPCPReport * msg);
	simtime_t getSleepArrivalTime(uint32_t idx) {
		return sleepTimeOutEventVec[idx]->getArrivalTime();
	}
	void ResetInterOrder() {
		interOnuOrder.clear();
	}
	void SetEocTime(simtime_t t) {
		eocTime = t;
	}
	simtime_t GetEocTime() {
		return eocTime;
	}

	// Inter Sleep part
	vector<vector<InterSleepOnu>> interOnuOrder; // layer 1: channel, layer 2: heavy ONU order info
	simtime_t eocTime;
	simtime_t largestRTT;  //record the biggest rtt.

private:
	int onuSize;
	vector<TimeOutMsg *> sleepTimeOutEventVec;
	vector<ONUTableEntry> tbl;

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void finish();
};
