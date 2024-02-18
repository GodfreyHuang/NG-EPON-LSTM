#include "ONUTable.h"

Define_Module(ONUTable);

ONUTable::ONUTable() {
}

ONUTable::~ONUTable() {
	for (uint16_t i = 0; i != onuSize; i++) {
		cancelAndDelete(sleepTimeOutEventVec[i]);
	}
}

void ONUTable::initialize() {
    IniParser& ini = IniParser::GetInstance();
	onuSize = ini.sizeOfONU;

	for (uint16_t i = 0; i != onuSize; i++) {
		stringstream sleepName, dozeName;
		sleepName << "sleepTimeOutEvent[" << i << "]";
		dozeName << "dozeTimeOutEvent[" << i << "]";
		TimeOutMsg * sleepTimeOutEvent = new TimeOutMsg(sleepName.str().c_str());
		TimeOutMsg * dozeTimeOutEvent = new TimeOutMsg(dozeName.str().c_str());
		sleepTimeOutEvent->setKind(TIMEOUT_TYPE);
		dozeTimeOutEvent->setKind(TIMEOUT_TYPE);
		sleepTimeOutEvent->setOnuIdx(i);
		dozeTimeOutEvent->setOnuIdx(i);
		sleepTimeOutEventVec.push_back(sleepTimeOutEvent);
	}

	eocTime = 0;
}

void ONUTable::handleMessage(cMessage *msg) {
	if (msg->getKind() == TIMEOUT_TYPE) { // doze/sleep time out event
		TimeOutMsg *tom = check_and_cast<TimeOutMsg *>(msg);
		uint16_t idx = tom->getOnuIdx();

		if (sleepTimeOutEventVec[idx] == tom) {
			GetEntry(idx)->mode = ACTIVE;
		}
	}
	else if (strcmp(msg->getClassName(), "RTTReg") == 0) { // Register ONU
		RegisterONU(check_and_cast<RTTReg*>(msg));
		if (GetTableSize() == onuSize) { // all ONU have been registered
			cMessage *ready = new cMessage("Ready");
			sendDirect(ready, getSimulation()->getModuleByPath("EPON.olt.dba"), "in"); // DBA ready to go
		}
	}
}

void ONUTable::finish() {
}

ONUTableEntry * ONUTable::GetEntry(uint32_t idx) {
	if (idx < 0 || idx >= tbl.size())
		return NULL;
	return &tbl[idx];
}

void ONUTable::RegisterONU(RTTReg *msg) {
	ONUTableEntry entry;
	entry.RTT = msg->getRtt();
	entry.LLID = msg->getSrcAddr();

	if(msg->getRtt() > largestRTT)
		largestRTT = msg->getRtt();

	tbl.push_back(entry);

	delete msg;
}

void ONUTable::UpdateTable(MPCPReport * msg) {
	Enter_Method("UpdateTable()");
	uint16_t idx = msg->getSrcAddr() - 2;
	ONUTableEntry * entry = GetEntry(idx);
	entry->mode = msg->getCurMode();
	entry->lastUpdate = simTime();

	if (entry->mode == SLEEP) {
		if (sleepTimeOutEventVec[idx]->isScheduled())
			cancelEvent(sleepTimeOutEventVec[idx]);
		simtime_t halfRTT = entry->RTT / 2;
		simtime_t remainTimeLen = SLEEPTIME * pow(10, -3) - halfRTT;
		scheduleAt(simTime() + remainTimeLen, sleepTimeOutEventVec[idx]);
	}

	if (entry->mode == ACTIVE || entry->mode == DOZE) {
		cancelEvent(sleepTimeOutEventVec[idx]); // cancel sleep timeout timer
	}
}
