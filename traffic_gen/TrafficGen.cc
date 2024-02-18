#include <omnetpp.h>
#include "../traffic_gen/Messages/EPON_messages_m.h"
#include "../common/IniParser.h"
#include "../common/LogResults.h"
#include <fstream>
#include <vector>
#include <iostream>

using namespace std;
class TrafficGen : public cSimpleModule {
public:
	TrafficGen();
	~TrafficGen();
private:
	double offered_load, max_rate, highPriorityRatio, send_rate;
	cMessage *sendTrafficEvent, *nextArrivalEvent, *triggerEvent;
	cQueue txQueue;
	uint32_t onuDownBytes;
	//
	double local_throughput, interpacket_time;
	//

	// self-similar variables
	bool pareto_on;
	uint16_t packet_train_length, mean_frame_size, on_packet_train_length, off_packet_train_length;
	double pareto_rate, next_switch, frameTimeGap, scale_on, scale_off, t_on, t_off, alpha_on, alpha_off, beta_on, beta_off, coef_on, coef_off;

	void generateDataFrame();

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void finish();
};

Define_Module(TrafficGen);

TrafficGen::TrafficGen() {
	sendTrafficEvent = NULL;
	nextArrivalEvent = NULL;
	triggerEvent = NULL;
}

TrafficGen::~TrafficGen() {
	cancelAndDelete(sendTrafficEvent);
	cancelAndDelete(nextArrivalEvent);
	cancelAndDelete(triggerEvent);

	txQueue.clear();
}

void TrafficGen::initialize() {
	// everything start from here...well, sort of
	// preventing you from waste time, check if log function work or not
	LogResults *testLog = new LogResults("TESTLOGFUNC");
	delete testLog;

	sendTrafficEvent = new cMessage("sendTrafficEvent");
	nextArrivalEvent = new cMessage("nextArrivalEvent");
	triggerEvent = new cMessage("triggerEvent");

	scheduleAt(simTime(), nextArrivalEvent);
	scheduleAt(simTime(), triggerEvent);

	IniParser& ini = IniParser::GetInstance();
	offered_load = ini.downOfferedLoad;
	highPriorityRatio = ini.downHighRatio;
	send_rate = ini.downSendRate;
	max_rate = 1 / send_rate; // Gbps
	//
	local_throughput = offered_load * max_rate;
	interpacket_time = ((((minEthFrame + maxEthFrame) / 2) * 8) / local_throughput);
	//

	// self-similar setting
	pareto_rate = ini.paretoRate;
    alpha_on = ini.paretoAlphaOn;
    alpha_off = ini.paretoAlphaOff;
    beta_on = ini.paretoBetaOn;
	mean_frame_size = (minEthFrame + maxEthFrame / 2.0) * 8.0;
	frameTimeGap = mean_frame_size / max_rate;
	coef_on = pow(((1.19 * alpha_on) - 1.166), -0.027);
	coef_off = pow(((1.19 * alpha_off) - 1.166), -0.027);
	t_on = (alpha_on - 1) / alpha_on;
	t_off = (alpha_off - 1) / alpha_off;
	scale_on = 1 - pow(DBL_MIN, t_on);
	scale_off = 1 - pow(DBL_MIN, t_off);
	beta_off = (coef_on / coef_off) * (t_off / t_on) * (scale_on / scale_off) * ((1 / offered_load) - 1);
	pareto_on = false;

	onuDownBytes = 0;
}
 //self traffic
