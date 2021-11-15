/*
 * This file contains the rest of function implementations except initialize(), handleMessage() and finish()
 */
#include <omnetpp/cevent.h>
#include <omnetpp/checkandcast.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/cpacket.h>
#include <omnetpp/cqueue.h>
#include <omnetpp/csimulation.h>
#include <omnetpp/simtime.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "../common/IniParser.h"
#include "../common/TemplateCollect.h"
#include "../traffic_gen/Messages/EPON_messages_m.h"
#include "DBA.h"
#include "ONUTable.h"
#include "ONUTableEntry.h"


float RoundUpToXDecimalPlace(float f, int x) { // round float up to xth decimal place. example: f=1.234567, x=2, result=1.23
	int tmp = f * pow(10, x);
	float result = static_cast<float>(tmp) / pow(10, x);
	return result;
}

double GetFracPart(double d){ // get fractional part of a number. example: d=1.2345, return=0.2345
    double intpart;
	return modf (d, &intpart);
}

void DBA::CheckIfAllOnuGated(uint32_t idx) {
	gateSent[idx] = true;
	for (int i=0; i!=onuSize; i++) { // check if the other gate-able ONUs are all gated
		if (!gateSent[i]) {
			return;
		}
	}

    allGated = true;

	if (onutbl->GetEntry(idx)->mode != ACTIVE) { // not need to wait for REPORT if ONU is in Doze mode
		waitingReport[idx] = false;
	}
}

bool DBA::CheckIfCycleComplete(){
    if (!allGated) {
        return false;
    }
    for (uint32_t idx=0; idx!=onuSize; idx++) {
        if (!isDownEnd[idx] || waitingReport[idx]) {
            return false;
        }
    }
    return true;
}

void DBA::ResetValueForNewCycle(){
    cycleStartTime = simTime();
    allGated = false;
    chAllocatedToOnu.assign(onuSize, -1); // -1 for error checking
    thisCycleTotalGrantSize = 0;
    heavyOnu.clear();
    lightOnu.clear();
    onutbl->ResetInterOrder();
    for (uint32_t i = 0; i < chSize; i++) {
        takeUpSpaceOfCh[i] = 0;
        onuTxOrder[i].clear();
    }
    for (uint32_t i = 0; i < onuSize; i++) {
        isDownEnd[i] = false;
        waitingReport[i] = true;
        gateSent[i] = false;
        largerGrant[i] = 0;
    }
}

void DBA::ScheduleNewCycle() {
    bool allComplete = CheckIfCycleComplete();

	if (allComplete) {
	    //cout << "new cycle" << endl;
		if(simTime() < onutbl->GetEocTime() + onutbl->largestRTT){
			scheduleAt(onutbl->GetEocTime() + onutbl->largestRTT, startNewCycleEvent);
			return;
		}
		if(simTime() > 1)
			cycGapTimeAccu += simTime() - (onutbl->GetEocTime() + onutbl->largestRTT);

		lastCycleStartTime = cycleStartTime;
		ResetValueForNewCycle();
		for (uint32_t i = 0; i < onuSize; i++) {
			GrantWindowToOnu(i); // Grant up and pop down buffer
			SetOnuAsHvOrLi(i); // classify ONU as Inter-sleep(heavy loading) or Intra-sleep(light loading)
		}
		OpenChannel(); // make decision that active how many transmission channels
		AllocChToOnu(); // tell ONU to use which channel to transmit their data
		SetEndOfCycle(); // set up the end of cycle time for all channels
		StartTransmission(); // make GATE message then send out GATE and data packets
		//if (simTime() > 1) { //Origin
		if (simTime() > 3) { // Try~
			simuTimeLen = simTime() - 1;
			//cycleCount++;
			cycleCount2++;
		}

		/// Added
		if (simTime() > 8){
		    //endSimulation();
		    cycleCount++;
		}
		///

		cycleFromZero++;
		if (cycleCount == 1)
		{
		    cycleCount8s = cycleFromZero;

		    LogResults olt("simulation_Time_Is_3_and_PredictTime_Is_8");
		    olt << "cycleCount : " << cycleCount << ", cycleFromZero : " << cycleFromZero << endl;
		    olt << "Start from : " << clock() << endl;
		}

		if (cycleCount2 == 1)
		{
		    cycleCount3s = cycleFromZero;
		}

		LogResults os1("ProcessReport_CycleStartTime");
		simtime_t calCycleTime;
		calCycleTime = cycleStartTime - lastCycleStartTime;
		totalCalTime += calCycleTime;
		os1 << "cycleFromZero : "<< cycleFromZero << " cycleStartTime : " << cycleStartTime << " lastCycleStartTime : " << lastCycleStartTime << " singleCycleTime : " << calCycleTime << " Total: " << totalCalTime << endl;
	}
}

