#pragma once

#include "Variant.h"
#include <set>
#include <string>

set <string> Operators = { "<=", "<>", ">=", ">", "<", ":=", ":", "+", "-", "*", "*)", "/", "=", "(", "(*", ")",
							"{", "}", "'", "[", "]", ".", "..", ",", "^", ";" };

set <string> ReservedWords = { "if", "do", "of", "or", "in", "to", "end", "var", "div", "and", "not", "for", "mod", "nil",
								"set", "then", "else", "case", "file", "goto", "type", "with", "read", "begin", "while", "array",
								"const", "label", "until", "write", "readln", "downto", "packed", "record", "repeat", "program",
								"writeln", "function", "procedure" };

enum class TokenType
{
	Operator,
	Identifier,
	Value
};

class Token
{
private:
	int LineNumber;
	int StartPosition;
	int EndPosition;
protected:
	TokenType Type;
public:
	Token(int lineNumber, int startPosition, int endPosition)
	{
		LineNumber = lineNumber;
		StartPosition = startPosition;
		EndPosition = endPosition;
	}
	virtual ~Token() {}
	virtual string GetValue() { return ""; }
	int GetLineNumber() { return LineNumber; }
	int GetStartPosition() { return StartPosition; }
	int GetEndPosition() { return EndPosition; }
	TokenType GetType() { return Type; }
};

class OperatorToken : public Token
{
private:
	string Value;
public:
	OperatorToken(int lineNumber, int startPosition, int endPosition, string value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = value;
		Type = TokenType::Operator;
	}
	~OperatorToken() {}
	string GetValue()
	{
		return Value;
	}
};

class IdentifierToken : public Token
{
private:
	string Value;
	bool IsReservedWord;
public:
	IdentifierToken(int lineNumber, int startPosition, int endPosition, bool isReservedWord, string value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = value;
		IsReservedWord = isReservedWord;
		Type = TokenType::Identifier;
	}
	~IdentifierToken() {}
	string GetValue()
	{
		return Value;
	}
};

class ValueToken : public Token
{
private:
	Variant* Value;
public:
	ValueToken(int lineNumber, int startPosition, int endPosition, int value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = new IntegerVariant(value);
		Type = TokenType::Value;
	}

	ValueToken(int lineNumber, int startPosition, int endPosition, double value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = new DoubleVariant(value);
		Type = TokenType::Value;
	}

	ValueToken(int lineNumber, int startPosition, int endPosition, string value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = new StringVariant(value);
		Type = TokenType::Value;
	}

	ValueToken(int lineNumber, int startPosition, int endPosition, int length, char value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = new CharVariant(value);
		Type = TokenType::Value;
	}

	~ValueToken() {}
	string GetValue()
	{
		string answer;
		if (Value->GetType() == VariantType::integer)
		{
			int val = ((IntegerVariant*)(Value))->GetValue();
			answer = to_string(val);
		}
		if (Value->GetType() == VariantType::__identifier(double))
		{
			double val = ((DoubleVariant*)(Value))->GetValue();
			answer = to_string(val);
		}
		if (Value->GetType() == VariantType::__identifier(char))
		{
			char val = ((CharVariant*)(Value))->GetValue();
			answer = to_string(val);
		}
		if (Value->GetType() == VariantType::__identifier(string))
		{
			answer = ((IntegerVariant*)(Value))->GetValue();
		}
		return answer;
	}
};