#pragma once

#include "stdafx.h"

class symbolDefinition {

	std::string kind;
	std::string type;
	int index;

public:
	std::string getKind() { return kind; }
	std::string getType() { return type; }
	int getIndex() { return index; }
	void setKind(std::string newKind) { kind = newKind; }
	void setType(std::string newType) { type = newType; }
	void setIndex(int newIndex) { index = newIndex; }
};