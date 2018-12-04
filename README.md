This project was completed as part of the Nand2Tetris course found here: https://www.nand2tetris.org/

The JackCompiler is a compiler for a simple Java-like language called Jack. 
The grammar can be found here: https://drive.google.com/file/d/1ujgcS7GoI-zu56FxhfkTAvEgZ6JT7Dxl/view

Jack grammar repeated below for convenience:

LEXICAL ELEMENTS////////////////////////////////////////////////////////////////////
Keywords: class, constructor, function, method, 
          field, static, var, int, char, boolean, 
          void, true, false, null, this, let, do,  
          if, else, while, return
          
Symbols: {, }, (, ), [, ], ., ',', ;, +, -, *, /, &, |, <, >, =, ~

Integer constants: decimals in range 0...32767

String constants: '"'a sequence of Unicode characters not including double quotes or newline '"'

Identifiers: a sequence of letter, digits and underscores, not starting with a digit

PROGRAM STRUCTURE///////////////////////////////////////////////////////////////////

Class:          'class' className '{' classVarDec* subroutineDec* '}'

ClassVarDec:    ('static'|'field') type varName(',' varName)*';'

type:           'int'|'char'|'boolean'|className

subroutineDec:  ('constructor'|'function'|'method') ('void'|type) subroutineName '('parameterList')' subroutineBody

parameterList:  ( (type varName) (',' type varName)*)?

subroutineBody: '{'varDec* statements'}'

varDec:         'var' type varName(',' varName)*';'

className:      identifier

subroutineName: identifier

varName:        identifier


STATEMENTS///////////////////////////////////////////////////////////////////////////////////////

statements:     statement*

statement:      letStatement|ifStatement|whileStatement|doStatement|returnStatement

letStatement:   'let' varName('['expression']')?'='expression';'

ifStatement:    'if''('expression')''{'statements'}'

doStatement:    'do' subroutineCall';'

returnStatement:'return' expression?';'


EXPRESSIONS//////////////////////////////////////////////////////////////////////////////////////

expression:      term(op term)*

term:            integerConstant|stringConstant|keywordConstant|varName|varName'['expression']'|
                 subroutineCall|'('expression')'|unaryop term
                 
subroutineCall:  subroutineName'('expressionList')' | (className|varName)'.'subroutineName'(expressionList')'

expressionList:  (expression(','expression)*)?

op:              '+'|'-'|'*'|'/'|'&'|'|'|'<'|'>'|'='

unaryOp:         '-'|'~'

keywordConstant: 'true'|'false'|'null'|'this'
                 
