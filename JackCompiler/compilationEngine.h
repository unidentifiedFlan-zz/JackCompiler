#pragma once

#include "stdafx.h"
#include "JackTokeniser.h"
#include "symbolTable.h"
#include "VMWriter.h"

class compilationEngine
{
	JackTokeniser &_jackTokeniser;
	VMWriter *_vmWriter;

	symbolTable classTable;
	symbolTable methodTable;
	int ifLabel = 0, whileLabel = 0;

	bool advance();
	std::string getIdentifier();
	bool checkSymbol(std::string symbol, std::string statementType);
	void defineVariable(std::string &classVarType, std::string &classVarKind);
	bool isStatement();
	bool isOperator();

	void compileClass();
	void compileClassVarDec();
	void compileSubroutineDec(std::string className, std::string subrouitneKind);
	void compileParameterList();
	void compileSubroutineBody(std::string className,
		std::string subroutineName, std::string subroutineKind);
	void compileVarDec();
	void compileStatements(std::string className);
	void compileLet();
	void compileIf(std::string className);
	void compileElse(std::string &className);
	void compileWhile(std::string className);
	void compileDo(std::string className);
	void compileMethodCall(int &nArgs, std::string &subroutineName, std::string &routineName);
	void compileFunctionCall(int &nArgs, std::string &routineName, std::string &className);
	void compileReturn();
	void compileExpression();
	void compileOperator(std::string &op);
	void compileTerm();
	void compileIntConst();
	void compileStringConst();
	void compileKeyword();
	void compileUnaryOperator();
	void compileArrayOrSubroutine(std::string &routineName);
	void compileTermArray(std::string &routineName);
	void compileTermVariable(std::string &routineName);
	void compileExpressionList(int &nArgs);
public:

	compilationEngine(JackTokeniser &jackTokeniser);

	void compileFile(std::string file);

	~compilationEngine();
};