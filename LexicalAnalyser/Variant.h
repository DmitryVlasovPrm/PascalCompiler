#pragma once

#include <string>
#include <iostream>
#include "Constants.h"
using namespace Constants;

class Variant
{
public:
	Variant() {}
	virtual ~Variant() {}
};

class IntegerVariant : public Variant
{
private:
	int Value;
public:
	IntegerVariant(int value)
	{
		Value = value;
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
	}
	~BoolVariant() {}
	bool GetValue() { return Value; }
};
