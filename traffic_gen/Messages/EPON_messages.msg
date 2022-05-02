cplusplus {{
#include <inttypes.h> 
}}

packet MyPacket
{
	uint16_t SrcAddr;
    uint16_t DestAddr;
    uint16_t Priority;
    simtime_t txEnd;
    simtime_t txStart;

	bool lastPkt=false;
	uint32_t cycle;
	uint32_t cycleZ;
	uint16_t onuIndex;
}

packet MPCP extends MyPacket
{
    uint16_t opcode;
    uint32_t ts;
}

packet RTTReg extends MPCP 
{
    simtime_t Rtt;
//    int ByteLength; // eth_header(14) + payload(1500) + FCS(4) = 1518  
}

packet MPCPAutoDiscovery extends MPCP 
{
    // use for rtt test
    simtime_t rtt;
}

packet MPCPGate extends MPCP 
{
    simtime_t startTime;
    uint32_t upLength;
    uint32_t downLength;
    uint32_t queueLength[2];
    
    simtime_t pwsTime[2];
    uint16_t pwsMode[2];
        
    bool interSleep = false;
    simtime_t endOfCycleTime;
    simtime_t endOfInterSleep;
    uint16_t transmitChannel;
    uint16_t onuIndex;
    bool holded = false;
    uint16_t recordedEnergyMode;
}

packet MPCPReport extends MPCP 
{
	uint16_t curMode;
    uint32_t requestSize;
}

packet TimeOutMsg
{
	uint16_t onuIdx;    
}