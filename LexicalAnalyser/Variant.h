#pragma once

#include <string>
#include <iostream>
#include "Constants.h"
using namespace Constants;

class Variant
{
protected:
	VariantType Type;
public:
	Variant() {}
	virtual ~Variant() {}
	VariantType GetType() { return Type; }
};

class IntegerVariant : public Variant
{
private:
	int Value;
public:
	IntegerVariant(int value)
	{
		Value = value;
		Type = VariantType::integer_type;
	}
	~IntegerVariant() {}
	int GetValue() { return Value; }
};

class DoubleVariant : public Variant
{
private:
	double Value;
public:
	DoubleVariant(double value)
	{
		Value = value;
		Type = VariantType::double_type;
	}
	~DoubleVariant() {}
	double GetValue() { return Value; }
};

class StringVariant : public Variant
{
private:
	string Value;
public:
	StringVariant(string value)
	{
		Value = value;
		Type = VariantType::string_type;
	}
	~StringVariant() {}
	string GetValue() { return Value; }
};

class CharVariant : public Variant
{
private:
	char Value;
public:
	CharVariant(char value)
	{
		Value = value;
		Type = VariantType::char_type;
	}
	~CharVariant() {}
	char GetValue() { return Value; }
};

class BoolVariant : public Variant
{
private:
	bool Value;
public:
	BoolVariant(bool value)
	{
		Value = value;
		Type = VariantType::boolean_type;
	}
	~BoolVariant() {}
	bool GetValue() { return Value; }
};
