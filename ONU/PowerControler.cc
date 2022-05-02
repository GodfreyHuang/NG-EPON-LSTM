#include "../OLT/DBA.h"
#include "PowerControler.h"
#include "Classifier.h"

Define_Module(PowerControler);

DBA* dba;

PowerControler::PowerControler() {
    sleepTimeUpEvent = new cMessage("sleepTimeUpEvent");
    startDozeEvent = new cMessage("startDozeEvent");
    startSleepEvent = new cMessage("startSleepEvent");
    maxMpcpEvent = new cMessage("maxMpcpEvent");
}

PowerControler::~PowerControler() {
	cancelAndDelete(sleepTimeUpEvent);
	cancelAndDelete(startDozeEvent);
	cancelAndDelete(startSleepEvent);
	cancelAndDelete(maxMpcpEvent);
}

void PowerControler::initialize() {
	IniParser& ini = IniParser::GetInstance();
	onuSize = ini.sizeOfONU;
	maxMpcpTimer = ini.maxMpcpTimer;

	currentMode = ACTIVE;
    activeStartTime = simTime();
	onuIdx = getParentModule()->getId() - 2;
    active_accu = 0;
    doze_accu = 0;
    sleep_accu = 0;
    intra_accu = inter_accu = 0;

	inTriSleep = false;
	isInter = isIntra = false;
	string schedulerPath = getParentModule()->getFullPath() + ".scheduler";
	scheduler = check_and_cast<Scheduler *>(getSimulation()->getModuleByPath(schedulerPath.c_str()));

	string classifierPath = getParentModule()->getFullPath() + ".classifier";
	classifier = check_and_cast<Classifier *>(getSimulation()->getModuleByPath(classifierPath.c_str()));

	string dbaPath = "EPON.olt.dba";
	dba = check_and_cast<DBA*>(getSimulation()->getModuleByPath(dbaPath.c_str()));
}

void PowerControler::handleMessage(cMessage *msg) {
	if (msg->isSelfMessage()) {
		if (msg == sleepTimeUpEvent) {
			classifier->CheckHighPriBuffer();
			if (classifier->prevTriMode == ACTIVE) {
				ChangeMode(ACTIVE, simTime(), 0, 11); // S -> A
			}
			else{
				ChangeMode(DOZE, simTime(), 0, 21); // S -> D
			}
		}
		else if (msg == startDozeEvent) {    // -> D
			ToDoze();
		}
		else if (msg == startSleepEvent) {   // -> S
			ToSleep();
		}
		else if (msg == maxMpcpEvent) {
			GeneralWakeUp();
		}
	}
}

void PowerControler::finish() {
	LogResults o("PwCtrl");

	IniParser& ini = IniParser::GetInstance();
	double upLoad = ini.upOfferedLoad;
	double downLoad = ini.downOfferedLoad;

	if (onuIdx == 0) {
		o << upLoad * 100 << "%\t" << downLoad * 100 << "%" << endl;
	}

	o << "\t" << onuIdx;
	simtime_t totalAccu = active_accu + doze_accu + sleep_accu + intra_accu + inter_accu;
	o << "\t" << active_accu << "\t" << doze_accu << "\t" << sleep_accu << "\t" << intra_accu << "\t" << inter_accu << "\t" << totalAccu << endl;

	if (onuIdx == onuSize - 1)
		o << "\n\n\n";
}

void PowerControler::ChangeMode(uint16_t mode, simtime_t startTime, simtime_t duration, int info) { // should cancel wake up event here
	Enter_Method("ChangeMode()");
	modeInfo = info;

	if (mode == DOZE) { // Doze
		if (startDozeEvent->isScheduled()) {
			cout << "t= " << simTime() << ", onu[" << onuIdx << "] D_TIMER has been scheduled" << endl;
			cout << info << endl;
			endSimulation();
		}
		if (sleepTimeUpEvent->isScheduled())
			cancelEvent(sleepTimeUpEvent);
		if(inTriSleep)
			scheduler->RecvHoldedGate();
		scheduleAt(startTime, startDozeEvent);
	}
	else if (mode == SLEEP) { // Sleep
		if (sleepTimeUpEvent->isScheduled()) {
			cancelEvent(sleepTimeUpEvent);
			scheduleAt(startTime, startSleepEvent);
		}
		else{
			scheduleAt(startTime, startSleepEvent);
		}
		scheduleAt(startTime + duration, sleepTimeUpEvent);
	}
	else if (mode == ACTIVE) { // Active
		if (maxMpcpEvent->isScheduled())
			cancelEvent(maxMpcpEvent);
		if (sleepTimeUpEvent->isScheduled())
			cancelEvent(sleepTimeUpEvent);

		if(inTriSleep)
			scheduler->RecvHoldedGate();
		ToActive();
	}
	else {
		cout << "ERROR in PowerControler changeMode(" << mode << ", "
		<< duration << ")\n";
		endSimulation();
	}

}

