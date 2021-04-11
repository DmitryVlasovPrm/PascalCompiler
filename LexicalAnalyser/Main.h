#pragma once

#include "Lexer.h"

// Функции для синтаксиса
void Program();
void Block();
void TypePart();
void TypeDeclaration();
void VarPart();
void VarDeclaration();
void CompoundStatement();
void Statement();
TokenValue Expression();
TokenValue SimpleExpression();
TokenValue Item();
TokenValue Multiplier();
void IfConditional();
void WhileCycle();

// Вспомогательные функции
void GetNextToken();
bool IsThisOperatorValue(TokenValue tValue);
bool IsThisOperatorValue(set<TokenValue> tValues);
bool SkipToVariousTokens(set<TokenValue> sValues, set<Constants::TokenType> sTypes, set<TokenValue> fValues);
string GetKeyByValue(TokenValue tValue);
TokenValue CheckIdentifier();
TokenValue CompareTypes(TokenValue leftPart, TokenValue rightPart, Token operationToken);