void DBA::GrantWindowToOnu(uint32_t idx) {
// for log
	totalReqCount[idx]++;
	if (onuRequestSize[idx] > maxReqSizeOfOnu[idx]) {
		maxReqSizeOfOnu[idx] = onuRequestSize[idx];
		maxReqSizeOfAll = onuRequestSize[idx]; //max request length among all ONUs
	}
	if (onuRequestSize[idx] < minReqSizeOfOnu[idx]) {
		minReqSizeOfOnu[idx] = onuRequestSize[idx];
		minReqSizeOfAll = onuRequestSize[idx]; //min request length among all ONUs
	}
	if (onuRequestSize[idx] == 0) {
		noReqCount[idx]++;
	}
	if(bufferSize[idx][0] + bufferSize[idx][1] > onuRequestSize[idx] ){
		downReqLarger = true;
		downBufLarger++;
	}
	else{
		downReqLarger = false;
		upBufLarger++;
	}
// Grant upstream
	GrantUpload(idx);
// Grant downstream
	RecordGateInfo(idx);
	GrantDownload(idx);
// Larger grant size will be the final grant size (for end of cycle calculation)
	if (grantUp[idx] > grantDown[idx]) {
		largerGrant[idx] = grantUp[idx];
		upLarger++;
	}
	else {
		largerGrant[idx] = grantDown[idx];
		downLarger++;
	}
	thisCycleTotalGrantSize += largerGrant[idx]; // for OpenChannel()
}