void PowerControler::GeneralWakeUp(){ // D/S -> A
	Enter_Method("GeneralWakeUp()");
	ChangeMode(ACTIVE, simTime(), 0, 10);
}

void PowerControler::IntraInterToActive() { // Intra/Inter -> A
	Enter_Method("IntraInterToActive()");

	if (currentMode == SLEEP) {
		if (isIntra) {
			if (simTime() > 1)
				intra_accu += simTime() - sleepStartTime;
			isIntra = false;
		}
		else if (isInter) {
			if (simTime() > 1)
				inter_accu += simTime() - sleepStartTime;
			isInter = false;
		}

		cancelEvent(sleepTimeUpEvent);
		scheduler->RecvHoldedGate();
	}

	if (maxMpcpEvent->isScheduled())
		cancelEvent(maxMpcpEvent);
	currentMode = ACTIVE;
	activeStartTime = simTime();
}

void PowerControler::IntraInterToDoze() { // Intra/Inter -> D
	Enter_Method("IntraInterToDoze()");

	if (GetEnergyMode() == SLEEP) {
		simtime_t saveTime = simTime() - sleepStartTime;
		if (isIntra) {
			if (simTime() > 1)
				intra_accu += saveTime;
			isIntra = false;
		}
		else if (isInter) {
			if (simTime() > 1)
				inter_accu += saveTime;
			isInter = false;
		}

		cancelEvent(sleepTimeUpEvent);
		scheduler->RecvHoldedGate();
	}

	if (!maxMpcpEvent->isScheduled())
		scheduleAt(simTime() + maxMpcpTimer, maxMpcpEvent);
	currentMode = DOZE;
	dozeStartTime = simTime();
}

void PowerControler::ToActive(){ // -> A
	if (currentMode == SLEEP) {
		if (simTime() > 1)
			sleep_accu += simTime() - sleepStartTime;
	}
	else if (currentMode == DOZE) {
		if (simTime() > 1)
			doze_accu += simTime() - dozeStartTime;
	}
	else{
		cout << "There is a Active to Active, find what cause it!" << endl;
		endSimulation();
	}

	currentMode = ACTIVE;
	activeStartTime = simTime();
}

void PowerControler::ToDoze(){ // -> D
	if (!maxMpcpEvent->isScheduled())
		scheduleAt(simTime() + maxMpcpTimer, maxMpcpEvent);

	if (currentMode == SLEEP) {
		if (simTime() > 1)
			sleep_accu += simTime() - sleepStartTime;
	}
	else if (currentMode == ACTIVE) {
		if (simTime() > 1)
			active_accu += simTime() - activeStartTime;
	}
	else{
		cout << "There is a Doze to Doze, find what cause it!" << endl;
		endSimulation();
	}

	currentMode = DOZE;
	dozeStartTime = simTime();
}

void PowerControler::ToSleep(){ // -> S
	if (!maxMpcpEvent->isScheduled())
		scheduleAt(simTime() + maxMpcpTimer, maxMpcpEvent);

	if (currentMode == DOZE) {
		if (simTime() > 1)
			doze_accu += simTime() - dozeStartTime;
	}
	else if (currentMode == ACTIVE) {
		if (simTime() > 1)
			active_accu += simTime() - activeStartTime;
	}
	else{
		cout << "There is a Sleep mode to Sleep mode, find what cause it!" << endl;
		endSimulation();
	}

	currentMode = SLEEP;
	sleepStartTime = simTime();
}
