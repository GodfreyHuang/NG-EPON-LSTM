#include <omnetpp.h>
#include <string.h>
#include <vector>
#include <fstream>
#include "../traffic_gen/Messages/EPON_messages_m.h"
#include "../common/LogResults.h"
#include "../common/IniParser.h"
#include "../common/TemplateCollect.h"
#include "../ONU/PowerControler.h"
#include "ONUTable.h"
#include "DBA.h"

using namespace std;

class OLT_Scheduler : public cSimpleModule {
public:
	OLT_Scheduler();
	~OLT_Scheduler();
private:
	cGate *upI, *downI, *up2I, *down2I, *up3I, *down3I, *up4I, *down4I, *downO, *upO;
	ONUTable * onutbl;

	map<uint16_t, vector<uint32_t> > downBytes;
	map<uint16_t, vector<uint32_t> > packetCount;   // map part: 0~128, vector part: [0] high [1] low
	map<uint16_t, vector<simtime_t> > packetDelay;
	map<uint16_t, vector<simtime_t> > maxDelay;
	uint16_t onuSize;
	double distance;
	double downRate;
	simtime_t tuningTimeLen;
	bool needTuning;
	uint32_t tuningCount;
	vector<uint16_t> curChOfOnu;

	void SendMpcpDiscovery();
	void ProcessMPCP(MPCP *msg);

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void finish();
};

void OLT_Scheduler::initialize() {
	upO = gateHalf("Up$o", cGate::OUTPUT);
	downO = gateHalf("Down$o", cGate::OUTPUT);
	upI = gateHalf("Up$i", cGate::INPUT);
	up2I = gateHalf("Up2$i", cGate::INPUT);
	up3I = gateHalf("Up3$i", cGate::INPUT);
	up4I = gateHalf("Up4$i", cGate::INPUT);
	downI = gateHalf("Down$i", cGate::INPUT);
	down2I = gateHalf("Down2$i", cGate::INPUT);
	down3I = gateHalf("Down3$i", cGate::INPUT);
	down4I = gateHalf("Down4$i", cGate::INPUT);

	onutbl = dynamic_cast<ONUTable *>(getSimulation()->getModuleByPath("EPON.olt.onuTable"));

	IniParser& ini = IniParser::GetInstance();
	onuSize = ini.sizeOfONU;
	distance = ini.distance;
	downRate = ini.downDataRateRec;
	tuningTimeLen = ini.tuningTime;
	needTuning = false;
	tuningCount = 0;

	for (uint16_t i = 0; i != onuSize; i++) {
		// map[onuIdx][priority]
		InitVecConstant(downBytes[i], 2, 0U); // [0] => high priority, [1] => low priority
		InitVecConstant(packetCount[i], 2, 0U);
		InitVec(packetDelay[i], 2);
		InitVec(maxDelay[i], 2);
		curChOfOnu.push_back(0);
	}

	SendMpcpDiscovery();
}

Define_Module(OLT_Scheduler);

OLT_Scheduler::OLT_Scheduler() {
}

OLT_Scheduler::~OLT_Scheduler() {
}

