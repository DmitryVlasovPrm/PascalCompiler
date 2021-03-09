#pragma once

#include <iostream>
using namespace std;

static class Error
{
private:
	string ErrorMsg;
	int LineNumber;
	int StartPosition;
	int EndPosition;
public:
	Error(string errorMsg, int lineNumber, int startPosition, int endPosition)
	{
		ErrorMsg = errorMsg;
		LineNumber = lineNumber;
		StartPosition = startPosition;
		EndPosition = endPosition;
	}
	~Error() {}

	void ShowError()
	{
		if (StartPosition != EndPosition)
			cout << "������: \"" << ErrorMsg << "\". ������ " << LineNumber << 
			", ������� " << StartPosition << " - " << EndPosition << "." << endl;
		else
			cout << "������: \"" << ErrorMsg << "\". ������ " << LineNumber <<
			", ������ " << StartPosition << "." << endl;
	}
};
