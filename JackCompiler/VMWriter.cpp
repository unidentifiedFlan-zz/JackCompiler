
#include "stdafx.h"
#include "VMWriter.h"

VMWriter::VMWriter(std::string file) {
	vmStream.open(file, std::fstream::out);
}

void VMWriter::writePush(std::string segment, unsigned int index) {
	vmStream << "push " << segment << " " << index << std::endl;
}

void VMWriter::writePop(std::string segment, int index) {
	vmStream << "pop " << segment << " " << index << std::endl;
}

void VMWriter::writeArithmetic(std::string command) {
	vmStream << command << std::endl;
}

void VMWriter::writeLabel(std::string label) {
	vmStream << "label " << label << std::endl;
}

void VMWriter::writeGoto(std::string label) {
	vmStream << "goto " << label << std::endl;
}

void VMWriter::writeIf(std::string label) {
	vmStream << "if-goto " << label << std::endl;
}

void VMWriter::writeCall(std::string name, int nArgs) {
	vmStream << "call " << name << " " << nArgs << std::endl;
}

void VMWriter::writeFunction(std::string name, int nLocals) {
	vmStream << "function " << name << " " << nLocals << std::endl;
}

void VMWriter::writeReturn() {
	vmStream << "return" << std::endl;
}

VMWriter::~VMWriter() {
	vmStream.close();
}