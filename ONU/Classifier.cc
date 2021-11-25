#include "Classifier.h"
#include "../OLT/DBA.h"

Define_Module(Classifier);

Classifier::Classifier() {
	sendUpstreamEvent = new cMessage("sendUpstreamEvent");
	checkHPQueue = new cMessage("checkHPQueue");
	iiDozeWakeEvent = new cMessage("iiDozeWakeEvent");
	iiWakeEvent = new cMessage("iiWakeEvent");
}

Classifier::~Classifier() {
	cancelAndDelete(sendUpstreamEvent);
	cancelAndDelete(checkHPQueue);
	cancelAndDelete(iiDozeWakeEvent);
	cancelAndDelete(iiWakeEvent);
}

void Classifier::initialize() {
    IniParser& ini = IniParser::GetInstance();
	bufferLimit = ini.pktBufferLimit;
	onuSize = ini.sizeOfONU;
	upRateRec = ini.upDataRateRec;
	tuningTimeLen = ini.tuningTime;

	upO.push_back(gateHalf("Up$o", cGate::OUTPUT));
	upO.push_back(gateHalf("Up2$o", cGate::OUTPUT));
	upO.push_back(gateHalf("Up3$o", cGate::OUTPUT));
	upO.push_back(gateHalf("Up4$o", cGate::OUTPUT));

	upI = gateHalf("Up$i", cGate::INPUT);
	up2I = gateHalf("Up2$i", cGate::INPUT);
	up3I = gateHalf("Up3$i", cGate::INPUT);
	up4I = gateHalf("Up4$i", cGate::INPUT);

	downO = gateHalf("Down$o", cGate::OUTPUT);
	down2O = gateHalf("Down2$o", cGate::OUTPUT);
	down3O = gateHalf("Down3$o", cGate::OUTPUT);
	down4O = gateHalf("Down4$o", cGate::OUTPUT);

	downI = gateHalf("Down$i", cGate::INPUT);
	down2I = gateHalf("Down2$i", cGate::INPUT);
	down3I = gateHalf("Down3$i", cGate::INPUT);
	down4I = gateHalf("Down4$i", cGate::INPUT);

	string powerConPath = getParentModule()->getFullPath() + ".powerControler";
	pwCtrler = dynamic_cast<PowerControler *>(getSimulation()->getModuleByPath(powerConPath.c_str()));

	prevTriMode = ACTIVE;

	highPriBufferSize = 0, lowPriBufferSize = 0;
	totalUploadedBytes = 0;
	onuStayChannel = 0;
	needTuning = false;

	///Added
	total_upQueueTotalLen = 0;
	grantUpOld = 0;
	///

	lastReportTime = simTime();
	reportCycles = 0;
	totalBufferSize = 0;
	maxQueueSize = 0;
	tuningCount = 0;
}