void DBA::GrantUpload(uint32_t idx){
    if (bufferSizeToMTW[idx] > 0.0f) {
        if (bufferSizeToMTW[idx] >= 1.0f)
            grantUp[idx] = MTW;
        else if (bufferSizeToMTW[idx] < 1.0f) {
            grantUp[idx] = bufferSizeToMTW[idx] * MTW;
        }
    }
    else if (bufferSizeToMTW[idx] == 0.0f) {
        if (onuRequestSize[idx] >= MTW) {
            grantUp[idx] = MTW;
        }
        else {
            /*
            //////////////// Added
            if ( onuRequestSize[idx] + 2000 < MTW) {
                grantUp[idx] = onuRequestSize[idx] + 2000;
            }
            else {
                grantUp[idx] = onuRequestSize[idx];
            }
            ////////////////
            */
            LogResults ddd("Show_ONU_Idx");
            ddd << "idx " << idx << endl;


            if( cycleCount > 0 )//&& idx == 16)  //when t = 8s, start LSTM.
            {
                /*
                LogResults try_o("Show_Timesteps");      //THE PLACE WE PREDICT WITH LSTM.
                int x = cycleCount % 10; // t = 2         //t = timesteps
                //timesteps[idx][x] = onuRequestSize[idx];
                if (x == 0)
                    timesteps[idx][1] = onuRequestSize[idx]; // t - 1 = 1
                else
                    timesteps[idx][x-1] = onuRequestSize[idx];
                try_o << "cycleCount : " << cycleCount << ", onuRequestSize[idx] : " << onuRequestSize[idx]<< ", idx  : " << idx  << endl;
                try_o << "Timesteps[idx][1] : " << timesteps[idx][0] << ", Timesteps[idx][2] : " << timesteps[idx][1] << ", Timesteps[idx][3] : " << timesteps[idx][2] << ", Timesteps[idx][4] : " << timesteps[idx][3] << ", Timesteps[idx][5] : " << timesteps[idx][4] <<", Timesteps[idx][6] : " << timesteps[idx][5] << ", Timesteps[idx][7] : " << timesteps[idx][6] << ", Timesteps[idx][8] : " << timesteps[idx][7] << ", Timesteps[idx][9] : " << timesteps[idx][8] << ", Timesteps[idx][10] : " << timesteps[idx][9] << endl;


                //timesteps[idx][0] = onuRequestSize[idx];  //when timesteps  = 1, only use this.

                LogResults o6("keras_model_2021_1110_In_DBA_1_ONUss");
                const auto model = fdeep::load_model("1110_8s_U12D02_1029081model.json"); //TimeStep=2 U24D02  keras_model_TimeStepIs_2_U12D02_2021_0618.json


                vector<float> vec;
                //U08D02 : 444363.0
                //U12D02 : 404807.0
                //U16D02 : 325693.0
                //U19.2D02:565807.0
                //U20D02 : 545572.0
                //U24D02 : 537837.0 [V]
                //U32D02 : 1291561.0
                //U64D02 : 2151135.0
                //U96D02 : 2484647.0
                float normalize_num = 1029081.0;  // U24D02
                for (int i = 0; i < 10 ; i++)   // TimeStep=2
                {
                    vec.push_back(timesteps[idx][i]/normalize_num);
                }

                const auto result = model.predict(
                            {fdeep::tensor(fdeep::tensor_shape(static_cast<std::size_t>(vec.size())), vec )}
                );//std::cout
                string r = fdeep::show_tensors(result);
                string res = "";
                for(int i = 0 ; i < r.length() ; i++)
                {
                    if(r[i] != '[' && r[i] != ']')
                    {
                        res = res+r[i];
                    }
                }
                float c = std::stof(res)  ;

                //o6 << "idx : " << idx << ", cycleCount : " << cycleCount << ", cycleFromZero : "<< cycleFromZero << ", show_tensors * normalize : "<<  c * normalize_num  << ", onuRequestSize[idx] : " << onuRequestSize[idx] <<   std::endl;

                if ( onuRequestSize[idx] + c * normalize_num < MTW )
                    grantUp[idx] = onuRequestSize[idx] + c * normalize_num ;
                else
                    grantUp[idx] = MTW;

                o6 << "idx : " << idx << ", cycleCount : " << cycleCount << ", cycleFromZero : "<< cycleFromZero << ", show_tensors * normalize : "<<  c * normalize_num  << ", onuRequestSize[idx] : " << onuRequestSize[idx] << ", grandUp[idx] : " << grantUp[idx] <<  std::endl;

                vec.erase(vec.begin(),vec.end());
                */
                ///*
                if ( onuRequestSize[idx] + 8000 < MTW )
                    grantUp[idx] = onuRequestSize[idx] + 8000;
                else
                    grantUp[idx] = MTW;
                //*/
                /*
                if ( onuRequestSize[idx] < MTW )
                    grantUp[idx] = onuRequestSize[idx];
                else
                    grantUp[idx] = MTW;
                */
            }
            else
            {
                grantUp[idx] = onuRequestSize[idx];
            }

        }

    }

    int32_t currentError = 0, averageTotalError = 0, SVError = 0, SD = 0, Rerror = 0, cycleNum = 0;
    int32_t currentError8to11 = 0, averageTotalError8to11 = 0, SVError8to11 = 0, SD8to11 = 0;
    //calculate totalError
    onunow = onuRequestSize[idx];
    if (simTime() < 3)
    {
        Rerror = 0;
    }
    else if (simTime() > 3)
    {
        if (cycleFromZero - cycleCount3s == 0)
        {
            cycleNum = 1;
        }
        else
        {
            cycleNum = cycleFromZero - cycleCount3s + 1;
        }
        //calculate average.
        Rerror = (onunow + grantUpold - onuold);
        currentError = fabs(grantUpold - onunow);
        totalError += currentError;
        averageTotalError = totalError / cycleNum;
        //calculate SD.
        SVError = pow(onunow - averageTotalError, 2);
        totalSVError += SVError;
        SD = sqrt(totalSVError / cycleNum);

        //calculate Error in 8~11s
        if(cycleCount > 0)
        {
            if (cycleFromZero - cycleCount8s == 0)
            {
                cycleNum8to11 = 1;
            }
            else
            {
                cycleNum8to11 = cycleFromZero - cycleCount8s + 1;
            }
            //calculate average.
            currentError8to11 = currentError;
            totalError8to11 += currentError8to11;
            averageTotalError8to11 = totalError8to11 / cycleNum8to11;
            //calculate SD.
            SVError8to11 = pow(onunow - averageTotalError8to11, 2);
            totalSVError8to11 += SVError8to11;
            SD8to11 = sqrt(totalSVError8to11 / cycleNum8to11);
        }
    }

    //---Print instantly
    LogResults op1("ONU_16_GrantUpLoadSizePerCycle");
    if( cycleCount > 0 && idx == 16 )
    {
        op1 << "cycleCount : " << cycleCount << " cycleFromZero : "<< cycleFromZero << " grantUp : " << grantUp[idx]  << endl;
    }
    LogResults op2("ONU_Except16_GrantUpLoadSizePerCycle");
    if( cycleCount > 0 && idx != 16 )
    {
        op2 << "ONU[" << idx << "]. "<< "cycleCount : "<< cycleCount << " cycleFromZero : "<< cycleFromZero << " grantUp : " << grantUp[idx] << endl;
    }
    LogResults op3("MTW_GrantUpLoadSizePerCycle");
    if( cycleCount == 1 && idx == 16 )
    {
        op3 << "cycleCount : " << cycleCount << " cycleFromZero : "<< cycleFromZero << " MTW : " << MTW  << endl;
    }
    LogResults op4("bufferSizeToMTW_test");
    op4 << "idx : " << idx << " bufferSizeToMTW : " << bufferSizeToMTW[idx] << " MTW : " << MTW  << endl;

    // counting the error of predictions.
    LogResults op5("ErrorCounter_Full_ONU16");
    if(idx == 16)
    {
        if (simTime() > 2)
            op5 << "cycleFromZero : " << cycleFromZero << " currentError : "<< currentError << " TotalError : "<< totalError << " cycleNum : "<< cycleNum << " averageTotalError_PerCycle : " << averageTotalError  << " Standard_Deviation_PerCycle : " << SD << " Rerror : " << Rerror << endl;
    }

    LogResults op6("ErrorCounter_8to11_ONU16");
    if( cycleCount > 0  && idx == 16 )
    {
        op6 << "cycleNum8to11 : " << cycleNum8to11 << " currentError8to11 : "<< currentError8to11 << " averageTotalError8to11_PerCycle : " << averageTotalError8to11  << " Standard_Deviation8to11_PerCycle : " << SD8to11 << " Rerror : " << Rerror << endl;
    }

    grantUpold = grantUp[idx];
    onuold = onuRequestSize[idx];

}

