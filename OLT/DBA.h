#pragma once
#include <omnetpp.h>
#include <vector>
#include <map>
#include <algorithm>
#include "../traffic_gen/Messages/EPON_messages_m.h"
#include "../common/LogResults.h"
#include "../common/IniParser.h"
#include "../common/TemplateCollect.h"
#include "ONUTableEntry.h"
#include "ONUTable.h"

#include "../LibFolder/fdeep/fdeep.hpp"
#include "../ONU/Classifier.h"

using namespace std;
using namespace omnetpp;

class DBA : public cSimpleModule {
public:
	DBA();
	~DBA();

private:
	cMessage* startNewCycleEvent;
	ONUTable * onutbl;

	vector<cGate*> downO;
	vector<cMessage*> sendCHDownstreamEvent;
	vector<cQueue> readyQueue;
	vector<cQueue> tempQueue;
	vector<int> chAllocatedToOnu;
	vector<int64_t> onuRequestSize;
	vector<int32_t> onunow;
	vector<int32_t> onuold;
	vector<int32_t> grantUpold;
	vector<simtime_t> useChAmountTimeLen;
	vector<uint32_t> gateCountOfOnu;
	vector<double> totalDownBufferSize;
	vector<float> bufferSizeToMTW;
	vector<int> heavyOnu;
	vector<int> lightOnu;
	vector<uint32_t> takeUpSpaceOfCh;
	vector<int> remainSpaceOfCh;

	/////// Added 20210611
	map<uint32_t, vector<uint32_t>> timesteps;
	///////

	map<int, vector<int>> onuTxOrder;
	map<uint16_t, uint32_t> accumulateUpstreamBytes, accumulateDownBufferBytes;
	map<uint32_t, bool> isDownEnd, waitingReport, gateSent;
	map<uint32_t, uint32_t> grantDown, minReqSizeOfOnu, maxReqSizeOfOnu, noReqCount, totalReqCount;
	map<uint32_t, int64_t> grantUp;
	map<uint32_t, int> largerGrant;
	map<uint16_t, vector<cQueue> > buffer; // [module id][priority] = packet queue
	map<uint16_t, vector<uint32_t> > bufferSize; // [module id][priority] = packet queue size
	map<uint16_t, vector<uint32_t> > pktDropped; // [module id][priority] = count of packets that can't fill in queue(dropped packet count)
	map<uint16_t, vector<uint32_t> > downedBytes;

	int maxSizeOfACycle; // bytes
    int cycleTimeLen;
    int activeChannel;
    int prevActiveCh;
    int interThresDecimalPlace; // decimal place of Inter-sleep threshold
    double downRateRec, upRateRec;
	bool allGated;
	bool enableTDMAmode;
	bool downReqLarger;
	float interThres;
	float chOpenThres;
	simtime_t nextTransmitTime, lastRecordChTime;
    simtime_t simuTimeLen;
	simtime_t cycleStartTime;
	simtime_t lastCycleStartTime;
	simtime_t totalCalTime;
	simtime_t cycleTimeAccu;
	simtime_t cycGapTimeAccu;
	uint32_t bufferLimit;
	uint32_t thisCycleTotalGrantSize;
    uint32_t cycleCount, cycleFromZero, onuSize, chSize, minReqSizeOfAll, maxReqSizeOfAll;
	uint32_t upLarger;
	uint32_t downLarger;
	uint32_t downBufLarger, upBufLarger;
	uint32_t sleepNoGrantCount;
	uint64_t totalError;
	uint64_t totalSVError;
	uint64_t totalError8to11;
	uint64_t totalSVError8to11;
	//int32_t onunow;
	//int32_t onuold;
	//int32_t grantUpold;
	uint32_t cycleCount8s;
	uint32_t cycleNum8to11;
	uint32_t cycleCount3s;
	uint32_t cycleCount2;



// methods
	void ProcessReport(MPCPReport * rep);
	void CheckIfAllOnuGated(uint32_t onuIdx);
	bool CheckIfCycleComplete();
	void ResetValueForNewCycle();
	void PushIntoBuffer(cMessage * msg);
	void RecordActiveChTimeLen();
	void ScheduleNewCycle();
	void GrantWindowToOnu(uint32_t idx);
	void GrantUpload(uint32_t idx);
	void GrantDownload(uint32_t idx);
	void SetOnuAsHvOrLi(uint32_t idx);
	void OpenChannel();
	void AllocChToOnu();
	void SetEndOfCycle();
	void StartTransmission();
	void MakeGateMsg(uint32_t idx, int64_t grantUpLen, uint32_t downLength);
	void PushTempToReady(uint32_t idx);
	void RecordGateInfo(uint16_t idx);
	void SetAsHeavyOnu(uint32_t idx);
	void SetAsLightOnu(uint32_t idx);
	vector<int> MakeAvailChVec();
	void AllocChToLightOnu(vector<int> order);
	uint32_t PushBufferToTemp(uint32_t idx);
	uint32_t ResumePopBuffer(uint32_t idx);
	void GoTDMA();

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void finish();
};