void Classifier::handleMessage(cMessage *msg) {
    int i;
    int length;
    uint32_t singleTotalSize = 0;
    length = readyQueue.getLength();
	if (msg->isSelfMessage()) {
		if (msg == sendUpstreamEvent && !readyQueue.isEmpty()) {
		    cModule *olt = getSimulation()->getModuleByPath("EPON.olt");
		    //Try to get the numbers of packets in a single cycle.
		    /*LogResults oBreak2("ONU16_singleCyclePKTs");
		    if(getParentModule()->getId() - 2 == 16 && simTime() > 3) {
		        for (i = 0; i < readyQueue.getLength(); i++) {
		            oBreak2 << " reportCycles : " << reportCycles << "\t Queue's i : " << i << endl;
		        }
		    }*/
		    //length = readyQueue.getLength();
		    for (i = 0; i < length; i++) {
		        MyPacket * pkt = check_and_cast<MyPacket *>(readyQueue.pop());
		        pkt->setSrcAddr(getParentModule()->getId());
		        pkt->setDestAddr(olt->getId());

		        singleTotalSize += pkt->getByteLength();
		        LogResults oBreak2("ONU16_singleCyclePKTs");
		        if(getParentModule()->getId() - 2 == 16 && simTime() > 3) {
		            oBreak2 << " reportCycles : " << reportCycles << "\t Queue's i : " << i << "\t singleTotalSize : " << singleTotalSize <<endl;
		        }

		        if (pkt->getKind() != MPCP_TYPE)
		            totalUploadedBytes += pkt->getByteLength();
		        send(pkt, upO[onuStayChannel]);
		    }

		    scheduleAt(simTime(), sendUpstreamEvent);
			//cModule *olt = getSimulation()->getModuleByPath("EPON.olt");
			/*MyPacket * pkt = check_and_cast<MyPacket *>(readyQueue.pop());
			pkt->setSrcAddr(getParentModule()->getId());
			pkt->setDestAddr(olt->getId());*/

			//The doc shows how many data upload from ONU16. The numbers are as same as LocalNetwork.cc's len.
			/*LogResults oBreak("ONU16_eachUploadedBytes_QueuePKT");
			if(getParentModule()->getId() - 2 == 16 && simTime() > 3)
			{
			    oBreak << " reportCycles : " << reportCycles << "\t ONU16_UploadedBytes : " << pkt->getByteLength() << endl;
			}*/


			/*if (pkt->getKind() != MPCP_TYPE)
				totalUploadedBytes += pkt->getByteLength();
			send(pkt, upO[onuStayChannel]);
			scheduleAt(simTime(), sendUpstreamEvent);*/
		}
		else if (msg == sendUpstreamEvent && readyQueue.isEmpty()) {
			int curSize = 0;
			if (!pktBuffer[0].isEmpty()) {
				cQueue temp = pktBuffer[0];
				for (int i = 0; i < temp.getLength(); i++) {
					cPacket *tmpPkt = check_and_cast<cPacket *>(temp.pop());
					curSize = curSize + tmpPkt->getByteLength();
					delete tmpPkt;
				}
			}
			//The doc shows how many data upload from ONU16. The numbers are as same as LocalNetwork.cc's len.
			LogResults oBreak2("ONU16_eachUploadedBytes_curSize");
			if(getParentModule()->getId() - 2 == 16 && simTime() > 3)
			{
			    oBreak2 << " reportCycles : " << reportCycles << "\t curSize : " << curSize << endl;
		    }
			curSize = curSize / pow(10, 3); //KB

			if (maxQueueSize < curSize)
				maxQueueSize = curSize;
		}
		else if (msg == iiWakeEvent) { // intra/inter
			IntraInterToActive();
			//cout << "ENTER HERE!!!!!!!!A\n";
			//endSimulation();
		}
		else if (msg == iiDozeWakeEvent) { // intra/inter
			IntraInterToDoze();
			//cout << "ENTER HERE!!!!!!!!B\n";
			//endSimulation();
		}
		else if (msg == checkHPQueue) {
		    //cout << "ENTER HERE!!!!!!!!C\n";
		    //endSimulation();
			if (!pktBuffer[0].isEmpty())
				GotHighPriWakeUp();
		}
	}
	else if (msg->getArrivalGate() == upI || msg->getArrivalGate() == up2I || msg->getArrivalGate() == up3I || msg->getArrivalGate() == up4I) { // send from localNetwork (upstream)
		PushIntoBuffer(check_and_cast<MyPacket *>(msg));
	}
	else if (msg->getArrivalGate() == downI || msg->getArrivalGate() == down2I || msg->getArrivalGate() == down3I || msg->getArrivalGate() == down4I) { // send from ONU scheduler (downstream)
		if (msg->getKind() == MPCP_TYPE) {
			ProcessGate(check_and_cast<MPCPGate *>(msg));
		}
		delete msg;
	}
}

void Classifier::finish() {
    IniParser& ini = IniParser::GetInstance();

// ---------- Classifier ----------
	LogResults o("Classifier");

	o << "[" << getParentModule()->getId() - 2 << "] upstream: " << 8 * totalUploadedBytes << " bits(" << 8 * totalUploadedBytes / pow(10, 6) << " Mbits) q[0]=" << highPriBufferSize << " q[1]=" << lowPriBufferSize << endl;

	if (getParentModule()->getId() - 1 == onuSize)
		o << "\n\n\n";

// ---------- UpQueueSize ----------
	LogResults o2("UpQueueSize");
	if (getParentModule()->getId() - 2 == 0) {
	    IniParser& ini = IniParser::GetInstance();
		double upLoad = ini.upOfferedLoad,
		        downLoad = ini.downOfferedLoad;
		o2 << upLoad * 100 << "% " << downLoad * 100 << "%" << endl;
	}
	o2 << getParentModule()->getId() - 2 << " AvgUpQueueSize: " << totalBufferSize / reportCycles << " MB " << " Max queue size : " << maxQueueSize << " KB, REPORT cycle: " << reportCycles << endl;
	if (getParentModule()->getId() - 1 == onuSize)
		o2 << "\n\n\n" << endl;

// ---------- TuningCount ----------
	LogResults o4("OnuTuningCount");
	o4 << getParentModule()->getId() - 2 << "\t" << tuningCount << endl;
	if (getParentModule()->getId() - 1 == onuSize)
		o4 << "\n\n\n";

	LogResults o5("totalBufferSize");
	    o5 << getParentModule()->getId() - 2 << "\t" << "totalBufferSize : " << totalBufferSize << ", reportCycles : " << reportCycles << endl;
	    if (getParentModule()->getId() - 1 == onuSize)
	        o5 << "\n\n\n";
	LogResults o6("Classifier_Cycle");
	o6 << "reportCycles : "<< reportCycles << endl;
	o6 << "cycle : " << cycle <<endl;
	if (getParentModule()->getId() - 1 == onuSize)
	o6 << "\n\n\n";
}