void DBA::GrantDownload(uint32_t idx){
	if (onutbl->GetEntry(idx)->mode != SLEEP) {
		grantDown[idx] = PushBufferToTemp(idx);

		LogResults op("ONU_16_GrantDownLoadSizePerCycle");
		if( cycleCount > 0 && idx == 16 )
		{
		    op << "cycleFromZero : "<< cycleFromZero << " cycleCount :  " << cycleCount <<" grantDown: " << grantDown[idx] << endl;
		}
		LogResults op2("ONU_Except16_GrantDownLoadSizePerCycle");
		if( cycleCount > 0 && idx != 16 )
		{
		    op2 << "ONU[" << idx << "]. "<< "cycleFromZero : " << cycleFromZero << " cycleCount : " << cycleCount << " grantDown: " << grantDown[idx] << endl;
		}
	}
	else {
		sleepNoGrantCount++;
		grantDown[idx] = 0;
		gateSent[idx] = true; // to ignore this SLEEP mode ONU during CheckIfAllOnuGated()
		waitingReport[idx] = false; // to ignore this SLEEP mode ONU during CheckIfCycleComplete()
		isDownEnd[idx] = true; // to ignore this SLEEP mode ONU during CheckIfCycleComplete()
	}
}

void DBA::SetOnuAsHvOrLi(uint32_t idx) { // classify ONU as heavy node or light node
    uint32_t tmpBufferSize, bufferSizeRest, grantSize;
    if(downReqLarger){
        grantSize = grantDown[idx];
        bufferSizeRest = bufferSize[idx][0] + bufferSize[idx][1]; // high priority buffer size + low priority buffer size
        tmpBufferSize = bufferSizeRest + grantDown[idx]; // buffer size before the grant
    }
    else{
        grantSize = grantUp[idx];
        tmpBufferSize = onuRequestSize[idx];

        if(tmpBufferSize <= grantUp[idx])
            bufferSizeRest = 0;
        else
            bufferSizeRest = tmpBufferSize - grantUp[idx];
    }

    if (tmpBufferSize >= interThres * MTW && grantSize != 0) { // set as heavy node
        SetAsHeavyOnu(idx);
        float restOfBuffer = bufferSizeRest / static_cast<float>(MTW);
        bufferSizeToMTW[idx] = RoundUpToXDecimalPlace(restOfBuffer, interThresDecimalPlace);
    }
    else { // set as light node
        SetAsLightOnu(idx);
        bufferSizeToMTW[idx] = 0.0f;
    }
}

void DBA::SetAsHeavyOnu(uint32_t idx) {
	onutbl->GetEntry(idx)->heavyLoading = true;
	heavyOnu.push_back(idx);
}

void DBA::SetAsLightOnu(uint32_t idx) {
	onutbl->GetEntry(idx)->heavyLoading = false;
	lightOnu.push_back(idx);
}

void DBA::OpenChannel() { // decide how many channel need to be active depend on the Granted data size
	prevActiveCh = activeChannel;
	double avgSpaceOfACh = static_cast<double>(thisCycleTotalGrantSize) / maxSizeOfACycle;
	int floorCh = floor(avgSpaceOfACh);
	int ceilCh = ceil(avgSpaceOfACh);
	double fracPart = GetFracPart(avgSpaceOfACh);
	if(fracPart < chOpenThres){
		activeChannel = floorCh;
		for (uint32_t i = 0; i < chSize; i++)
			remainSpaceOfCh[i] = ceil(static_cast<double>(thisCycleTotalGrantSize) / activeChannel);
	}
	else{
		activeChannel = ceilCh;
		for (uint32_t i = 0; i < chSize; i++)
			remainSpaceOfCh[i] = maxSizeOfACycle;
	}

// TDMA
	if(enableTDMAmode){
		if(activeChannel == chSize){
			GoTDMA();
		}
	}
	RecordActiveChTimeLen();
}

