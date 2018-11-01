#pragma once

#include "stdafx.h"
#include "symbolDefinition.h"

class symbolTable {

	std::map<std::string, symbolDefinition> definitions;

public:

	void startSubroutine(std::string className, std::string subroutineKind);
	void define(std::string name, std::string type, std::string kind);
	int varCount(std::string kind);
	std::string kindOf(std::string name);
	std::string typeOf(std::string name);
	int indexOf(std::string name);
};

