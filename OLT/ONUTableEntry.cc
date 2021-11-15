#include "ONUTableEntry.h"

ONUTableEntry::ONUTableEntry() {
	LLID = 0;
	lastUpdate = simTime();
	mode = ACTIVE;
	currentCh = 1;
	heavyLoading = false;
}

ONUTableEntry::~ONUTableEntry() {
}
