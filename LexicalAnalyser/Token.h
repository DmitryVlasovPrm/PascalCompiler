#pragma once

#include "Variant.h"
#include "Constants.h"
using namespace Constants;

class Token
{
private:
	int LineNumber;
	int StartPosition;
protected:
	TokenValue ValueType;
	Constants::TokenType TokenType;
public:
	Token(int lineNumber, int startPosition)
	{
		LineNumber = lineNumber;
		StartPosition = startPosition;
	}
	virtual ~Token() {}

	int GetLineNumber() { return LineNumber; }
	int GetStartPosition() { return StartPosition; }
	TokenValue GetValueType() { return ValueType; }
	void SetValueType(TokenValue valueType) { ValueType = valueType; }
	Constants::TokenType GetTokenType() { return TokenType; }
};

class OperatorToken : public Token
{
public:
	OperatorToken(int lineNumber, int startPosition, TokenValue valueType) : Token(lineNumber, startPosition)
	{
		ValueType = valueType;
		TokenType = Constants::TokenType::Operator;
	}
	~OperatorToken() {}
};

class IdentifierToken : public Token
{
private:
	string Name;
public:
	IdentifierToken(int lineNumber, int startPosition, string name) : Token(lineNumber, startPosition)
	{
		Name = name;
		TokenType = Constants::TokenType::Identifier;
	}
	~IdentifierToken() {}

	string GetName()
	{
		return Name;
	}

	bool operator<(const IdentifierToken& idToken) const { return Name < idToken.Name; }
};

class ValueToken : public Token
{
private:
	Variant* Value;
public:
	ValueToken(int lineNumber, int startPosition, int value) : Token(lineNumber, startPosition)
	{
		Value = new IntegerVariant(value);
		ValueType = TokenValue::integer_tk;
		TokenType = Constants::TokenType::Value;
	}

	ValueToken(int lineNumber, int startPosition, double value) : Token(lineNumber, startPosition)
	{
		Value = new DoubleVariant(value);
		ValueType = TokenValue::double_tk;
		TokenType = Constants::TokenType::Value;
	}

	ValueToken(int lineNumber, int startPosition, string value) : Token(lineNumber, startPosition)
	{
		Value = new StringVariant(value);
		ValueType = TokenValue::string_tk;
		TokenType = Constants::TokenType::Value;
	}

	ValueToken(int lineNumber, int startPosition, char value) : Token(lineNumber, startPosition)
	{
		Value = new CharVariant(value);
		ValueType = TokenValue::char_tk;
		TokenType = Constants::TokenType::Value;
	}

	ValueToken(int lineNumber, int startPosition, bool value) : Token(lineNumber, startPosition)
	{
		Value = new BoolVariant(value);
		ValueType = TokenValue::boolean_tk;
		TokenType = TokenType::Value;
	}

	~ValueToken() {}
	string GetValue()
	{
		string answer;
		if (ValueType == TokenValue::integer_tk)
		{
			int val = ((IntegerVariant*)(Value))->GetValue();
			answer = to_string(val);
		}

		if (ValueType == TokenValue::double_tk)
		{
			double val = ((DoubleVariant*)(Value))->GetValue();
			answer = to_string(val);
		}

		if (ValueType == TokenValue::char_tk)
		{
			char val = ((CharVariant*)(Value))->GetValue();
			answer = string(1, val);
		}

		if (ValueType == TokenValue::string_tk)
		{
			answer = ((StringVariant*)(Value))->GetValue();
		}

		if (ValueType == TokenValue::boolean_tk)
		{
			bool val = ((BoolVariant*)(Value))->GetValue();
			answer = val ? "true" : "false";
		}

		return answer;
	}
};