/*
void TrafficGen::handleMessage(cMessage *msg) {
	if (offered_load == 0)
		return;

	if (msg == nextArrivalEvent) {
	    if (pareto_on && on_packet_train_length-- > 0) {
	        generateDataFrame();
	        scheduleAt(simTime() + frameTimeGap, nextArrivalEvent);
	    }

		if (!sendTrafficEvent->isScheduled())
			scheduleAt(simTime(), sendTrafficEvent);
	}
	else if (msg == triggerEvent) {
		pareto_on = !pareto_on;

		if (pareto_on) {
			on_packet_train_length = round(pareto_shifted(alpha_on, beta_on, 0, this->getIndex()));
			next_switch = on_packet_train_length * frameTimeGap;
			scheduleAt(simTime(), nextArrivalEvent);
		}
		else {
			cancelEvent(nextArrivalEvent);
			off_packet_train_length = round(pareto_shifted(alpha_off, beta_off, 0, this->getIndex()));
			next_switch = off_packet_train_length * frameTimeGap;
		}
		scheduleAt(simTime() + next_switch, msg);

		if (!sendTrafficEvent->isScheduled())
			scheduleAt(simTime(), sendTrafficEvent);
	}
	else if (msg == sendTrafficEvent) {
		if (txQueue.isEmpty())
			return;
		else {
			MyPacket * pkt = check_and_cast<MyPacket *>(txQueue.pop());
			sendDirect(pkt, getSimulation()->getModuleByPath("EPON.olt.dba"), "gen");
			scheduleAt(simTime() + pkt->getBitLength() * send_rate, sendTrafficEvent);
		}
	}
}
*/
/* //poisson traffic
void TrafficGen::handleMessage(cMessage *msg) {
    if (offered_load == 0)
        return;
    if (msg == nextArrivalEvent) {
        if (pareto_on && on_packet_train_length-- > 0) {
            generateDataFrame();
            //self
            //scheduleAt(simTime() + frameTimeGap, nextArrivalEvent);
            //

            //poisson
            scheduleAt(simTime() + exponential(interpacket_time, this->getIndex()), nextArrivalEvent);
            //
        }
        if (!sendTrafficEvent->isScheduled())
            scheduleAt(simTime(), sendTrafficEvent);
    }
    else if (msg == triggerEvent) {
        pareto_on = !pareto_on;
        if (pareto_on) {
            on_packet_train_length = round(pareto_shifted(alpha_on, beta_on, 0, this->getIndex()));
            next_switch = on_packet_train_length * frameTimeGap;
            scheduleAt(simTime(), nextArrivalEvent);
        }
        else {
            cancelEvent(nextArrivalEvent);
            off_packet_train_length = round(pareto_shifted(alpha_off, beta_off, 0, this->getIndex()));
            next_switch = off_packet_train_length * frameTimeGap;
        }
        scheduleAt(simTime() + next_switch, msg);
        if (!sendTrafficEvent->isScheduled())
            scheduleAt(simTime(), sendTrafficEvent);
    }
    else if (msg == sendTrafficEvent) {
        if (txQueue.isEmpty())
            return;
        else {
            MyPacket * pkt = check_and_cast<MyPacket *>(txQueue.pop());
            sendDirect(pkt, getSimulation()->getModuleByPath("EPON.olt.dba"), "gen");
            scheduleAt(simTime() + pkt->getBitLength() * send_rate, sendTrafficEvent);
        }
    }
}
*/
///* //constant traffic
void TrafficGen::handleMessage(cMessage *msg) {
    if (offered_load == 0)
        return;

    if (msg == nextArrivalEvent)
    {
        generateDataFrame();
        scheduleAt(simTime() + 0.00001333, nextArrivalEvent);
        // ----
        //pktArrivalRate += 0.00002;
        //pktCount++;
        // ----
        if (!sendTrafficEvent->isScheduled())
            scheduleAt(simTime() + 0.00001333, sendTrafficEvent);
    }
    else if (msg == sendTrafficEvent) {
        if (txQueue.empty())
            return;
        else
        {
            MyPacket * pkt = check_and_cast<MyPacket *>(txQueue.pop());
            sendDirect(pkt, getSimulation()->getModuleByPath("EPON.olt.dba"), "gen");
            //scheduleAt(simTime()+pkt->getBitLength()pow(10,-8), sendTrafficEvent);   // 100 Mbps
            scheduleAt(simTime() + 0.00001333, sendTrafficEvent);
        }
    }
}
//*/
void TrafficGen::generateDataFrame() {
	MyPacket *job = new MyPacket("traffic");
	job->setTimestamp();
	job->setDestAddr(this->getIndex() + 2);

	int16_t hpRatio = highPriorityRatio * 1000;

	//uint32_t len = intuniform(minEthFrame, maxEthFrame, this->getIndex());
    uint32_t len = 1;
	job->setByteLength(len);

	uint16_t pri = intuniform(1, 1000, this->getIndex());

	onuDownBytes += len;

	/*
	if (pri > 1000 - hpRatio)
		job->setPriority(0);
	else
		job->setPriority(1);
	*/

	job->setPriority(0);///Added!

	txQueue.insert(job);
}

void TrafficGen::finish() {
	LogResults o("TrafficGen");

	o << "OFFER_LOAD : " << offered_load << endl;
	o << "ONU[" << this->getIndex() << "] downstream: " << onuDownBytes / pow(10, 6) * 8 << "M bits" << endl;

	IniParser& ini = IniParser::GetInstance();
	int onuSize = ini.sizeOfONU;
	if (this->getIndex() == onuSize - 1)
		o << "\n\n";
}
