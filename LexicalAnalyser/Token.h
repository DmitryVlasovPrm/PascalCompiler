#pragma once

#include "Variant.h"

class Token
{
private:
	int LineNumber;
	int StartPosition;
	int EndPosition;
protected:
	Constants::TokenType Type;
public:
	Token(int lineNumber, int startPosition, int endPosition)
	{
		LineNumber = lineNumber;
		StartPosition = startPosition;
		EndPosition = endPosition;
	}
	virtual ~Token() {}
	int GetLineNumber() { return LineNumber; }
	int GetStartPosition() { return StartPosition; }
	int GetEndPosition() { return EndPosition; }
	Constants::TokenType GetType() { return Type; }
};

class OperatorToken : public Token
{
private:
	TokenName Name;
public:
	OperatorToken(int lineNumber, int startPosition, int endPosition, TokenName name) :
		Token(lineNumber, startPosition, endPosition)
	{
		Name = name;
		Type = Constants::TokenType::Operator;
	}
	~OperatorToken() {}
	TokenName GetName()
	{
		return Name;
	}
};

class IdentifierToken : public Token
{
private:
	string Name;
public:
	IdentifierToken(int lineNumber, int startPosition, int endPosition, string name) :
		Token(lineNumber, startPosition, endPosition)
	{
		Name = name;
		Type = Constants::TokenType::Identifier;
	}
	~IdentifierToken() {}
	string GetName()
	{
		return Name;
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
		Type = Constants::TokenType::Value;
	}

	ValueToken(int lineNumber, int startPosition, int endPosition, double value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = new DoubleVariant(value);
		Type = Constants::TokenType::Value;
	}

	ValueToken(int lineNumber, int startPosition, int endPosition, string value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = new StringVariant(value);
		Type = Constants::TokenType::Value;
	}

	ValueToken(int lineNumber, int startPosition, int endPosition, char value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = new CharVariant(value);
		Type = Constants::TokenType::Value;
	}

	ValueToken(int lineNumber, int startPosition, int endPosition, bool value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = new BoolVariant(value);
		Type = TokenType::Value;
	}

	~ValueToken() {}
	string GetValue()
	{
		string answer;
		if (Value->GetType() == VariantType::integer_type)
		{
			int val = ((IntegerVariant*)(Value))->GetValue();
			answer = to_string(val);
		}

		if (Value->GetType() == VariantType::double_type)
		{
			double val = ((DoubleVariant*)(Value))->GetValue();
			answer = to_string(val);
		}

		if (Value->GetType() == VariantType::char_type)
		{
			char val = ((CharVariant*)(Value))->GetValue();
			answer = string(1, val);
		}

		if (Value->GetType() == VariantType::string_type)
		{
			answer = ((StringVariant*)(Value))->GetValue();
		}

		if (Value->GetType() == VariantType::boolean_type)
		{
			bool val = ((BoolVariant*)(Value))->GetValue();
			answer = val ? "true" : "false";
		}
		return answer;
	}
};