bool CompOnuSize(const Onu& a, const Onu& b) {
	return a.dataSize > b.dataSize;
}

void DBA::AllocChToOnu() { // allocate channel to all ONU
// Allocate channel to heavy node
	if (heavyOnu.size() > 1) {
		vector<InterSleepOnu> tmpInterOrder;
		for (int i = 0; i < heavyOnu.size(); i++) {
			for (int j = 0; j < activeChannel; j++){
				if (remainSpaceOfCh[j] >= largerGrant[heavyOnu[i]]) {
					chAllocatedToOnu[heavyOnu[i]] = j;
					remainSpaceOfCh[j] -= largerGrant[heavyOnu[i]];
					takeUpSpaceOfCh[j] += largerGrant[heavyOnu[i]];
					if (bufferSizeToMTW[heavyOnu[i]] > 0) { // record transmission order&size of Inter-sleep ONU
						InterSleepOnu interOnu;
						interOnu.index = heavyOnu[i];
						if (bufferSizeToMTW[heavyOnu[i]] >= 1)
							interOnu.dataSize = 1;
						else
							interOnu.dataSize = bufferSizeToMTW[heavyOnu[i]];
						tmpInterOrder.push_back(interOnu);
					}
					if (remainSpaceOfCh[j] < MTW || i == heavyOnu.size() - 1) { // last one in this channel  OR  last one of Heavy node
						onutbl->interOnuOrder.push_back(tmpInterOrder); // push InterOrder of a channel into collection vector
						tmpInterOrder.clear();
					}
					onuTxOrder[j].push_back(heavyOnu[i]);
					break;
				}
			}
			if (chAllocatedToOnu[heavyOnu[i]] == -1) { // a checkpoint, sometimes channel allocation just not work
				for (int j = 0; j < activeChannel; j++)
					cout << "ch[" << j << "] remain : " << remainSpaceOfCh[j] << " bytes." << endl;
				cout << "Cycle : " << cycleFromZero << endl;
				cout << "Grant : " << largerGrant[heavyOnu[i]] << endl;
				cout << "No channel was allocated to a heavy load ONU " << heavyOnu[i] << endl;
				endSimulation();
			}
		}

// Allocate channel to light ONU ----------
		AllocChToLightOnu(lightOnu);
	}
	else {
		vector<int> allOnu;
		for(uint32_t i = 0; i < onuSize; i++)
			allOnu.push_back(i);
		AllocChToLightOnu(allOnu); // every ONU is light loading
	}
}

vector<int> DBA::MakeAvailChVec() { // return a vector that contains indices of channel currently available
    vector<int> availableCh;
    for (int i = 0; i < activeChannel; i++) {
        availableCh.push_back(i);
    }
    return availableCh;
}

void DBA::AllocChToLightOnu(vector<int> onuRange){ // channel allocation method for light node and the case when there's no heavy node
	vector<int> availableCh = MakeAvailChVec();
	vector<Onu> onuCantFitIn;
	for (uint32_t i = 0; i < onuRange.size(); i++) { // pick random channel and see if remain size of it still enough, if not pick a random channel again (minus the not enough one)
		int ch = availableCh[rand() % availableCh.size()];
		while (remainSpaceOfCh[ch] - largerGrant[onuRange[i]] < 0) {
			VecRemoveVal(availableCh, ch); // remove the channel that space not enough from random list
			if(availableCh.size() == 0){ // the case channel space is not enough (the fragment is a little bigger than channel space)
				ch = -1;
				Onu onu;
				onu.index = onuRange[i];
				onu.dataSize = largerGrant[onuRange[i]];
				onuCantFitIn.push_back(onu);
				break;
			}
			ch = availableCh[rand() % availableCh.size()];
		}
		if(ch!=-1){
			remainSpaceOfCh[ch] -= largerGrant[onuRange[i]];
			chAllocatedToOnu[onuRange[i]] = ch;
			takeUpSpaceOfCh[ch] += largerGrant[onuRange[i]];
			onuTxOrder[ch].push_back(onuRange[i]);
		}
		availableCh.clear();
		availableCh = MakeAvailChVec();
	}

	if(onuCantFitIn.size() != 0){ // pick a least allocated data size channel (cause extend cycle length a little bit)
		sort(onuCantFitIn.begin(), onuCantFitIn.end(), CompOnuSize);
		for (int i = 0; i < onuCantFitIn.size(); i++){
			int leastUsedChSize = 2 * maxSizeOfACycle; // it just a big number, we use two times cycle length
			int leastUsedCh;
			for(int j=0; j<activeChannel; j++){
				int tmpLeng = takeUpSpaceOfCh[j] + largerGrant[onuCantFitIn[i].index];
				if(tmpLeng < leastUsedChSize){
					leastUsedChSize = tmpLeng;
					leastUsedCh = j;
				}
			}
			takeUpSpaceOfCh[leastUsedCh] += largerGrant[onuCantFitIn[i].index];
			chAllocatedToOnu[onuCantFitIn[i].index] = leastUsedCh;
			onuTxOrder[leastUsedCh].push_back(onuCantFitIn[i].index);
		}
	}
}

