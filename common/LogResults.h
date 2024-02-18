#pragma once
#include <omnetpp.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <dirent.h>
#include <string>

#ifdef WIN32
#include <Windows.h>
#include <locale>
#include <codecvt>
#endif

using namespace omnetpp;

class LogResults
{
	public:
		LogResults(std::string lN);
		~LogResults();
		void CreateFolder(std::string folderPath);
		std::ofstream& operator<<(const std::string& ss);
		std::ofstream& operator<<(const int& ii);
		std::ofstream& operator<<(const float& ff);
		std::ofstream& operator<<(const double& dd);
		std::ofstream& operator<<(const uint32_t& u32);
		std::ofstream& operator<<(const uint16_t& u16);
	private:
		std::string extFolderName;
		std::string dirPath;
		std::string logName;
		std::stringstream path;
		std::ofstream content;
};
