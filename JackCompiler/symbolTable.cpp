
#include "stdafx.h"
#include "symbolTable.h"
#include "symbolDefinition.h"

void symbolTable::startSubroutine(std::string className, std::string subroutineKind) {

	definitions.clear();

	if(subroutineKind == "method")
		define("this", className, "argument");
}

void symbolTable::define(std::string name, std::string type, std::string kind) {

	symbolDefinition nameDefinition;

	nameDefinition.setType(type);
	nameDefinition.setKind(kind);
	nameDefinition.setIndex(varCount(kind));

	definitions.emplace(name, nameDefinition);

}

int symbolTable::varCount(std::string kind) {

	int count = 0;

	for (auto it = definitions.begin(); it != definitions.end(); it++) {
		if (it->second.getKind() == kind)
			count++;
	}

	return count;
}


std::string symbolTable::kindOf(std::string name) {

	if (definitions.find(name) != definitions.end())
		return definitions[name].getKind();
	else
		return "none";
}

std::string symbolTable::typeOf(std::string name) {

	if (definitions.find(name) != definitions.end())
		return definitions[name].getType();
	else
		return "none";
}

int symbolTable::indexOf(std::string name) {

	if (definitions.find(name) != definitions.end())
		return definitions[name].getIndex();

	else
		return -1;
}
