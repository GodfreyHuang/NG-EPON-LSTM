#include "Scheduler.h"

Define_Module(Scheduler);

Scheduler::Scheduler() {
}

Scheduler::~Scheduler() {
	cancelAndDelete(sendUpstreamMessage);
}

void Scheduler::initialize() {
	upI = gateHalf("Up$i", cGate::INPUT);
	upO = gateHalf("Up$o", cGate::OUTPUT);
	downI = gateHalf("Down$i", cGate::INPUT);
	downO = gateHalf("Down$o", cGate::OUTPUT);
	up2I = gateHalf("Up2$i", cGate::INPUT);
	up2O = gateHalf("Up2$o", cGate::OUTPUT);
	down2I = gateHalf("Down2$i", cGate::INPUT);
	down2O = gateHalf("Down2$o", cGate::OUTPUT);
	up3I = gateHalf("Up3$i", cGate::INPUT);
	up3O = gateHalf("Up3$o", cGate::OUTPUT);
	down3I = gateHalf("Down3$i", cGate::INPUT);
	down3O = gateHalf("Down3$o", cGate::OUTPUT);
	up4I = gateHalf("Up4$i", cGate::INPUT);
	up4O = gateHalf("Up4$o", cGate::OUTPUT);
	down4I = gateHalf("Down4$i", cGate::INPUT);
	down4O = gateHalf("Down4$o", cGate::OUTPUT);

	sendUpstreamMessage = new cMessage("sendUpstreamMessage");
	isHoldingGate = false;
	holdGateMsg = NULL;

	string powerConPath = getParentModule()->getFullPath() + ".powerControler";
	pwCtrler = check_and_cast<PowerControler *>(getSimulation()->getModuleByPath(powerConPath.c_str()));

	IniParser& ini = IniParser::GetInstance();
	if(this->getParentModule()->getIndex() == 0) // the first module initialize do Parse(), onu[0].scheduler
	    ini.Parse();
	onuSize = ini.sizeOfONU;
	distance = ini.distance;
	upRateRec = ini.upDataRateRec;

	packetDelay[0] = 0;
	packetDelay[1] = 0;
	packetCount[0] = 0;
	packetCount[1] = 0;
	maxDelay[0] = 0;
	maxDelay[1] = 0;
	upBytes[0] = 0;
	upBytes[1] = 0;
	holdGateCount = 0;
}

void Scheduler::handleMessage(cMessage *msg) {
	if (msg->isSelfMessage()) {
		if (msg == sendUpstreamMessage && !transmitQueue.isEmpty())
			SendUpstream();
	}
	else {
		cGate *fromGate = msg->getArrivalGate();
		if (fromGate == upI || fromGate == up2I || fromGate == up3I || fromGate == up4I) { // send from ONU classifier (upstream)
			transmitQueue.insert(msg);
			if (msg->getKind() == MPCP_TYPE) {
				if (!sendUpstreamMessage->isScheduled())
					scheduleAt(simTime(), sendUpstreamMessage);
			}
		}
		else if (fromGate == gate("in") || fromGate == gate("in2") || fromGate == gate("in3") || fromGate == gate("in4")) { // sendDirect from OLT scheduler (downstream)
			if (msg->getKind() == MPCP_TYPE) {
				MPCPGate * gt = check_and_cast<MPCPGate *>(msg);
				if (pwCtrler->GetEnergyMode() != SLEEP) {
				    ProcessMPCP(gt);
				}
				else {
//				    cout << pwCtrler->modeInfo;
//				    endSimulation();
                    isHoldingGate = true;
                    if(holdGateMsg!=NULL){
                        delete holdGateMsg;
                    }
                    gt->setHolded(true);
                    holdGateMsg = gt;
                    holdGateCount++;
				}
			}
			else {
				MyPacket *pkt = check_and_cast<MyPacket *>(msg);
				send(msg, downO);
			}
		}
		else if (fromGate == downI || fromGate == down2I || fromGate == down3I || fromGate == down4I) { // send from splitter (downstream)
			if (strcmp(msg->getClassName(), "MPCPAutoDiscovery") == 0) {
				ProcessMPCP(check_and_cast<MPCP *>(msg));
			}
			delete msg;
		}
	}
}

