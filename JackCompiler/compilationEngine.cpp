#include "stdafx.h"
#include "compilationEngine.h"
#include "jackTokeniser.h"
#include "vmWriter.h"
#include "symbolTable.h"
#include "Utils.h"

compilationEngine::compilationEngine(JackTokeniser &_jackTokeniser) : _jackTokeniser(_jackTokeniser) {}

void compilationEngine::compileFile(std::string file)
{
	_vmWriter = new VMWriter(file);

	advance();

	if (_jackTokeniser.getToken() == "class") {
		compileClass();
	}
	else
		std::cout << "Compile error: class declaration not found" << std::endl;
}

bool compilationEngine::advance() {
	if (_jackTokeniser.hasMoreTokens()) {
		_jackTokeniser.advance();
		return true;
	}
	else {
		throw std::exception("Unexpected end of file");
	}
}

std::string compilationEngine::getIdentifier() {

	if (_jackTokeniser.tokenType() == "identifier") {
		return _jackTokeniser.getToken();
	}
	else {
		throw std::exception("Missing identifier");
	}
}

bool compilationEngine::checkSymbol(std::string symbol, std::string statementType) {

	if (_jackTokeniser.getToken() == symbol)
		return true;
	else {
		std::cout << "Error: missing '" << symbol << "' in " << statementType << std::endl;
		return false;
	}
}

void compilationEngine::defineVariable(std::string &classVarType, std::string &classVarKind) {

	if (_jackTokeniser.tokenType() == "identifier") {
		classTable.define(_jackTokeniser.getToken(), classVarType, classVarKind);
	}
	else {
		std::cout << "Error: variable name expected" << std::endl;
		return;
	}
}

bool compilationEngine::isStatement() {
	return _jackTokeniser.getToken() == "if" ||
		   _jackTokeniser.getToken() == "let" ||
		   _jackTokeniser.getToken() == "do" ||
	 	   _jackTokeniser.getToken() == "while" ||
		   _jackTokeniser.getToken() == "return";
}

bool compilationEngine::isOperator() {

	return _jackTokeniser.getToken() == "+" ||
		   _jackTokeniser.getToken() == "-" ||
		   _jackTokeniser.getToken() == "*" ||
		   _jackTokeniser.getToken() == "/" ||
		   _jackTokeniser.getToken() == "&" ||
		   _jackTokeniser.getToken() == "|" ||
		   _jackTokeniser.getToken() == "<" ||
		   _jackTokeniser.getToken() == ">" ||
		   _jackTokeniser.getToken() == "=";
}


void compilationEngine::compileClass() {

	std::string className, subroutineKind;
	
	//IDENTIFIER
	advance();
	className = getIdentifier();

	//bracket
	advance();
	checkSymbol("{", "class");

	//New symbolTable for the class
	classTable.startSubroutine(" ", " ");

	//variable and method declarations
	while (advance()) {
		if (_jackTokeniser.getToken() == "field" || _jackTokeniser.getToken() == "static")
			compileClassVarDec();
		else if (_jackTokeniser.getToken() == "constructor" || 
			     _jackTokeniser.getToken() == "function" ||
			     _jackTokeniser.getToken() == "method") {

			subroutineKind = _jackTokeniser.getToken();
			compileSubroutineDec(className, subroutineKind);
		}
		else{
			checkSymbol("}", "class");
			break;
		}
	}
}


void compilationEngine::compileClassVarDec() {

	std::string classVarKind, classVarType;

	if (_jackTokeniser.getToken() == "static" || _jackTokeniser.getToken() == "field") {
		classVarKind = _jackTokeniser.getToken();
	}
	else
		return;

	//variable type
	advance();
	if (_jackTokeniser.tokenType() != "keyword" && _jackTokeniser.tokenType() != "identifier") {
		std::cout << "Error: invalid class variable type in class variable declaration." << std::endl;
	}
	else {
		classVarType = _jackTokeniser.getToken();
	}

	//variable name
	advance();
	classTable.define(_jackTokeniser.getToken(), classVarType, classVarKind);


	//check for additional variables
	while (advance()) {

		if (_jackTokeniser.getToken() == ",") {
			advance();
			classTable.define(getIdentifier(), classVarType, classVarKind);
		}
		else if (_jackTokeniser.getToken() == ";")
			break;
		else {
			std::cout << "Error: expected ';'";
		}
	}
}


