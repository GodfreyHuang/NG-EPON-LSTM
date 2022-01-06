/*
 * This file contains only initialize(), handleMessage() and finish() function implementations
 */
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "DBA.h"

#include <ctime>

#include "../ONU/Classifier.h"

Define_Module(DBA);

DBA::DBA() {}

DBA::~DBA() {
	cancelAndDelete(startNewCycleEvent);
	for (cMessage* event : sendCHDownstreamEvent)
		cancelAndDelete(event);

	// clear remain packets in buffer queue
    for (int i = 0; i < onuSize; i++) {
        for (int j = 0; j < 2; j++) {
            buffer[i][j].clear();
        }
    }
    for (int i = 0; i < chSize; i++) {
        readyQueue[i].clear();
    }

    for (int i = 0; i < onuSize; i++) {
        tempQueue[i].clear();
    }
}

void DBA::initialize() {
	downO.push_back(gateHalf("Down$o", cGate::OUTPUT));
	downO.push_back(gateHalf("Down2$o", cGate::OUTPUT));
	downO.push_back(gateHalf("Down3$o", cGate::OUTPUT));
	downO.push_back(gateHalf("Down4$o", cGate::OUTPUT));

	startNewCycleEvent = new cMessage("startNewCycleEvent");
	sendCHDownstreamEvent.push_back(new cMessage("sendCH1DownstreamEvent"));
	sendCHDownstreamEvent.push_back(new cMessage("sendCH2DownstreamEvent"));
	sendCHDownstreamEvent.push_back(new cMessage("sendCH3DownstreamEvent"));
	sendCHDownstreamEvent.push_back(new cMessage("sendCH4DownstreamEvent"));

	IniParser& ini = IniParser::GetInstance();
	onuSize = ini.sizeOfONU;
	chSize = ini.sizeOfCh;
	bufferLimit = ini.pktBufferLimit;
	downRateRec = ini.downDataRateRec;
	upRateRec = ini.upDataRateRec;
	interThres = ini.interSleepThres;
	cycleTimeLen = ini.cycleTimeLen;
	maxSizeOfACycle = ini.downDataRate * pow(10, 9) / 8 * cycleTimeLen * pow(10, -3); //maxSlotPerCh * MTW;
	chOpenThres =ini.chOpenThres;
	enableTDMAmode = ini.enableTDMAmode;

	onutbl = check_and_cast<ONUTable *>(getSimulation()->getModuleByPath("EPON.olt.onuTable"));
	cycleCount = 0;
	cycleFromZero = 0;
	minReqSizeOfAll = 1000000000;
	maxReqSizeOfAll = 0;
	activeChannel = 1;
	prevActiveCh = 1;
	thisCycleTotalGrantSize = 0;
	lastRecordChTime = 0;
	allGated = true;
	interThresDecimalPlace = 2;
	upLarger = downLarger = 0;
    downBufLarger = upBufLarger = 0;
    sleepNoGrantCount = 0;
    totalError = 0;
    totalSVError = 0;
    totalError8to11 = 0;
    totalSVError8to11 = 0;
    cycleCount8s = 0;
    cycleNum8to11 = 0;
    //onunow = 0;
    //onuold = 0;
    //grantUpold = 0;
    cycleCount2 = 0;


	InitVec(tempQueue, onuSize);
	InitVecConstant(takeUpSpaceOfCh, chSize, 0U);
	InitVecConstant(remainSpaceOfCh, chSize, maxSizeOfACycle);
	InitVec(useChAmountTimeLen, chSize);
	InitVec(readyQueue, chSize);
	InitVec(onuRequestSize, onuSize);
	InitVecConstant(bufferSizeToMTW, onuSize, 0.0f);
	InitVecConstant(gateCountOfOnu, onuSize, 0U);
	InitVecConstant(totalDownBufferSize, onuSize, 0.0);
	InitVec(onunow, onuSize);
	InitVec(onuold, onuSize);
	InitVec(grantUpold, onuSize);
	InitVec(Rerror, onuSize);

	for (uint32_t i = 0; i != onuSize; i++) {
		InitVec(buffer[i], 2);
		InitVecConstant(bufferSize[i], 2, 0U);
		InitVecConstant(pktDropped[i], 2, 0U);
		InitVecConstant(downedBytes[i], 2, 0U);
		isDownEnd[i] = true;
		waitingReport[i] = false;
		gateSent[i] = true;
		accumulateUpstreamBytes[i] = 0;
		accumulateDownBufferBytes[i] = 0;
		minReqSizeOfOnu[i] = 10000000;
		maxReqSizeOfOnu[i] = 0;
		noReqCount[i] = 0;
		totalReqCount[i] = 0;
		largerGrant[i] = 0;
	}

	//////Added 20210611
	for (uint32_t i = 0 ; i != onuSize; i++){
	    InitVecConstant(timesteps[i], 50, 0U); // Change to 20
	}
	//////


	const char *s = getEnvir()->getConfig()->getConfigValue("sim-time-limit");
	cout << "sim-Time-Limit: " << atoi(s) - 1 << " second(s)" << endl;
}

