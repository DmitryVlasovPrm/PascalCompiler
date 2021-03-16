#pragma once

#include <string>
#include <iostream>
using namespace std;

enum class VariantTypeEnum
{
	integer,
	__identifier(double),
	__identifier(string),
	__identifier(char),
	boolean
};

class Variant
{
protected:
	VariantTypeEnum Type;
public:
	Variant() {}
	virtual ~Variant() {}
	VariantTypeEnum GetType() { return Type; }
};

class IntegerVariant : public Variant
{
private:
	int Value;
public:
	IntegerVariant(int value)
	{
		Value = value;
		Type = VariantTypeEnum::integer;
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
		Type = VariantTypeEnum::__identifier(double);
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
		Type = VariantTypeEnum::__identifier(string);
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
		Type = VariantTypeEnum::__identifier(char);
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
		Type = VariantTypeEnum::boolean;
	}
	~BoolVariant() {}
	bool GetValue() { return Value; }
};
