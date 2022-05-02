#include "../common/LogResults.h"

#ifdef WIN32
// Source : https://gist.github.com/danzek/d7192d250c951804dec05125f5223a30
void createDirectoryRecursively(const std::wstring &directory) {
	static const std::wstring separators(L"\\/");

	// If the specified directory name doesn't exist, do our thing
	DWORD fileAttributes = ::GetFileAttributesW(directory.c_str());
	if (fileAttributes == INVALID_FILE_ATTRIBUTES) {

		// Recursively do it all again for the parent directory, if any
		std::size_t slashIndex = directory.find_last_of(separators);
		if (slashIndex != std::string::npos) {
			createDirectoryRecursively(directory.substr(0, slashIndex));
		}

		// Create the last directory on the path (the recursive calls will have taken
		// care of the parent directories by now)
		BOOL result = ::CreateDirectoryW(directory.c_str(), nullptr);
		if (result == FALSE) {
			throw std::runtime_error("Could not create directory");
		}

	}
	else { // Specified directory name already exists as a file or directory

		bool isDirectoryOrJunction = ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) || ((fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);

		if (!isDirectoryOrJunction) {
			throw std::runtime_error("Could not create directory because a file with the same name exists");
		}

	}
}
#endif

std::ofstream& LogResults::operator<<(const std::string& ss)
{
	if (this->content.is_open())
	{
		this->content << ss;
		return this->content;
	}
}

std::ofstream& LogResults::operator<<(const int& ii)
{
    if (this->content.is_open()){
        this->content << ii;
        return this->content;
    }
}

std::ofstream& LogResults::operator<<(const float& ff)
{
    if (this->content.is_open()){
        this->content << ff;
        return this->content;
    }
}

std::ofstream& LogResults::operator<<(const double& dd)
{
    if (this->content.is_open()){
        this->content << dd;
        return this->content;
    }
}

std::ofstream& LogResults::operator<<(const uint32_t& u32){
    if (this->content.is_open()){
        this->content << u32;
        return this->content;
    }
}

std::ofstream& LogResults::operator<<(const uint16_t& u16){
    if (this->content.is_open()){
        this->content << u16;
        return this->content;
    }
}

void LogResults::CreateFolder(std::string folderPath)
{
	DIR* openDir = opendir(folderPath.c_str());
	if (openDir) // if dir exist dont create
	{
		closedir(openDir);
	}
#ifdef WIN32
	else{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring ws = converter.from_bytes(folderPath);
		createDirectoryRecursively(ws);
	}
#endif
#ifdef __linux__
	else
	{ // if dir not exist we create
		const int dir_err = mkdir(folderPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (-1 == dir_err)
		{
			std::cout << "Error creating directory!" << std::endl;
			exit(1);
		}
	}
#endif
}

LogResults::LogResults(std::string lN) :
		logName(lN)
{
	cModule *epon = getSimulation()->getModuleByPath("EPON");
	extFolderName = epon->par("extFolderName").stringValue();
	bool downFolder = epon->par("downFolder").boolValue();

#ifdef WIN32
	dirPath = epon->par("winSavePath").stringValue();
    if(downFolder){
        cModule *trafficGen = getSimulation()->getModuleByPath("EPON.trafficGen[0]"); //EPON.trafficGen[0].offered_load
        int offerLoad = trafficGen->par("offered_load").doubleValue() * 1280;
        std::string downFolderName = "\\" + std::to_string(offerLoad);
        dirPath = dirPath + extFolderName + downFolderName;
    }
    else
        dirPath = dirPath + extFolderName;
	CreateFolder(dirPath);
	dirPath += "\\";

#endif

#ifdef __linux__
	dirPath = epon->par("linuxSavePath").stringValue();
	dirPath = dirPath + extFolderName;
	CreateFolder(dirPath);
	dirPath += "/";
#endif

	if(lN != "TESTLOGFUNC"){
		path << dirPath << logName << ".txt";
		content.open(path.str().c_str(), std::ios::out | std::ios::app);
	}
}

LogResults::~LogResults()
{
	if (content.is_open())
		content.close();
}
