#include "IniParser.h"


IniParser& IniParser::GetInstance() {
    static IniParser instance;
    return instance;
}

void IniParser::Parse(){
	cModule* epon = getSimulation()->getModuleByPath("EPON");

	sizeOfONU = epon->par("sizeOfONU").intValue();
	sizeOfCh = epon->par("sizeOfCh").intValue();
	pktBufferLimit = epon->par("pktBufferLimit").intValue();
	downDataRate = epon->par("downDataRate").intValue();
	upDataRate = epon->par("upDataRate").intValue();
	distance = epon->par("distance").doubleValue();
	tuningTime = epon->par("tuningTime").doubleValue();
	interSleepThres = epon->par("interSleepThres").doubleValue();
	chOpenThres = epon->par("channelOpenThres").doubleValue();
	maxMpcpTimer = epon->par("maxMpcpTimer").doubleValue();
	cycleTimeLen = epon->par("cycleTimeLen").intValue();
	enableTDMAmode = epon->par("enableTDMAmode").boolValue();
	upOfferedLoad = epon->par("upOfferedLoad").doubleValue();
	downOfferedLoad = epon->par("downOfferedLoad").doubleValue();
	std::cout << downOfferedLoad << std::endl;
	upHighRatio = epon->par("upHighRatio").doubleValue();
	downHighRatio = epon->par("downHighRatio").doubleValue();
	upSendRate = epon->par("upSendRate").doubleValue();
	downSendRate = epon->par("downSendRate").doubleValue();
	paretoRate = epon->par("paretoRate").doubleValue();
    paretoAlphaOn = epon->par("paretoAlphaOn").doubleValue();
    paretoAlphaOff = epon->par("paretoAlphaOff").doubleValue();
    paretoBetaOn = epon->par("paretoBetaOn").doubleValue();

	// do some calculations after parse
	onuAmountPerCh = sizeOfONU / sizeOfCh;
	std::cout << "ONU slots in a channel : " << onuAmountPerCh << std::endl;
	downDataRateRec = static_cast<double>(1) / (downDataRate * pow(10, 9));
	upDataRateRec = static_cast<double>(1) / (upDataRate * pow(10, 9));
	mtw = downDataRate * pow(10, 9) / onuAmountPerCh / 8 * cycleTimeLen * pow(10, -3) / 1000;
	mtw *= 1000;
	std::cout << "MTW : " << mtw << "bytes" << std::endl;
}

void IniParser::ShowParsed(){
}

