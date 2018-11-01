//class for identifying tokens in .jack file 
//

#include "stdafx.h"
#include "JackTokeniser.h"


void JackTokeniser::newFile(std::string file) {
	fileStream.open(file, std::fstream::in);
}


bool JackTokeniser::hasMoreTokens() {
	
	//Returns true if we have reached the end of the line (/n) but not the end of the file

	fileStream >> std::ws;

	while (fileStream.peek() == '/' || fileStream.peek() == '\n' || fileStream.peek() == '*') { 

		fileStream.ignore(256, '\n'); 
		fileStream >> std::ws;

	}

	if (fileStream.eof()) 
		return false;

	return true;
}

bool JackTokeniser::isASymbol() {
	return (currentLine[0] == '{' || currentLine[0] == '}' || currentLine[0] == '(' || currentLine[0] == ')' ||
		currentLine[0] == '[' || currentLine[0] == ']' || currentLine[0] == '.' || currentLine[0] == ',' ||
		currentLine[0] == ';' || currentLine[0] == '+' || currentLine[0] == '-' || currentLine[0] == '/' ||
		currentLine[0] == '&' || currentLine[0] == '|' || currentLine[0] == '=' || currentLine[0] == '~' ||
		currentLine[0] == '<' || currentLine[0] == '>');
}


bool JackTokeniser::isAKeyword() {
	return (cToken == "class" || cToken == "method" || cToken == "function" || cToken == "constructor" || cToken == "int" ||
		cToken == "boolean" || cToken == "char" || cToken == "void" || cToken == "var" || cToken == "static" ||
		cToken == "field" || cToken == "let" || cToken == "do" || cToken == "if" || cToken == "else" || cToken == "while" ||
		cToken == "return" || cToken == "true" || cToken == "false" || cToken == "null" || cToken == "this");
}

void JackTokeniser::getNextLine() {
	while (currentLine.size() == 0) {
		std::getline(fileStream, currentLine);
		if (currentLine.size() == 0) {
			std::cout << "Error: File empty" << std::endl;
			return;
		}

		std::cout << currentLine << std::endl; /* Debug */

		//Identify point in line at which to halt parsing
		findEndOfLine();
	}
}

void JackTokeniser::findEndOfLine() {
	for (std::string::iterator it = currentLine.begin(); it != currentLine.end(); it++) {
		if (*it == '\n' || *it == '*') {
			editLine(it);
			if (*it == '\n')
				break;
		}

		if (*it == '/') {
			it++;
			if (*it == '/' || *it == '*') {
				it--;
				editLine(it);
				break;
			}
		}
	}
}

void JackTokeniser::getStringConst() {
	cToken.clear();
	currentLine.erase(currentLine.begin());

	while (currentLine[0] != '"') {
		cToken.push_back(currentLine[0]);
		currentLine.erase(currentLine.begin());
	}
	currentLine.erase(currentLine.begin());
	tokType = "string_const";
}

void JackTokeniser::getIntConst() {
	bool isInt = true;

	for (std::string::iterator it = cToken.begin(); it != cToken.end(); it++) {

		if (!isADigit(*it)) {
			isInt = false;
			break;
		}
	}

	if (isInt)
		tokType = "int_const";
	else
		std::cout << "Error: undefined token type: " << cToken << std::endl;
}

bool JackTokeniser::isADigit(char &c) {
	return (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' ||
		c == '5' || c == '6' || c == '7' || c == '8' || c == '9');
}


void JackTokeniser::advance() {

	//Obtains the next line from fileStream to tokenise if currentLine empty
	
	getNextLine();
	
	//
	//Determine the next token from the current line and its type, save it, then erase it from current line

	//Check if token is a symbol
	if (isASymbol()) {
		cToken = currentLine[0];
		tokType = "symbol";
		currentLine.erase(currentLine.begin());
	}

	//Check if it's a string constant
	else if (currentLine[0] == '"') {
		getStringConst();
	}

	//Else it must be a string not in quotation marks (which is a string const) => check which type
	else {
		cToken.clear();

		//Create token, stopping when reaching a natural end
		while (!isASymbol() && currentLine[0] != ' ' && currentLine[0] != '/n' && currentLine[0] != '/0') {
			cToken.push_back(currentLine[0]);
			currentLine.erase(currentLine.begin());
		}
		
		//Check if keyword
		if (isAKeyword()) {
			tokType = "keyword";
		}
		
		//Having exhausted options, if first char is not a number then it must be an identifier
		else if (!isADigit(cToken[0])) {
			tokType = "identifier";
		}

		//Else, since first char is a number, string must be an int const or be invalid
		else {
			getIntConst();
		}
	}

	//Remove white space
	while (currentLine[0] == ' ' || currentLine[0] == '\t')
		currentLine.erase(currentLine.begin());
}

void JackTokeniser::editLine(std::string::iterator it) {
	
	if (it == currentLine.begin()) {
		currentLine.clear();
		return;
	}

	if (*it != '*') {
		currentLine.erase(it, currentLine.end());
		return;
	}
	
	return;
}


std::string JackTokeniser::tokenType() {

	//Returns KEYWORD, SYMBOL, IDENTIFIER, INT_CONST or STRING_CONST
	return tokType;
}

std::string JackTokeniser::getToken() { return cToken; }


void JackTokeniser::closeFile() { fileStream.close(); }
