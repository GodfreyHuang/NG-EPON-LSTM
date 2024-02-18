#pragma once
#include <omnetpp.h>
#include <fstream>
#include <cmath>
#include <vector>
#include "../traffic_gen/Messages/EPON_messages_m.h"
#include "../common/LogResults.h"
#include "../common/IniParser.h"
#include "../ONU/PowerControler.h"

using namespace std;

class Classifier;
class PowerControler;

class Scheduler : public cSimpleModule {
public:
	Scheduler();
	~Scheduler();

	void RecvHoldedGate();
private:
	int onuSize;
	double distance;
	double upRateRec;
	bool isHoldingGate;
	MPCPGate* holdGateMsg;
	uint32_t holdGateCount;

	map<uint16_t, uint32_t> upBytes;
	map<uint16_t, uint32_t> packetCount;
	map<uint16_t, simtime_t> packetDelay;
	map<uint16_t, simtime_t> maxDelay;

	cMessage *sendUpstreamMessage;
	cGate *upI, *upO, *downI, *downO, *up2I, *up2O, *down2I, *down2O, *up3I, *up3O, *down3I, *down3O, *up4I, *up4O, *down4I, *down4O;

	cQueue transmitQueue;
	PowerControler* pwCtrler;

	void SendUpstream();
	void ProcessMPCP(MPCP *msg);

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void finish();
};
