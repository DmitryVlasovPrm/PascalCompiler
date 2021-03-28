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
	queue<unique_ptr<Token>> Tokens;
	vector<unique_ptr<Error>> AllErrors;
public:
	Lexer(string fileName);
	~Lexer();
	void Start();
	unique_ptr<Token> GetToken();
	vector<unique_ptr<Error>> GetLexicalErrors() { return move(AllErrors); }
};