#pragma once

#include "Variant.h"

enum class TokenTypeEnum
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
	TokenTypeEnum Type;
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
	TokenTypeEnum GetType() { return Type; }
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
		Type = TokenTypeEnum::Operator;
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
		Type = TokenTypeEnum::Identifier;
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
		Type = TokenTypeEnum::Value;
	}

	ValueToken(int lineNumber, int startPosition, int endPosition, double value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = new DoubleVariant(value);
		Type = TokenTypeEnum::Value;
	}

	ValueToken(int lineNumber, int startPosition, int endPosition, string value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = new StringVariant(value);
		Type = TokenTypeEnum::Value;
	}

	ValueToken(int lineNumber, int startPosition, int endPosition, char value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = new CharVariant(value);
		Type = TokenTypeEnum::Value;
	}

	ValueToken(int lineNumber, int startPosition, int endPosition, bool value) :
		Token(lineNumber, startPosition, endPosition)
	{
		Value = new BoolVariant(value);
		Type = TokenTypeEnum::Value;
	}

	~ValueToken() {}
	string GetValue()
	{
		string answer;
		if (Value->GetType() == VariantTypeEnum::integer)
		{
			int val = ((IntegerVariant*)(Value))->GetValue();
			answer = to_string(val);
		}

		if (Value->GetType() == VariantTypeEnum::__identifier(double))
		{
			double val = ((DoubleVariant*)(Value))->GetValue();
			answer = to_string(val);
		}

		if (Value->GetType() == VariantTypeEnum::__identifier(char))
		{
			char val = ((CharVariant*)(Value))->GetValue();
			answer = string(1, val);
		}

		if (Value->GetType() == VariantTypeEnum::__identifier(string))
		{
			answer = ((StringVariant*)(Value))->GetValue();
		}

		if (Value->GetType() == VariantTypeEnum::boolean)
		{
			bool val = ((BoolVariant*)(Value))->GetValue();
			answer = val ? "true" : "false";
		}
		return answer;
	}
};