void Classifier::ProcessGate(MPCPGate *gt) {

	cycle = gt->getCycleZ();
	uint32_t upQueueTotalLen = highPriBufferSize + lowPriBufferSize;
	uint32_t downstreamLen = gt->getDownLength();
	uint32_t grantUpLen = gt->getUpLength();
	uint32_t curMode = pwCtrler->GetEnergyMode();
	uint16_t channel = gt->getTransmitChannel();
	simtime_t MS = pow(10, -3);
	simtime_t startSaving;
	bool noPlanToSleep = true;
	bool planToDoze = false;
	bool upQueueEmpty = (pktBuffer[0].isEmpty() && pktBuffer[1].isEmpty()) ? true : false;
	bool downQueueEmpty = (gt->getQueueLength(0) == 0 && gt->getQueueLength(1) == 0) ? true : false;
	grantUpOld = grantUpLen;

	if (onuStayChannel != channel) {
		needTuning = true;
		onuStayChannel = channel;
	}
	if (grantUpLen > upQueueTotalLen) {
		grantUpLen = upQueueTotalLen;
	}
	if (grantUpLen > downstreamLen)
		downstreamLen = grantUpLen;

	simtime_t transmitTimeLen = downstreamLen * 8 * upRateRec;
	startSaving = simTime() + transmitTimeLen;
	if (needTuning) {
		startSaving = startSaving + tuningTimeLen;
		needTuning = false;
		tuningCount++;
	}

	// normal tri-mode
	if (curMode == ACTIVE) {
		// LOG
		totalBufferSize += (highPriBufferSize + lowPriBufferSize) / pow(10, 6); // Mbyte
		reportCycles++;
		lastReportTime = simTime();


		///Added!
		PopBuffer(grantUpLen);
		SendReport(ACTIVE, upQueueTotalLen - grantUpLen);
		prevTriMode = ACTIVE;

		/* Hide it!
		if (upQueueEmpty) { // no up
			if (gt->getDownLength() == 0 && downQueueEmpty) {
				SendReport(SLEEP, upQueueTotalLen - grantUpLen);
				pwCtrler->inTriSleep = true;
				pwCtrler->ChangeMode(SLEEP, startSaving, SLEEPTIME * MS, 30); // up queue is empty, no down, down queue is empty
				prevTriMode = SLEEP;
				noPlanToSleep = false;
			}
			else {
				SendReport(DOZE, upQueueTotalLen - grantUpLen);
				planToDoze = true;
			}
		}
		else { // remain active
			PopBuffer(grantUpLen);
			SendReport(ACTIVE, upQueueTotalLen - grantUpLen);
			prevTriMode = ACTIVE;
		}
		*/

		//if (simTime() > 8)
	    //    print repotCycles <- using LogResult.
		if(getParentModule()->getId() - 2 == 16) //why process_gate message is ONU 16.
		{
		    total_upQueueTotalLen = total_upQueueTotalLen + upQueueTotalLen;
		    LogResults owyo2("ProcessGate_ONU16_upQueueTotalLen");
		    owyo2 << "cycle : " << cycle << ", reportCycles : " << reportCycles << "\t"<< getParentModule()->getId() - 2 << "\t" << "upQueueTotalLen : " << total_upQueueTotalLen << endl;
		}
		///*///////////////////// Added

		if(getParentModule()->getId() - 2 == 16  && simTime() > 3) //compare with ONU16_onuRequestSize.
		{
		    LogResults owoo("ProcessGate_ONU16_3s_request");
		    owoo << "cycle : " << cycle  << ", reportCycles : " << reportCycles << "\t"<< getParentModule()->getId() - 2 << "\t RequestSize : " << upQueueTotalLen - grantUpLen << "\t totalBufferSize : " << totalBufferSize<<"\t grantUpLen : "<< grantUpLen << "\t grantDownLen : "<<downstreamLen <<endl;
		}

		if(getParentModule()->getId() - 2 == 16)
		{
		    LogResults owoo("ProcessGate_ONU16_request");
		    owoo << "cycle : " << cycle  << ", reportCycles : " << reportCycles << "\t"<< getParentModule()->getId() - 2 << "\t RequestSize : " << upQueueTotalLen - grantUpLen << "\t totalBufferSize : " << totalBufferSize<<"\t grantUpLen : "<< grantUpLen << "\t grantDownLen : "<<downstreamLen <<endl;

		    LogResults owoo2("ProcessGate_ONU16_request_needTuning");
		    owoo2 << "cycle : " << cycle  << ", reportCycles : " << reportCycles << ", needTuning : " << needTuning << endl;
		}

		if(getParentModule()->getId() - 2 != 16) //the data without ONU 16. In theory this file can be train.
		{
		    LogResults oo2("ProcessGate_ONUss_request");
		    oo2 << "cycle : " << cycle << ", reportCycles : " << reportCycles << "\t" << getParentModule()->getId() - 2 << "\t RequestSize : " << upQueueTotalLen - grantUpLen << "\t totalBufferSize : " << totalBufferSize<<"\t grantUpLen : "<< grantUpLen << "\t grantDownLen : "<<downstreamLen <<endl;
		}


		///////////////////////*/
	}
	else if(curMode == DOZE || curMode == SLEEP){
		if (gt->getDownLength() == 0 && downQueueEmpty) { // up queue is already empty. no down, down queue is empty
			if (curMode != SLEEP) {
				pwCtrler->inTriSleep = true;
				pwCtrler->ChangeMode(SLEEP, simTime(), SLEEPTIME * MS, 31); // no report
				prevTriMode = SLEEP;
				noPlanToSleep = false;
			}
		}
		else { // do nothing, keep DOZE or SLEEP
			if (curMode == DOZE) {
				prevTriMode = DOZE;
			}
			else if(curMode == SLEEP){
				prevTriMode = SLEEP;
			}
		}
		SendReport(-1, upQueueTotalLen - grantUpLen); //not formal report
	}

	/*
	simtime_t tmpTargetTime;
	if (gt->getInterSleep()) {
		tmpTargetTime = gt->getEndOfInterSleep();
	}
	else {
		tmpTargetTime = gt->getEndOfCycleTime();
	}


	if (noPlanToSleep && curMode != SLEEP && ((tmpTargetTime - startSaving) > 250*pow(10, -6))) {
		if (gt->getInterSleep()){
			pwCtrler->ChangeMode(SLEEP, startSaving, SLEEPTIME * MS, 32);
		}
		else{
			pwCtrler->ChangeMode(SLEEP, startSaving, SLEEPTIME * MS, 33);
		}

		if (prevTriMode == ACTIVE) { // ACTIVE
			if(iiWakeEvent->isScheduled())
				cancelEvent(iiWakeEvent);
			scheduleAt(tmpTargetTime - 125*pow(10, -6), iiWakeEvent);
			if (gt->getInterSleep())
				pwCtrler->SetIsInter();
			else
				pwCtrler->SetIsIntra();
		}
		else if (prevTriMode == DOZE) { // DOZE
			scheduleAt(tmpTargetTime - 125*pow(10, -6), iiDozeWakeEvent);
			if (gt->getInterSleep())
				pwCtrler->SetIsInter();
			else
				pwCtrler->SetIsIntra();
		}
		else {
			cout << "Unknown Previous Mode, " << prevTriMode << endl;
			endSimulation();
		}
	}
	else if(planToDoze){
		pwCtrler->ChangeMode(DOZE, simTime(), 0, 20);
		prevTriMode = DOZE;
	}
	*/

	if(prevTriMode!=SLEEP)
		pwCtrler->inTriSleep = false;
	if(!sendUpstreamEvent->isScheduled())
		scheduleAt(simTime(), sendUpstreamEvent);
	if (pwCtrler->GetEnergyMode() != ACTIVE && !checkHPQueue->isScheduled() && !pwCtrler->GetIsIntra() && !pwCtrler->GetIsInter())
		scheduleAt(simTime(), checkHPQueue);
}