void DBA::SetEndOfCycle() {
	simtime_t largerEocAmongAllChLen;
	for (int i = 0; i < activeChannel; i++) {
		simtime_t tmpEoc = takeUpSpaceOfCh[i] * 8 * downRateRec;
		if (tmpEoc > largerEocAmongAllChLen)
			largerEocAmongAllChLen = tmpEoc;
	}
	largerEocAmongAllChLen = simTime() + largerEocAmongAllChLen;
	onutbl->SetEocTime(largerEocAmongAllChLen);
	if(simTime() > 1)
		cycleTimeAccu += largerEocAmongAllChLen - cycleStartTime;
}

void DBA::StartTransmission() {
	for (int i = 0; i < activeChannel; i++) {
		for (int j = 0; j < onuTxOrder[i].size(); j++) {
			int idx = onuTxOrder[i][j];
			MakeGateMsg(idx, grantUp[idx], grantDown[idx]);
			LogResults oawo("StartTransmission");
			oawo << "cycleFromZero : " << cycleFromZero << ", idx : " << idx << endl;
			PushTempToReady(idx);
		}
	}
	for (int i = 0; i < activeChannel; i++)
		scheduleAt(simTime(), sendCHDownstreamEvent[i]);
}

void DBA::MakeGateMsg(uint32_t idx, uint32_t grantUpLen, uint32_t grantDownLen) {
	MPCPGate * gt = new MPCPGate("MPCPGate", MPCP_TYPE);
	gt->setOnuIndex(idx);
	gt->setOpcode(MPCP_GATE);
	gt->setDestAddr(onutbl->GetEntry(idx)->LLID);
	gt->setUpLength(grantUpLen);
	gt->setDownLength(grantDownLen);
	gt->setByteLength(64);
	gt->setTimestamp();
	gt->setCycleZ(cycleFromZero);
	gt->setCycle(cycleCount);
	gt->setQueueLength(0, buffer[idx][0].getLength());
	gt->setQueueLength(1, buffer[idx][1].getLength());
	gt->setRecordedEnergyMode(onutbl->GetEntry(idx)->mode);

	if(cycleCount > 0 && (idx == 15))
	{
	    LogResults oawo("MakeGateMsg_ONU15_grantUpLen_grantDownLen");
	    oawo << "ONU[" << idx << "]" << " cycleCount : " << cycleCount << ", cycleFromZero : " << cycleFromZero << "\t" << "GrantUpLen : " << grantUpLen << "\t GrantDownLen : " <<  grantDownLen << endl;
	}
	if(cycleCount > 0 && (idx == 16))
	{
	    LogResults owo("MakeGateMsg_ONU16_grantUpLen_grantDownLen");
	    owo << "ONU[" << idx << "]" << " cycleCount : " << cycleCount << ", cycleFromZero : " << cycleFromZero  << "\t" << "GrantUpLen : " << grantUpLen << "\t GrantDownLen : " <<  grantDownLen << endl;
	}

	if (chAllocatedToOnu[idx] < chSize && chAllocatedToOnu[idx] >= 0) {
		simtime_t eocPlusPropa = onutbl->GetEocTime() + onutbl->GetEntry(idx)->RTT;
		gt->setEndOfCycleTime(eocPlusPropa); // end of cycle is for Intra-sleep ONU
		if (bufferSizeToMTW[idx] > 0 && onutbl->interOnuOrder.size() > 0) { // set the info for Inter-sleep
			simtime_t interTimeLen;
			float heavyOnuTimePos = 0;
			int ch = chAllocatedToOnu[idx];
			vector<vector<InterSleepOnu>>& interOnuOrder = onutbl->interOnuOrder;
			for (int i = 0; i < interOnuOrder[ch].size(); i++) {
				if (interOnuOrder[ch][i].index == idx)
					break;
				heavyOnuTimePos += interOnuOrder[ch][i].dataSize; // sum up positions of X * MTW data size from previous ONUs
			}
			interTimeLen = heavyOnuTimePos * MTW * 8 * downRateRec;
			gt->setInterSleep(true);
			gt->setEndOfInterSleep(eocPlusPropa + interTimeLen);
		}
		else {
			gt->setInterSleep(false);
		}
	}
	else {
		cout << "No channel was allocated to ONU " << idx << endl;
		endSimulation();
	}

// Set transmit channel
	int channel = chAllocatedToOnu[idx];
	gt->setTransmitChannel(channel);
	onutbl->GetEntry(idx)->currentCh = channel;

	grantDownLen == 0 ? gt->setLastPkt(true) : gt->setLastPkt(false);

// After the Gate message get ready, push Gate message into front of tempQueue
	if(!tempQueue[idx].isEmpty()) // sometimes there's no data need to downstream thus there's no front() to call. so check if it's empty first
	    tempQueue[idx].insertBefore(tempQueue[idx].front(), gt);
	else
	    tempQueue[idx].insert(gt);
}