void OLT_Scheduler::handleMessage(cMessage *msg) {
	cGate *fromGate = msg->getArrivalGate();
	//msg->getArrivalGate() == gate("gen")
	if (fromGate == upI || fromGate == up2I || fromGate == up3I || fromGate == up4I || fromGate == gate("in") || fromGate == gate("in2")) { // sendDirect from ONU scheduler (upstream)
		if ((msg->getKind() == MPCP_TYPE)) {
			MPCP* mpcp = check_and_cast<MPCP *>(msg);
			ProcessMPCP(mpcp);
		}
		else {
			delete msg;
		}
	}
	else if (fromGate == downI || fromGate == down2I || fromGate == down3I || fromGate == down4I) { // send from DBA (downstream)
		MyPacket * pkt = check_and_cast<MyPacket *>(msg);
		uint16_t dest = pkt->getDestAddr() - 2;
		uint16_t pri = pkt->getPriority();

		stringstream path;
		path << "EPON.onu[" << dest << "].scheduler";
		cModule *onu = getSimulation()->getModuleByPath(path.str().c_str());

		simtime_t propagationDelay = distance / 200000;
		simtime_t duration = (pkt->getBitLength() * downRate);
		if (msg->getKind() == MPCP_TYPE) {
			MPCPGate* gt = check_and_cast<MPCPGate*>(msg);
			uint16_t channel = gt->getTransmitChannel();
			if (channel != curChOfOnu[dest]) {
				needTuning = true;
				curChOfOnu[dest] = channel;
				tuningCount++;
			}
			else {
				needTuning = false;
			}
		}
		//else if (simTime() > 1) { //Origin, Later change back.
		else if (simTime() > 3) { // the time we start collect the data to train.
			downBytes[dest][pri] += pkt->getByteLength();
			packetCount[dest][pri]++;
			packetDelay[dest][pri] += simTime() - pkt->getTimestamp();

			//this doc is not "the gold", but it can show how many cycle after 3s.
		    LogResults oOLTS("ONU16_OLTScheduler");
		    if(dest == 16)
		    {
		        oOLTS << " ONU16_HiPriPacketCount : " << packetCount[dest][pri] << endl;
		    }

			if (needTuning){
				packetDelay[dest][pri] += tuningTimeLen;
			}
			if (simTime() - pkt->getTimestamp() > maxDelay[dest][pri])
				maxDelay[dest][pri] = simTime() - pkt->getTimestamp();
		}

		if (fromGate == downI)
			sendDirect(pkt, propagationDelay, duration, onu, "in");
		else if (fromGate == down2I)
			sendDirect(pkt, propagationDelay, duration, onu, "in2");
		else if (fromGate == down3I)
			sendDirect(pkt, propagationDelay, duration, onu, "in3");
		else if (fromGate == down4I)
			sendDirect(pkt, propagationDelay, duration, onu, "in4");
	}
}

void OLT_Scheduler::finish() {
    IniParser& ini = IniParser::GetInstance();

// ---------- OLT_Scheduler ----------
	LogResults o("OLT_Scheduler");
	double upstreamLoading = ini.upOfferedLoad;
	double downstreamLoading = ini.downOfferedLoad;
	o << upstreamLoading * 100 << "%\t" << downstreamLoading * 100 << "%\tMbits" << endl;
	for (uint16_t i = 0; i != onuSize; i++) {

		simtime_t high_pktDelay, low_pktDelay;

		if (packetCount[i][0] == 0)
			high_pktDelay = 0;
		else
			high_pktDelay = packetDelay[i][0] / packetCount[i][0];

		if (packetCount[i][1] == 0)
			low_pktDelay = 0;
		else
			low_pktDelay = packetDelay[i][1] / packetCount[i][1];

		o << i << "\t" << (double) downBytes[i][0] * 8 / 1000000 << "\t" << (double) downBytes[i][1] * 8 / 1000000 << "\t" << high_pktDelay << "\t" << low_pktDelay << "\t" << maxDelay[i][0] << "\t" << maxDelay[i][1] << "\n";
	}
	o << "\n\n\n";

// ---------- TuningCount ----------
	LogResults o2("TuningCount");
	o2 << upstreamLoading * 100 << "% " << downstreamLoading * 100 << "% Mbits" << endl;
	o2 << "tuning count : " << tuningCount << endl;

}

void OLT_Scheduler::SendMpcpDiscovery() {
	MPCPAutoDiscovery *discover = new MPCPAutoDiscovery();
	discover->setKind(MPCP_TYPE);
	discover->setOpcode(MPCP_DISCOVER);
	discover->setTimestamp();
	discover->setByteLength(64);
	send(discover, downO);
}

void OLT_Scheduler::ProcessMPCP(MPCP *msg) { // update energy mode of ONU
	switch (msg->getOpcode()) {
		case MPCP_REGISTER: { // this case have done by module ONUtable, REGISTER won't process here.
			endSimulation();
		}
		case MPCP_REPORT: {
			MPCPReport * rep = check_and_cast<MPCPReport *>(msg);
			onutbl->UpdateTable(rep);
			send(rep, upO);
			break;
		}
		default:
			cout << "Unrecognized MPCP OpCode!!" << endl;
			endSimulation();
	};
}