void compilationEngine::compileSubroutineDec(std::string className, std::string subroutineKind) {

	std::string subroutineType, subroutineName;

	//Add method to list of methods
	methodTable.startSubroutine(className, subroutineKind);

	//subroutine TYPE
	advance();
	if (_jackTokeniser.getToken() != "void" && _jackTokeniser.getToken() != "int" &&
		_jackTokeniser.getToken() != "char" && _jackTokeniser.getToken() != "boolean" &&
		_jackTokeniser.tokenType() != "identifier") {

		std::cout << "Error: invalid subroutine return type" << std::endl;
		return;
	}
	else {
		subroutineType = _jackTokeniser.getToken();
	}

	//IDENTIFIER
	advance();
	subroutineName = getIdentifier();

	//PARAMETERS
	advance();
	checkSymbol("(", "subroutine declaration");
	compileParameterList();

	//METHOD BODY
	advance();
	checkSymbol("{", "subroutine declaration");
	compileSubroutineBody(className, subroutineName, subroutineKind);
}




void compilationEngine::compileParameterList() {

	std::string parameterType, parameterKind;
	bool commaPossible = false;

	while (advance()) {

		if (_jackTokeniser.getToken() == ")")
				break;
		else if(commaPossible && checkSymbol(",", "parameter list"))
			commaPossible = false;
		else if (_jackTokeniser.tokenType() == "keyword" || _jackTokeniser.tokenType() == "identifier") {

			parameterType = _jackTokeniser.getToken();
			advance();
			methodTable.define(getIdentifier(), parameterType, "argument");

			commaPossible = true;
		}
		else {
			throw std::exception("Unrecognised token in parameter list");
		}
	}
}


void compilationEngine::compileSubroutineBody(std::string className,
											  std::string subroutineName, 
											  std::string subroutineKind) 
{
	advance();
	while (_jackTokeniser.hasMoreTokens()) {

		if (_jackTokeniser.getToken() == "var")
			compileVarDec();
		else
			break;
	}

	while (_jackTokeniser.hasMoreTokens()) {

		if (_jackTokeniser.getToken() == "}")
			break;
		else if (isStatement()) {
			//Write function label 
			_vmWriter->writeFunction(className + "." + subroutineName, methodTable.varCount("local"));

			//Allocate memory for newly constructed object
			if (subroutineKind == "constructor") {
				_vmWriter->writePush("constant", classTable.varCount("field"));
				_vmWriter->writeCall("Memory.alloc", 1);
				_vmWriter->writePop("pointer", 0);
			}
			else if (subroutineKind == "method") {
				_vmWriter->writePush("argument", 0);
				_vmWriter->writePop("pointer", 0);
			}
			else if (subroutineKind != "function") {
				std::cout << "Error: invalid subroutine kind" << std::endl;
			}
			compileStatements(className);
		}
		else
			throw std::exception("Unrecognised statement");
	}
}


void compilationEngine::compileVarDec() {

	std::string varType;

	advance();
	if (_jackTokeniser.tokenType() == "keyword" || _jackTokeniser.tokenType() == "identifier") {
		varType = _jackTokeniser.getToken();
	}

	while (advance()) {
		if (_jackTokeniser.tokenType() == "identifier") {
			methodTable.define(_jackTokeniser.getToken(), varType, "local");
		}
		else if (_jackTokeniser.getToken() == ";")
			break;
		else{
			checkSymbol(",", "variable declaration. Alternatively missing an identifier or ;");
		}
	}
	advance();
}



void compilationEngine::compileStatements(std::string className) {

	do {
		if (_jackTokeniser.getToken() == "let")
			compileLet();
		else if (_jackTokeniser.getToken() == "if")
			compileIf(className);
		else if (_jackTokeniser.getToken() == "while")
			compileWhile(className);
		else if (_jackTokeniser.getToken() == "do")
			compileDo(className);
		else if (_jackTokeniser.getToken() == "return")
			compileReturn();
		else
			break;
	} while (_jackTokeniser.hasMoreTokens());
}

