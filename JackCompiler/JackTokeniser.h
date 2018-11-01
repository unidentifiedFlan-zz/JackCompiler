#pragma once

#include "stdafx.h"
#include "symbolDefinition.h"

class JackTokeniser
{
	std::fstream fileStream;
	std::string currentLine;
	std::string cToken;
	std::string tokType;
	std::map<std::string, symbolDefinition> identifier;

public:
	void newFile(std::string file);
	bool hasMoreTokens();
	bool isASymbol();
	bool isAKeyword();
	bool isADigit(char &c);
	void getStringConst();
	void getIntConst();
	void getNextLine();
	void findEndOfLine();
	void advance();
	void editLine(std::string::iterator it);
	std::string tokenType();
	std::string getToken();
	void closeFile();
};
