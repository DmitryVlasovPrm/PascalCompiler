#pragma once

#include <iostream>
using namespace std;

class Error
{
private:
	string ErrorMsg;
	int LineNumber;
	int StartPosition;
public:
	Error(string errorMsg, int lineNumber, int startPosition)
	{
		ErrorMsg = errorMsg;
		LineNumber = lineNumber;
		StartPosition = startPosition;
	}
	~Error() {}

	void ShowError()
	{
		cout << ErrorMsg << " Строка " << LineNumber << ", символ " << StartPosition << "." << endl;
	}
};