void compilationEngine::compileLet() {

	std::string lhsName, lhsSegment;
	int lhsIndex;

	//Variable being manipulated
	advance();
	lhsName = getIdentifier();

	//Check kind of variable to left of equals sign
	if (methodTable.kindOf(lhsName) != "none") {
		lhsSegment = methodTable.kindOf(lhsName);
		lhsIndex = methodTable.indexOf(lhsName);
	}
	else {
		if (classTable.kindOf(lhsName) == "field")
			lhsSegment = "this";
		else if (classTable.kindOf(lhsName) == "static")
			lhsSegment = "static";

		lhsIndex = classTable.indexOf(lhsName);
	}

	//check if array element
	bool isArray = false;
	while (advance()) {

		if (_jackTokeniser.getToken() == "=")
			break;
		else if (_jackTokeniser.getToken() == "[") {

			isArray = true;
			_vmWriter->writePush(lhsSegment, methodTable.indexOf(lhsName));

			advance();
			compileExpression();
			checkSymbol("]", "let");
		}
	}

	if (isArray) {
		_vmWriter->writeArithmetic("add");
	}
	// expression after equals sign
	advance();
	compileExpression();

	if (isArray) {
		_vmWriter->writePop("temp", 0);
		_vmWriter->writePop("pointer", 1);
		_vmWriter->writePush("temp", 0);
		_vmWriter->writePop("that", 0);
	}
	else {
		_vmWriter->writePop(lhsSegment, lhsIndex);
	}
	checkSymbol(";", "let");
	advance();
}


void compilationEngine::compileIf(std::string className) {

	int currentLabel = ++ifLabel;
	//OPENING BRACKET
	advance(); 
	checkSymbol("(", "if");
	//BOOLEAN EXPRESSION
	advance();
	compileExpression();
	_vmWriter->writeArithmetic("not");
	_vmWriter->writeIf("if-true" + std::to_string(currentLabel));
	checkSymbol(")", "if");
	//BRACKET
	advance();
	checkSymbol("{", "if");
	//STATEMENTS
	advance();
	if (_jackTokeniser.getToken() != "}") {
		compileStatements(className);
	}
	_vmWriter->writeGoto("if-false" + std::to_string(currentLabel));
	_vmWriter->writeLabel("if-true" + std::to_string(currentLabel));
	checkSymbol("}", "if");
	//ELSE statements
	advance();
	if (_jackTokeniser.getToken() == "else") {
		compileElse(className);
	}
	_vmWriter->writeLabel("if-false" + std::to_string(currentLabel));
}


void compilationEngine::compileElse(std::string &className) {
	advance();
	checkSymbol("{", "else");
	advance();
	if (_jackTokeniser.getToken() != "}") {
		compileStatements(className);
	}
	checkSymbol("}", "else");
	advance();
}


void compilationEngine::compileWhile(std::string className) {

	int currentLabel = ++whileLabel;
	//check for opening bracket
	advance();
	checkSymbol("(", "while loop");
	_vmWriter->writeLabel("loop-start" + std::to_string(currentLabel));
	//EXPRESSION
	advance();
	compileExpression();
	_vmWriter->writeArithmetic("not");
	_vmWriter->writeIf("loop-end" + std::to_string(currentLabel));
	//BRACKETS
	checkSymbol(")", "while loop");
	//BRACKETS
	advance();
	checkSymbol("{", "while loop");
	//STATEMENTS
	advance();
	if (_jackTokeniser.getToken() != "}") {
		compileStatements(className);
	}
	checkSymbol("}", "while loop");
	advance();
	_vmWriter->writeGoto("loop-start" + std::to_string(currentLabel));
	_vmWriter->writeLabel("loop-end" + std::to_string(currentLabel));
}


