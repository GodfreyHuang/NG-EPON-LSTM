#include <omnetpp.h>
#include "../traffic_gen/Messages/EPON_messages_m.h"
#include "../common/IniParser.h"
#include "../common/LogResults.h"
#include <fstream>
#include <vector>

/////////////////////
#include "../LibFolder/fdeep/fdeep.hpp"

/////////////////////

using namespace std;
class LocalNetwork : public cSimpleModule {
public:
	LocalNetwork();
	~LocalNetwork();
private:
	double offered_load, max_rate, highPriorityRatio;
	double send_rate;
	double pktArrivalRate;
    //
    double local_throughput, interpacket_time;
    //
    int lPktCount, hPktCount, pktCount;
	cMessage *sendTrafficEvent, *nextArrivalEvent, *triggerEvent;
	cQueue txQueue;
	uint32_t onuUpBytes;
	cModule *epon = getSimulation()->getModuleByPath("EPON");

	// self-similar variables
	bool pareto_on;
	double pareto_rate, next_switch, frameTimeGap, scale_on, scale_off, t_on, t_off, alpha_on, alpha_off, beta_on, beta_off, coef_on, coef_off;
	uint16_t packet_train_length, mean_frame_size, on_packet_train_length, off_packet_train_length;

	void generateDataFrame();

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void finish();
};

Define_Module(LocalNetwork);

LocalNetwork::LocalNetwork() {
	sendTrafficEvent = NULL;
	nextArrivalEvent = NULL;
	triggerEvent = NULL;
    //
    pktArrivalRate=0;
    pktCount= 0;
    //
}

LocalNetwork::~LocalNetwork() {
	cancelAndDelete(sendTrafficEvent);
	cancelAndDelete(nextArrivalEvent);
	cancelAndDelete(triggerEvent);

	txQueue.clear();
}

void LocalNetwork::initialize() {
	sendTrafficEvent = new cMessage("sendTrafficEvent");
	nextArrivalEvent = new cMessage("nextArrivalEvent");
	triggerEvent = new cMessage("triggerEvent");

	scheduleAt(simTime(), nextArrivalEvent);
	scheduleAt(simTime(), triggerEvent);

	IniParser& ini = IniParser::GetInstance();
	offered_load = ini.upOfferedLoad;
	highPriorityRatio = ini.upHighRatio;
	send_rate = ini.upSendRate;
	max_rate = 1 / send_rate;

    //possion
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

	onuUpBytes = 0;
	pktArrivalRate = 0;
    lPktCount = hPktCount = 0;
    //
    pktCount = 0;
    //
}
/* //self traffic
void LocalNetwork::handleMessage(cMessage *msg) {
	if (offered_load == 0)
		return;

	if (msg == nextArrivalEvent) {
	    if (pareto_on && on_packet_train_length-- > 0) {
	        generateDataFrame();
	        //self-similar setting
	        scheduleAt(simTime() + frameTimeGap, nextArrivalEvent);
	        //

	    }
	    //self-similar setting
	    pktArrivalRate += frameTimeGap;
	    //

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
		scheduleAt(simTime() + next_switch, triggerEvent);

		if (!sendTrafficEvent->isScheduled())
			scheduleAt(simTime(), sendTrafficEvent);
	}
	else if (msg == sendTrafficEvent) {
		if (txQueue.isEmpty())
			return;
		else {
			MyPacket * pkt = check_and_cast<MyPacket *>(txQueue.pop());
			send(pkt, "ethUp$o");
			scheduleAt(simTime() + pkt->getBitLength() * send_rate, sendTrafficEvent);
		}
	}
}
*/
/* //poisson traffic
void LocalNetwork::handleMessage(cMessage *msg) {
    if (offered_load == 0)
        return;
    if (msg == nextArrivalEvent) {
        if (pareto_on && on_packet_train_length-- > 0) {
            generateDataFrame();
            //self-similar setting
            //scheduleAt(simTime() + frameTimeGap, nextArrivalEvent);
            //
            //possion
            scheduleAt(simTime() + exponential(interpacket_time, this->getIndex()), nextArrivalEvent);
            //
        }
        //self-similar setting
        //pktArrivalRate += frameTimeGap;
        //
        //possion
        pktArrivalRate += exponential(interpacket_time, this->getIndex());
        pktCount++;
        //
        if (!sendTrafficEvent->isScheduled())
            scheduleAt(simTime(), sendTrafficEvent);
    }
    else if (msg == triggerEvent) {
        pareto_on = !pareto_on;
        if (pareto_on) {
            on_packet_train_length = round(pareto_shifted(alpha_on, beta_on, 0, this->getIndex()));
            next_switch = on_packet_train_length * frameTimeGap;
            //self-similar setting
            scheduleAt(simTime(), nextArrivalEvent);
            //
            //possion
            //scheduleAt(simTime() + exponential(interpacket_time, this->getIndex()), nextArrivalEvent);
            //
        }
        else {
            cancelEvent(nextArrivalEvent);
            off_packet_train_length = round(pareto_shifted(alpha_off, beta_off, 0, this->getIndex()));
            next_switch = off_packet_train_length * frameTimeGap;
        }
        scheduleAt(simTime() + next_switch, triggerEvent);

        if (!sendTrafficEvent->isScheduled()) {
            //self-simular setting
            scheduleAt(simTime(), sendTrafficEvent);
            //
            //possion
            //scheduleAt(simTime() + exponential(interpacket_time, this->getIndex()), nextArrivalEvent);
            //
        }
    }
    else if (msg == sendTrafficEvent) {
        if (txQueue.isEmpty())
            return;
        else {
            MyPacket * pkt = check_and_cast<MyPacket *>(txQueue.pop());
            send(pkt, "ethUp$o");
            scheduleAt(simTime() + pkt->getBitLength() * send_rate, sendTrafficEvent);
        }
    }
}

*/
///* //constant traffic
void LocalNetwork::handleMessage(cMessage *msg) {
    if (offered_load == 0)
        return;

    if (msg == nextArrivalEvent)
    {
        generateDataFrame();
        scheduleAt(simTime() + 0.00001333, nextArrivalEvent); //CT: 8G = 0.00002, 12G = 0.00001333, 16G = 0.00001, 20G = 0.000008
        // ----
        pktArrivalRate += 0.00001333;
        pktCount++;
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
            send(pkt, "ethUp$o");
            //scheduleAt(simTime()+pkt->getBitLength()pow(10,-8), sendTrafficEvent);   // 100 Mbps
            scheduleAt(simTime() + 0.00001333, sendTrafficEvent);
        }
    }
}
//*/

