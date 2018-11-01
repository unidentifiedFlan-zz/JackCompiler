#pragma once

#include "stdafx.h"

class VMWriter {

	std::fstream vmStream;

public:

	VMWriter(std::string file);

	void writePush(std::string segment, unsigned int index);
	void writePop(std::string segment, int index);
	void writeArithmetic(std::string command);
	void writeLabel(std::string label);
	void writeGoto(std::string label);
	void writeIf(std::string label);
	void writeCall(std::string name, int nArgs);
	void writeFunction(std::string name, int nLocals);
	void writeReturn();

	~VMWriter();
};