void DBA::PushTempToReady(uint32_t idx) {
	int channel = onutbl->GetEntry(idx)->currentCh;
	int tempQueueSize = tempQueue[idx].getLength(); // getLength() function would change once you pop(), so you need to save value of it first
	for(int i=0; i<tempQueueSize; i++){
	    readyQueue[channel].insert(tempQueue[idx].pop());
	}
}

uint32_t DBA::PushBufferToTemp(uint32_t idx) {
	uint32_t pri = 0, nextPktSize = 0, totalPktSize = 0;
	if (buffer[idx][0].isEmpty() && buffer[idx][1].isEmpty())
		return 0;
	while (pri < 2) {
		if (!buffer[idx][pri].isEmpty()) {
			nextPktSize = check_and_cast<MyPacket*>(buffer[idx][pri].front())->getByteLength();
			if ((bufferSizeToMTW[idx] >= 1 && totalPktSize + nextPktSize <= MTW) || (bufferSizeToMTW[idx] < 1 && bufferSizeToMTW[idx] > 0 && totalPktSize + nextPktSize <= bufferSizeToMTW[idx] * MTW)
					|| (bufferSizeToMTW[idx] == 0 && totalPktSize + nextPktSize <= MTW)) {
				totalPktSize += nextPktSize;
				if(nextPktSize > bufferSize[idx][pri]) // another checkpoint, unsigned integer reason
					endSimulation();
				bufferSize[idx][pri] -= nextPktSize;
				MyPacket * pkt = check_and_cast<MyPacket*>(buffer[idx][pri].pop());
				pkt->setOnuIndex(idx);
				pkt->setCycleZ(cycleFromZero);
				pkt->setCycle(cycleCount);
				tempQueue[idx].insert(pkt);
			}
			else if (bufferSizeToMTW[idx] < 0) {
				cout << "Inter Quota is lower than 0." << endl;
				endSimulation();
			}
			else
				break;
		}
		else
			pri++;
	}
	check_and_cast<MyPacket*>(tempQueue[idx].back())->setLastPkt(true);
	return totalPktSize;
}

uint32_t DBA::ResumePopBuffer(uint32_t idx){ // TDMA mode resume pop out the buffer up to 1 MTW
	uint32_t pri = 0, nextPktSize = 0, totalPktSize = grantDown[idx];
	if (buffer[idx][0].isEmpty() && buffer[idx][1].isEmpty())
		return totalPktSize;
	if(tempQueue[idx].getLength() > 0)
		check_and_cast<MyPacket*>(tempQueue[idx].back())->setLastPkt(false);
	while (pri < 2) {
		if (!buffer[idx][pri].isEmpty()) {
			nextPktSize = check_and_cast<MyPacket*>(buffer[idx][pri].front())->getByteLength();
			if (totalPktSize + nextPktSize <= MTW) {
				totalPktSize += nextPktSize;
				if(nextPktSize > bufferSize[idx][pri])
					endSimulation();
				bufferSize[idx][pri] -= nextPktSize;
				MyPacket * pkt = check_and_cast<MyPacket*>(buffer[idx][pri].pop());
				pkt->setOnuIndex(idx);
				pkt->setCycleZ(cycleFromZero);
				pkt->setCycle(cycleCount);
				tempQueue[idx].insert(pkt);
			}
			else if (bufferSizeToMTW[idx] < 0) {
				cout << "Inter Quota is lower than 0." << endl;
				endSimulation();
			}
			else
				break;
		}
		else
			pri++;
	}
	check_and_cast<MyPacket*>(tempQueue[idx].back())->setLastPkt(true);
	return totalPktSize;
}

void DBA::PushIntoBuffer(cMessage* msg) { // push downstream packets into buffer queue
	MyPacket * pkt = check_and_cast<MyPacket*>(msg);
	uint16_t dest = pkt->getDestAddr() - 2;
	uint16_t pri = pkt->getPriority();
	int64_t pktSize = pkt->getByteLength();
	pkt->setLastPkt(false);
	if (bufferSize[dest][pri] + pktSize <= bufferLimit) {
		buffer[dest][pri].insert(msg);
		bufferSize[dest][pri] += pktSize;
		if (simTime() > 1)
			accumulateDownBufferBytes[dest] += pktSize;
	}
	else {
		pktDropped[dest][pri]++;
		delete pkt;
	}
}