void DBA::handleMessage(cMessage *msg) {
	if (msg->isSelfMessage()) {
		if (msg == startNewCycleEvent) {
			ScheduleNewCycle();
		}
		else if (FindValInVec(sendCHDownstreamEvent, msg)) {
			int ch = -1;
			int chIndex = GetIndexOfValInVec(sendCHDownstreamEvent, msg);
			if (msg == sendCHDownstreamEvent[chIndex] && !readyQueue[chIndex].isEmpty())
				ch = chIndex;
			if (ch != -1) {
				MyPacket * pkt = check_and_cast<MyPacket*>(readyQueue[ch].pop());
				uint32_t destIdx = pkt->getDestAddr() - 2;

				bool isMPCP = (pkt->getKind() == MPCP_TYPE) ? true : false;
				if (isMPCP) {
					CheckIfAllOnuGated(destIdx);
				}
				else if (simTime() > 1 && !isMPCP) {
					accumulateUpstreamBytes[destIdx] += pkt->getByteLength();
					downedBytes[destIdx][pkt->getPriority()] += pkt->getByteLength();
				}

				if (pkt->getLastPkt()) {
					isDownEnd[destIdx] = true;
				}
				nextTransmitTime = simTime() + downRateRec * pkt->getBitLength();
				send(pkt, downO[ch]);
				scheduleAt(nextTransmitTime, sendCHDownstreamEvent[ch]);
			}

			bool allReadyQueueEmpty = true;
			for (int i = 0; i < chSize; i++) {
				if (!readyQueue[i].isEmpty()) {
					allReadyQueueEmpty = false;
					break;
				}
			}
			if (allReadyQueueEmpty) {
				if(!startNewCycleEvent->isScheduled())
					scheduleAt(simTime(), startNewCycleEvent);
			}
		}
	}
	else { //network message
		if (msg->getArrivalGate() == gate("gen")) { // Receive from trafficeGen by directSend then store and classify packets
			PushIntoBuffer(msg);
		}
		else if (strcmp(msg->getName(), "Ready") == 0) { // Start point
			delete msg;
			ScheduleNewCycle();
		}
		else if (strcmp(msg->getClassName(), "MPCPReport") == 0) {
			ProcessReport(check_and_cast<MPCPReport *>(msg));
			delete msg;
		}
	}
}

