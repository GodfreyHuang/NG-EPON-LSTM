#include <omnetpp.h>
#include "../common/IniParser.h"
#include "../traffic_gen/Messages/EPON_messages_m.h"

using namespace std;
using namespace omnetpp;

class PON_Splitter : public cSimpleModule {
private:
	int onuSize;

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void finish();
};

Define_Module(PON_Splitter);

void PON_Splitter::initialize() {
    IniParser& ini = IniParser::GetInstance();
	onuSize = ini.sizeOfONU;
}

void PON_Splitter::handleMessage(cMessage *msg) {
	cGate *fromGate = msg->getArrivalGate();
	const char *gateName = fromGate->getFullName();
	if (strcmp(gateName, "oltDown$i") == 0) { // Downstream broad-casting for MPCPdiscovery
		int outGateBaseId = gateBaseId("onuDown$o");
		for (int i = 0; i < onuSize; i++)
			send(i == onuSize - 1 ? msg : msg->dup(), outGateBaseId + i);
	}
}

void PON_Splitter::finish(){
}