void DBA::GoTDMA(){ // grant every ONU 1 MTW and 1 InterQuota then force them enter Inter-sleep
	heavyOnu.clear();
	lightOnu.clear();
	for (uint32_t idx = 0; idx < onuSize; idx++) {
		grantUp[idx] = MTW;
		grantDown[idx] = ResumePopBuffer(idx);
		largerGrant[idx] = MTW;
		SetAsHeavyOnu(idx);
		bufferSizeToMTW[idx] = 1;
	}
}

void DBA::RecordGateInfo(uint16_t idx) {
	uint32_t pktSizeByte = (bufferSize[idx][0] + bufferSize[idx][1]);
	gateCountOfOnu[idx]++; // add number of times of gate of each ONU
	totalDownBufferSize[idx] += pktSizeByte / pow(10, 6);
}

void DBA::RecordActiveChTimeLen() { // record time length of each amounts of active channel
	if (simTime() > 1) {
		useChAmountTimeLen[prevActiveCh - 1] += simTime() - lastRecordChTime;
	}
	lastRecordChTime = simTime();
}

void DBA::ProcessReport(MPCPReport * rep) { // receive REPORT message and reset the waiting REPORT flag for xth ONU
    uint16_t onuIdx = rep->getSrcAddr() - 2;
    onuRequestSize[onuIdx] = rep->getRequestSize();

    if (waitingReport[onuIdx]) {
        waitingReport[onuIdx] = false;
        if (!startNewCycleEvent->isScheduled()) // 'try' to schedule a new cycle event
            scheduleAt(simTime(), startNewCycleEvent);
    }

    if(cycleCount > 0)
    {
        if(onuIdx == 5)
        {
                LogResults opr("ProcessReport_ONU6_RequestSize_OLT_DBA");
                opr << "cycleCount : " << cycleCount << " cycleFromZero : "<< cycleFromZero << " onuRequestSize : " << onuRequestSize[onuIdx] << endl;
        }
        if(onuIdx == 9)
        {
        LogResults oap("ProcessReport_ONU10_RequestSize_OLT_DBA");
        oap << "cycleCount : " << cycleCount << " cycleFromZero : "<< cycleFromZero << " onuRequestSize : " << onuRequestSize[onuIdx] << endl;
        }
        if(onuIdx == 19)
        {
                LogResults ops("ProcessReport_ONU20_RequestSize_OLT_DBA");
                ops << "cycleCount : " << cycleCount << " cycleFromZero : "<< cycleFromZero << " onuRequestSize : " << onuRequestSize[onuIdx] << endl;
        }
        if(onuIdx == 29)
        {
                LogResults opw("ProcessReport_ONU30_RequestSize_OLT_DBA");
                opw << "cycleCount : " << cycleCount << " cycleFromZero : " << cycleFromZero << " onuRequestSize : " << onuRequestSize[onuIdx] << endl;
        }
        if(onuIdx == 24)
        {
                LogResults op24("ProcessReport_ONU25_RequestSize_OLT_DBA");
                op24 << "cycleCount : " << cycleCount << " cycleFromZero : " << cycleFromZero << " onuRequestSize : " << onuRequestSize[onuIdx] << endl;
        }
        if(onuIdx == 0)
        {
                LogResults op1("ProcessReport_ONU1_RequestSize_OLT_DBA");
                op1 << "cycleCount : " << cycleCount << " cycleFromZero : "<< cycleFromZero << " onuRequestSize : " << onuRequestSize[onuIdx] << endl;
        }
        if(onuIdx == 31)
        {
                LogResults op32("ProcessReport_ONU32_RequestSize_OLT_DBA");
                op32 << "cycleCount : " << cycleCount << " cycleFromZero : "<< cycleFromZero << " onuRequestSize : " << onuRequestSize[onuIdx] << endl;
        }
        if(onuIdx == 16)
        {
            LogResults os("ProcessReport_ONU16_RequestSize_OLT_DBA");
            os << "cycleCount : " << cycleCount << " cycleFromZero : "<< cycleFromZero << " onuRequestSize : " << onuRequestSize[onuIdx] << endl;
        }
        LogResults ooo("ProcessReport_32_ONUs_RequestSize_OLT_DBA");
        ooo << "ONU[" << onuIdx << "]. " << "cycleCount : " << cycleCount << ", cycleFromZero : " << cycleFromZero << " onuRequestSize : " << onuRequestSize[onuIdx] << endl;

    }
}