void Scheduler::finish() {
    IniParser& ini = IniParser::GetInstance();

// ---------- Scheduler ----------
	LogResults o("Scheduler");

	if (getParentModule()->getId() - 2 == 0) {
	    IniParser& ini = IniParser::GetInstance();
		double upLoad = ini.upOfferedLoad, downLoad = ini.downOfferedLoad;
		o << upLoad * 100 << "%\t" << downLoad * 100 << "%" << "\t(Mbits)\tendTime=" << simTime() << endl;
	}

	simtime_t pktDelayHi = packetDelay[0] == 0 ? 0 : (packetDelay[0] / packetCount[0]);
	simtime_t pktDelayLo = packetDelay[1] == 0 ? 0 : (packetDelay[1] / packetCount[1]);

	o << getParentModule()->getId() - 2 << "\t" << (double) upBytes[0] * 8 / 1000000 << "\t" << (double) upBytes[1] * 8 / 1000000 << "\t" << pktDelayHi << "\t" << pktDelayLo << "\t" << maxDelay[0] << "\t" << maxDelay[1] << endl;

	if (getParentModule()->getId() - 2 == onuSize - 1)
		o << "\n\n\n";

// --- PKT_DEALY------
	LogResults pktFile("Pkt_Delay_And_Pkt_Count");
	if (getParentModule()->getId() - 2 == 0) {
	    IniParser& ini = IniParser::GetInstance();
	    double upLoad = ini.upOfferedLoad, downLoad = ini.downOfferedLoad;
	    pktFile << upLoad * 100 << "%\t" << downLoad * 100 << "%" << "\t(Mbits)\tendTime=" << simTime() << endl;
	}

	pktFile << getParentModule()->getId() - 2 << "\t" << packetDelay[0] <<  "\t" <<  packetCount[0] << "\t" << pktDelayHi << endl;

	if (getParentModule()->getId() - 2 == onuSize - 1)
	    pktFile << "\n\n\n";


// ---------- GateHolded ----------
	LogResults o2("GateHolded");
	o2 << getParentModule()->getId() - 2 << "\t" << holdGateCount << endl;
	if (getParentModule()->getId() - 2 == onuSize - 1)
		o2 << "\n\n\n";
}

void Scheduler::ProcessMPCP(MPCP *msg) {
	switch (msg->getOpcode()) {
		case MPCP_DISCOVER: {
			RTTReg *reg = new RTTReg();
			reg->setKind(MPCP_TYPE);
			reg->setOpcode(MPCP_REGISTER);
			reg->setTimestamp(simTime());
			reg->setSrcAddr(getParentModule()->getId());
			reg->setDestAddr(getSimulation()->getModuleByPath("EPON.olt")->getId());
			reg->setByteLength(64);
			reg->setRtt(2 * (distance / 200000));
			cModule *onutbl = getSimulation()->getModuleByPath("EPON.olt.onuTable");
			sendDirect(reg, onutbl, "in");
			break;
		}
		case MPCP_GATE: {
			MPCPGate * gate = check_and_cast<MPCPGate *>(msg);
			send(gate, downO);
			break;
		}
		default:
			cout << "Unrecognized MPCP OpCode!!" << endl;
			endSimulation();
	};
}

void Scheduler::SendUpstream() {
	cModule *olt_Scheduler = getSimulation()->getModuleByPath("EPON.olt.olt_Scheduler");
	MyPacket *pkt = check_and_cast<MyPacket *>(transmitQueue.pop());
	uint16_t pri = pkt->getPriority();
	simtime_t propagationDelay = distance / 200000; // 20km and light speed 200,000 km/s, so it's 100us
	simtime_t packetDuration = (pkt->getBitLength() * upRateRec);
	simtime_t nxtSendTime = simTime() + packetDuration;
	sendDirect(pkt, propagationDelay, packetDuration, olt_Scheduler, "in");
	if (pkt->getKind() != MPCP_TYPE) {
		//if (simTime() > 1) { //Origin, Later change back
		if (simTime() > 7) {
			packetDelay[pri] += simTime() - pkt->getTimestamp();
			if ((simTime() - pkt->getTimestamp()) > maxDelay[pri]) {
				maxDelay[pri] = simTime() - pkt->getTimestamp();
			}
			packetCount[pri]++;
			upBytes[pri] += pkt->getByteLength();
		}
	}
	if (!transmitQueue.isEmpty())
		scheduleAt(nxtSendTime, sendUpstreamMessage);
}

void Scheduler::RecvHoldedGate() { // for issue of Intra/Inter wake-up time mismatch
	Enter_Method("RecvHoldedGate()");
	if (isHoldingGate && holdGateMsg != NULL) {
		ProcessMPCP(holdGateMsg);
		holdGateMsg = NULL;
		isHoldingGate = false;
	}
}
