#include "stdafx.h"
#include "Utils.h"


Utils::Utils()
{
}

void Utils::getFilesList(std::string filePath, std::string extension, std::vector<std::string> & returnFileName)
{
	WIN32_FIND_DATAA fileInfo;
	HANDLE hFind;
	std::string  fullPath = filePath + extension;
	hFind = FindFirstFileA(fullPath.c_str(), &fileInfo);
	if (hFind != INVALID_HANDLE_VALUE) {
		returnFileName.push_back(fileInfo.cFileName);
		while (FindNextFileA(hFind, &fileInfo) != 0) {
			returnFileName.push_back(fileInfo.cFileName);
		}
	}
	else {
		throw std::exception("File not found");
	}
}



//Returns string as integer
bool Utils::try_stoi(int & i, const std::string & s) {
	try {
		size_t pos;
		i = stoi(s, &pos);
		return pos == s.size();
	}
	catch (const std::invalid_argument&) {
		return false;
	}
}



Utils::~Utils()
{
}
