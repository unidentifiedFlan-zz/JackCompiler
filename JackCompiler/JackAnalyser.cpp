#include "stdafx.h"
#include "JackAnalyser.h"
#include "JackTokeniser.h"
#include "CompilationEngine.h"
#include "Utils.h"
#include "VMWriter.h"

JackAnalyser::JackAnalyser(std::string file) {


	findFiles(file);

	fileIterator = fileList.begin();

	//LOOP over files to be parsed using vector of files found by Windows, fileList...
	JackTokeniser _jackTokeniser;
	compilationEngine _compilationEngine(_jackTokeniser);

	do {
		_jackTokeniser.newFile(file + fileList[fileIterator - fileList.begin()]);
		_compilationEngine.compileFile(file + getCurrentVMFile()); //Remove JackToeniser from paramters
		_jackTokeniser.closeFile();

	} while (moreFiles());

	return;
}

void JackAnalyser::findFiles(std::string &file) {
	Utils Utils;

	//Obtain relevant files
	if (file.find(".jack") == std::string::npos) {

		std::string inputFolderPath = file.append("\\");
		Utils.getFilesList(inputFolderPath, "*.jack*", fileList);
	}
	else
		fileList.push_back(file);
}

bool JackAnalyser::moreFiles() {

	fileIterator++;

	if (fileIterator != fileList.end())
		return 1;

	return 0;
}



std::string JackAnalyser::getCurrentVMFile() {

	std::string file = fileList[fileIterator - fileList.begin()];

	// Remove ".jack" from file name
	file.erase(file.end() - 5, file.end());

	file.append(".vm");

	return file;
}
