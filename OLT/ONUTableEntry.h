#pragma once
#include "omnetpp.h"
#include "../common/IniParser.h"

using namespace omnetpp;
using namespace std;

class ONUTableEntry {
public:
	simtime_t RTT, lastUpdate;
	uint16_t LLID, // ONU index
			mode;
	int currentCh;
	bool heavyLoading;
public:
	ONUTableEntry();
	~ONUTableEntry();
};