void compilationEngine::compileDo(std::string className) {

	std::string routineName, subroutineName;
	int nArgs = 0;

	advance();
	//SubroutineCall//
	routineName = getIdentifier();
	advance();
	//Check if varName.routine(), else routine()
	if (_jackTokeniser.getToken() == ".") {
		compileMethodCall(nArgs, subroutineName, routineName);
	}
	else { //calling methods within same class
		compileFunctionCall(nArgs, routineName, className);
	}
	checkSymbol("(", "do");
	advance();
	compileExpressionList(nArgs);
	// End of subroutineCall//
	_vmWriter->writeCall(routineName, nArgs);
	if (subroutineName != "new")
		_vmWriter->writePop("temp", 0);

	advance();
	checkSymbol(";", "do");
	advance();
}

void compilationEngine::compileMethodCall(int &nArgs, std::string &subroutineName, std::string &routineName) {

	advance();
	subroutineName = "." + getIdentifier();

	//Push object pointer if it is an object
	if (methodTable.kindOf(routineName) != "none") {
		_vmWriter->writePush(methodTable.kindOf(routineName), methodTable.indexOf(routineName));
		nArgs++;
		routineName = methodTable.typeOf(routineName);
	}
	else if (classTable.kindOf(routineName) != "none") {
		std::string varKind;
		if (classTable.kindOf(routineName) == "field")
			varKind = "this";
		else
			varKind = classTable.kindOf(routineName);
		_vmWriter->writePush(varKind, classTable.indexOf(routineName));
		nArgs++;
		routineName = classTable.typeOf(routineName);
	}
	routineName = routineName + subroutineName;
	advance();
}

void compilationEngine::compileFunctionCall(int &nArgs, std::string &routineName, std::string &className) {
	routineName = className + "." + routineName;
	nArgs++;
	_vmWriter->writePush("pointer", 0);
}

void compilationEngine::compileReturn() {

	advance();
	if (_jackTokeniser.getToken() != ";") {
		compileExpression();
	}
	else {
		_vmWriter->writePush("constant", 0);
	}
	_vmWriter->writeReturn();
	
	advance();
}



void compilationEngine::compileExpression() {

	bool firstTerm = true;
	std::string op = "none";

	while (_jackTokeniser.hasMoreTokens()) {

		if (_jackTokeniser.getToken() == ")" ||
			_jackTokeniser.getToken() == "]" ||
			_jackTokeniser.getToken() == ";" ||
			_jackTokeniser.getToken() == ",")
			break;
		else if (isOperator()) {
			if (firstTerm && _jackTokeniser.getToken() == "-") {
				compileTerm();
				firstTerm = false;
			}
			else {
				compileOperator(op);
			}
		}
		else {
			compileTerm();
			if (!firstTerm) {
				_vmWriter->writeArithmetic(op);
				op = "none";
			}
			firstTerm = false;
		}
	}
}

void compilationEngine::compileOperator(std::string &op) {

	if (_jackTokeniser.getToken() == "+")
		op = "add";
	else if (_jackTokeniser.getToken() == "-")
		op = "sub";
	else if (_jackTokeniser.getToken() == "*")
		op = "call Math.multiply 2";
	else if (_jackTokeniser.getToken() == "/")
		op = "call Math.divide 2";
	else if (_jackTokeniser.getToken() == "&")
		op = "and";
	else if (_jackTokeniser.getToken() == "|")
		op = "or";
	else if (_jackTokeniser.getToken() == "<")
		op = "lt";
	else if (_jackTokeniser.getToken() == ">")
		op = "gt";
	else if (_jackTokeniser.getToken() == "=")
		op = "eq";

	advance();
}


