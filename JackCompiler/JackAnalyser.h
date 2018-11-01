#pragma once

#include "stdafx.h"


class JackAnalyser
{
	std::vector<std::string> fileList;
	std::vector<std::string>::iterator fileIterator;

public:
	JackAnalyser(std::string file);
	void findFiles(std::string &file);
	bool moreFiles();
	std::string getCurrentVMFile();
};