void Classifier::IntraInterToActive() {
	if (checkHPQueue->isScheduled())
		cancelEvent(checkHPQueue);
	pwCtrler->IntraInterToActive();
	prevTriMode = ACTIVE;
}

void Classifier::IntraInterToDoze() {
	if (checkHPQueue->isScheduled())
		cancelEvent(checkHPQueue);
	pwCtrler->IntraInterToDoze();
	prevTriMode = DOZE;
}

void Classifier::GotHighPriWakeUp() {
	if(!pwCtrler->GetIsInter() && !pwCtrler->GetIsIntra()){
		pwCtrler->GeneralWakeUp();
	}
	if(pwCtrler->GetIsInter() || pwCtrler->GetIsIntra()){
		IntraInterToActive();
	}
	prevTriMode = ACTIVE;
}

void Classifier::PushIntoBuffer(MyPacket *pkt) {
	switch (pkt->getPriority()) {
		case 0:     // High priority
			if (highPriBufferSize < bufferLimit) {
				highPriBufferSize += pkt->getByteLength();
				pktBuffer[0].insert(pkt);
			}
			else
				delete pkt;
			break;
		case 1:     // Low priority
			if (lowPriBufferSize < bufferLimit) {
				lowPriBufferSize += pkt->getByteLength();
				pktBuffer[1].insert(pkt);
			}
			else
				delete pkt;
			break;
		default:
			cout<<"Unknown priority"<<endl;
			endSimulation();
	}
	if (pwCtrler->GetEnergyMode() != ACTIVE && !checkHPQueue->isScheduled() && !pwCtrler->GetIsInter() && !pwCtrler->GetIsIntra())
		scheduleAt(simTime(), checkHPQueue);
}