void compilationEngine::compileTerm() {

	//INT_CONST TERM
	if (_jackTokeniser.tokenType() == "int_const") {
		compileIntConst();
	}
	//STRING_CONST TERM
	else if (_jackTokeniser.tokenType() == "string_const") {
		compileStringConst();
	}
	//KEYWORD TERM (true, false, null, this)
	else if (_jackTokeniser.tokenType() == "keyword") {
		compileKeyword();
	}
	//BRACKETED EXPRESSION
	else if (_jackTokeniser.getToken() == "(") {
		advance();
		compileExpression();
	}
	//UNARY OP (NEGATION) TERM
	else if (_jackTokeniser.getToken() == "~" || _jackTokeniser.getToken() == "-") {
		compileUnaryOperator();
		return;
	}
	//ELSE current term is an identifier and may be be followed by a bracketed expression or local method
	else { 
		std::string routineName = getIdentifier();
		advance();

		if (_jackTokeniser.getToken() == "[" || _jackTokeniser.getToken() == "(" || _jackTokeniser.getToken() == ".") {
			compileArrayOrSubroutine(routineName);
		}
		else {
			compileTermVariable(routineName);
			return;
		}
	}
	advance();
}


void compilationEngine::compileIntConst() {

	int intConstant;
	Utils utilTemp;
	utilTemp.try_stoi(intConstant, _jackTokeniser.getToken());
	_vmWriter->writePush("constant", intConstant);
}


void compilationEngine::compileStringConst() {

	_vmWriter->writePush("constant", _jackTokeniser.getToken().length());
	_vmWriter->writeCall("String.new", 1);

	std::string stringConst = _jackTokeniser.getToken();
	for (auto it = stringConst.begin(); it != stringConst.end(); it++) {

		_vmWriter->writePush("constant", (int)*it);
		_vmWriter->writeCall("String.appendChar", 2);
	}
}


void compilationEngine::compileKeyword() {

	if (_jackTokeniser.getToken() == "true") {
		_vmWriter->writePush("constant", 0);
		_vmWriter->writeArithmetic("not");
	}
	else if (_jackTokeniser.getToken() == "false" || _jackTokeniser.getToken() == "null") {
		_vmWriter->writePush("constant", 0);
	}
	else if (_jackTokeniser.getToken() == "this") {
		_vmWriter->writePush("pointer", 0);
	}
}


void compilationEngine::compileUnaryOperator() {
	std::string op;
	if (_jackTokeniser.getToken() == "~")
		op = "not";
	else if (_jackTokeniser.getToken() == "-")
		op = "neg";

	advance();
	compileTerm();
	_vmWriter->writeArithmetic(op);
}


void compilationEngine::compileArrayOrSubroutine(std::string &routineName) {

	int nArgs = 0;
	std::string subroutineName;

	if (_jackTokeniser.getToken() == ".") {
		compileMethodCall(nArgs, subroutineName, routineName);
		checkSymbol("(", "term, object's method call");
	}

	if (_jackTokeniser.getToken() == "[") {
		compileTermArray(routineName);
	}
	else {
		advance();
		compileExpressionList(nArgs);
		_vmWriter->writeCall(routineName, nArgs);
	}
}


void compilationEngine::compileTermArray(std::string &routineName) {

	advance();
	if (methodTable.kindOf(routineName) != "none")
		_vmWriter->writePush(methodTable.kindOf(routineName), methodTable.indexOf(routineName));
	else if (classTable.kindOf(routineName) != "none")
		_vmWriter->writePush(classTable.kindOf(routineName), classTable.indexOf(routineName));

	compileExpression();

	_vmWriter->writeArithmetic("add");
	_vmWriter->writePop("pointer", 1);
	_vmWriter->writePush("that", 0);
}


void compilationEngine::compileTermVariable(std::string &routineName) {

	if (methodTable.kindOf(routineName) != "none")
		_vmWriter->writePush(methodTable.kindOf(routineName), methodTable.indexOf(routineName));

	else if (classTable.kindOf(routineName) != "none") {
		std::string varKind;
		if (classTable.kindOf(routineName) == "field")
			varKind = "this";
		else
			varKind = "static";
		_vmWriter->writePush(varKind, classTable.indexOf(routineName));
	}
}


void compilationEngine::compileExpressionList(int &nArgs) {

	while (_jackTokeniser.getToken() != ")") {
		compileExpression();
		nArgs++;
		if (_jackTokeniser.getToken() == ",") {
			advance();
		}
	}
}


compilationEngine::~compilationEngine() {
	delete _vmWriter;
}