void LocalNetwork::generateDataFrame() {
	MyPacket *job = new MyPacket("traffic");
	job->setTimestamp();
	int16_t hpRatio = highPriorityRatio * 1000;

	//uint32_t len = intuniform(minEthFrame, maxEthFrame, this->getIndex());
    uint32_t len = 625;
	job->setByteLength(len);
	uint16_t pri = intuniform(1, 1000, this->getIndex());
	onuUpBytes += len;
/*
    LogResults oLocal("ONU16_generateDataFrame");
    if(this->getIndex() == 16)
    {
        oLocal << " ONU16_generateDataFrame : " << len << "\t simtime = " << simTime() << endl;
    }
*/
	/*
	if (pri > 1000 - hpRatio)
		job->setPriority(0);
	else
		job->setPriority(1);

	if(job->getPriority() == 0)
		hPktCount++;
	else
		lPktCount++;
	*/


	job->setPriority(0);///Added!
	hPktCount++;///Added!

	txQueue.insert(job);
}

void LocalNetwork::finish() {
	LogResults o("LocalNet");

	o << "ONU[" << this->getIndex() << "] upstream: " << onuUpBytes / pow(10, 6) * 8 << " Mbits" << " avgArrival=" << pktArrivalRate / (lPktCount+hPktCount)  << " lamba : " << interpacket_time << endl;

	IniParser& ini = IniParser::GetInstance();
	int onuSize = ini.sizeOfONU;
	if (this->getIndex() == onuSize - 1)
		o << "\n\n";

	LogResults o2("LocalNetHiRate");
	o2 << "ONU[" << this->getIndex() << "] H: " << hPktCount << ", L: " << lPktCount << ", HighPriRatio: " << (float)hPktCount / (lPktCount+hPktCount) * 100 << "%" << endl;
	if (this->getIndex() == onuSize - 1)
		o2 << "\n\n";

	LogResults o4("TryAndError12345");
	o4 << "ONU[" << this->getIndex() << "] "  <<  endl;
	if (this->getIndex() == onuSize - 1)
	    o4 << "\n\n";


}
