#pragma once

#include "stdafx.h"
#include <windows.h>

class Utils
{
public:
	Utils();
	void getFilesList(std::string filePath, std::string extension, std::vector<std::string> & returnFileName);
	bool try_stoi(int & i, const std::string & s);
	~Utils();
};