void DBA::finish() {

// ---------- DBA ----------
	LogResults o("DBA");
	for (uint16_t i = 0; i != onuSize; i++) {
		o << i << " UP: " << (double) accumulateUpstreamBytes[i] * 8 / pow(10, 6) << "M bits" << " DOWN: " << (double) accumulateDownBufferBytes[i] * 8 / pow(10, 6) << "M bits," << " HQ: " << buffer[i][0].getLength() << "pkts("
				<< bufferSize[i][0] * 8 / pow(10, 6) << " MBits);" << " LQ: " << buffer[i][1].getLength() << "pkts(" << bufferSize[i][1] * 8 / pow(10, 6) << " MBits);";
		o << " dropCount: " << pktDropped[i][0] + pktDropped[i][1] << "; " << "down[0]: " << downedBytes[i][0] * 8 / pow(10, 6) << " MBits);" << "down[1]: " << downedBytes[i][1] * 8 / pow(10, 6) << " MBits);" << endl;
	}
	o << " cycle count = " << cycleCount << ", avg cycleTime = " << simuTimeLen / cycleCount << endl;
	o << "\n\n\n";

// ---------- ReqLen ----------
	LogResults o2("ReqLen");
	IniParser& ini = IniParser::GetInstance();
	double upLoad = ini.upOfferedLoad, downLoad = ini.downOfferedLoad;
	o2 << upLoad * 100 << "% " << downLoad * 100 << "% " << " minReqLen maxReqLen countReqZero countReqAll countReqZeroRatio" << endl;

	for (uint16_t i = 0; i != onuSize; i++) {
		o2 << i << " " << minReqSizeOfOnu[i] << " " << maxReqSizeOfOnu[i] << " " << noReqCount[i] << " " << totalReqCount[i] << " " << (double) noReqCount[i] / totalReqCount[i] << "\n";
	}
	o2 << " meanMinReqLen=" << minReqSizeOfAll << " meanMaxReqLen" << maxReqSizeOfAll << endl;
	o2 << "\n\n\n";

// --------- DownQueueSize ----------
	LogResults o3("DownQueueSize");
	o3 << upLoad * 100 << "%  " << downLoad * 100 << "% " << endl;
	for (uint16_t i = 0; i != onuSize; i++) {
		o3 << i << " AvgDownQueueSizePerCycle: " << totalDownBufferSize[i] / gateCountOfOnu[i] << " MB, Gated cycle: " << gateCountOfOnu[i] << endl;
	}
	o3 << "\n\n\n" << endl;

// --------- ChannelUsage ----------
	LogResults o4("ChannelUsage");
	o4 << upLoad * 100 << "%  " << downLoad * 100 << "% " << endl;
	for(int i=0; i<chSize; i++){
		o4 << i+1 << " channel time length :\t" << useChAmountTimeLen[i] << endl;
	}
	o4 << "\n\n" << endl;

// ---------- CycleLength ----------
	LogResults o5("CycleLength");
	o5 << "Cycle count:\t" << cycleCount << endl;
	o5 << "avg cycle length =\t" << cycleTimeAccu/cycleCount << endl;
	o5 << "avg cycle gap =\t" << cycGapTimeAccu/cycleCount << endl;
	o5 << "\n\n" << endl;

// -------------------------------------

	LogResults o7("DBA_cycle");
	o7 << "cycleFromZero : " << cycleFromZero << endl;
	o7 << "cycleCount : " << cycleCount << endl;

//---------------------------------------
	LogResults o8("End_Of_Clock_time_In_DBA");
	o8 << "End Of Clock time : "  << clock() << endl;

//---TotalError
	LogResults o9("TotalError_Full");
	uint32_t averageTotalError, SD;
	averageTotalError = totalError / (cycleFromZero);
	SD = sqrt(totalSVError / (cycleFromZero));
	o9 << " totalError : " << totalError << " averageTotalError_PerCycle : " << averageTotalError  << " Standard_Deviation_PerCycle : " << SD << endl;

	LogResults o10("TotalError_8to11");
	uint32_t averageTotalError8to11, SD8to11;
	averageTotalError8to11 = totalError8to11 / cycleNum8to11;
	SD8to11 = sqrt(totalSVError8to11 / cycleNum8to11);
	o10 << " totalError8to11 : " << totalError8to11 << " averageTotalError8to11_PerCycle : " << averageTotalError8to11  << " Standard_Deviation8to11_PerCycle : " << SD8to11 << endl;
}
