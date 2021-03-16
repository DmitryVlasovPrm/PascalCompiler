#pragma once

#include <fstream>
#include <vector>
#include <queue>
#include <thread>
#include <Windows.h>
#include "Token.h"
#include "Error.h"

class Lexer
{
private:
	string FileName;
	bool IsLastToken;
	queue<Token*> Tokens;
	vector<Error> AllErrors;
public:
	Lexer(string fileName);
	~Lexer();
	void Start();
	Token* GetToken();
	vector<Error> GetLexicalErrors() { return AllErrors; }
};