uint32_t Classifier::PopBuffer(uint32_t grantSize) {
	uint32_t pri = 0, topPktSize = 0, totalSize = 0;

	while (pri < 2) {
		if (pktBuffer[pri].isEmpty())
			pri++;
		else {
			topPktSize = ((MyPacket *) pktBuffer[pri].front())->getByteLength();
			if (totalSize + topPktSize <= grantSize) {
				totalSize += topPktSize;
				cPacket *tmpPkt = check_and_cast<cPacket *>(pktBuffer[pri].pop());
				readyQueue.insert(tmpPkt);
				if (pri == 0){
					if(highPriBufferSize < tmpPkt->getByteLength())
						endSimulation();
					highPriBufferSize -= tmpPkt->getByteLength();
				}
				else if (pri == 1){
					if(lowPriBufferSize < tmpPkt->getByteLength())
						endSimulation();
					lowPriBufferSize -= tmpPkt->getByteLength();
				}
			}
			else {
				break;
			}
		}
	}

	return totalSize;
}

MyPacket * Classifier::SendReport(uint16_t mode, uint32_t bufferSize) { // we report once we recerived gate msg, so you must remember to minus the buffer size with grant size here
	MPCPReport * rep = new MPCPReport();
	uint32_t curMode = pwCtrler->GetEnergyMode();

	if (curMode != ACTIVE) {
		if (!pktBuffer[0].isEmpty()) {
			GotHighPriWakeUp(); // high pri queue is not empty -> wake up
		}
	}
	rep->setCurMode(mode);
	rep->setTimestamp();
	rep->setDestAddr(getSimulation()->getModuleByPath("EPON.olt")->getId());
	rep->setSrcAddr(getParentModule()->getId());
	rep->setKind(MPCP_TYPE);
	rep->setOpcode(MPCP_REPORT);
	rep->setByteLength(64);
	rep->setLastPkt(true);
	rep->setRequestSize(bufferSize);

	if(getParentModule()->getId() - 2 == 15)
	{
	    LogResults owwo("SendReport_ONU15_BufferSize");
	    owwo << "cycle : " << cycle << ", reportCycles : " << reportCycles << "\t"<< getParentModule()->getId() - 2 << "\t" << "Buffersize : " << bufferSize << endl;
	}

	if(getParentModule()->getId() - 2 == 16)
	{
	    LogResults owyo("SendReport_ONU16_BufferSize");
	    owyo << "cycle : " << cycle << ", reportCycles : " << reportCycles << "\t"<< getParentModule()->getId() - 2 << "\t" << "Buffersize : " << bufferSize << endl;
	}

	send(rep, upO[onuStayChannel]);
	return rep;
}

void Classifier::CheckHighPriBuffer() {
	Enter_Method("CheckHighPriBuffer()");

	if (pwCtrler->GetEnergyMode() != ACTIVE && !checkHPQueue->isScheduled() && !pwCtrler->GetIsIntra() && !pwCtrler->GetIsInter())
		scheduleAt(simTime(), checkHPQueue);    //check high priority queue
}
