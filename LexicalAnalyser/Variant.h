#pragma once

#include <string>
#include <iostream>
using namespace std;

enum class VariantType
{
	integer,
	__identifier(double),
	__identifier(string),
	__identifier(char)
};

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
		Type = VariantType::integer;
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
		Type = VariantType::__identifier(double);
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
		Type = VariantType::__identifier(string);
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
		Type = VariantType::__identifier(char);
	}
	~CharVariant() {}
	char GetValue() { return Value; }
};
