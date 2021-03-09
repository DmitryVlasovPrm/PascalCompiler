#include "Variant.h"
#include "Token.h"
#include "Error.h"
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

string fileName = "pascal.txt";

int main()
{
	setlocale(LC_ALL, "Russian");

	int curLineNumber = 0;
	int curSymbolNumber = 0;
	vector <Token*> allTokens;
	vector <Error*> allErrors;

	int blockCommentType = 0;

	string lineBuffer;
	ifstream file(fileName);
	if (file.is_open())
	{
		while (getline(file, lineBuffer))
		{
			curLineNumber++;
			int lineLength = lineBuffer.length();
			for (int i = 0; i < lineLength; i++)
			{
				char curSymbol = lineBuffer[i];

				// Берем следующие строки для поиска конца блочного комментария
				if (blockCommentType != 0)
				{
					if (curSymbol == '}' && blockCommentType == 1)
						blockCommentType = 0;
					if (curSymbol == '*' && lineBuffer[i + 1] == ')' && blockCommentType == 2)
					{
						i++;
						blockCommentType = 0;
					}
					continue;
				}

				// Пропускаем пробелы
				if (curSymbol == ' ')
					continue;

				// Запрещенные символы
				if (curSymbol == '?' || curSymbol == '&' || curSymbol == '%')
				{
					string errorMsg = "Запрещенный символ";
					Error* error = new Error(errorMsg, curLineNumber, i + 1, i + 1);
					allErrors.push_back(error);
					continue;
				}

				// Идентификаторы
				if (curSymbol >= 'a' && curSymbol <= 'z' || curSymbol >= 'A' && curSymbol <= 'Z')
				{
					int curStartPosition = i;
					string curLexem = "";
					curLexem += curSymbol;

					while (i < lineLength - 1)
					{
						char nextSymbol = lineBuffer[i + 1];
						if (nextSymbol >= 'a' && nextSymbol <= 'z' || nextSymbol >= 'A' && nextSymbol <= 'Z' ||
							nextSymbol >= '0' && nextSymbol <= '9')
						{
							i++;
							curLexem += nextSymbol;
						}
						else
							break;
					}

					bool isReservedWord = ReservedWords.find(curLexem) != ReservedWords.end();
					Token* token = new IdentifierToken(curLineNumber, curStartPosition + 1, i + 1, isReservedWord, curLexem);
					allTokens.push_back(token);
					continue;
				}

				// Целые числа
				if (curSymbol >= '0' && curSymbol <= '9')
				{
					int curStartPosition = i;
					int curNumber = 0;
					curNumber += curSymbol - '0';
					bool isError = false;
					string errorMsg = "";

					while (i < lineLength - 1)
					{
						char nextSymbol = lineBuffer[i + 1];
						if (nextSymbol >= '0' && nextSymbol <= '9')
						{
							i++;
							if (!isError)
							{
								int digit = nextSymbol - '0';
								if (curNumber < INT_MAX / 10 || (curNumber == INT_MAX / 10 && digit <= INT_MAX % 10))
									curNumber = 10 * curNumber + digit;
								else
								{
									isError = true;
									errorMsg = "Значение превышает предел";
								}
							}
						}
						else
							break;
					}

					if (isError)
					{
						Error* error = new Error(errorMsg, curLineNumber, curStartPosition + 1, i + 1);
						allErrors.push_back(error);
					}
					else
					{
						Token* token = new ValueToken(curLineNumber, curStartPosition + 1, i + 1, curNumber);
						allTokens.push_back(token);
					}
					continue;
				}

				// Скобки и арифметические операторы
				if (curSymbol == '+' || curSymbol == '-' || curSymbol == '*' || 
					curSymbol == ')' || curSymbol == '[' || curSymbol == ']')
				{
					Token* token = new OperatorToken(curLineNumber, i + 1, i + 1, string(1, curSymbol));
					allTokens.push_back(token);
					continue;
				}

				// Строчные комментарии и знак деления
				if (curSymbol == '/')
				{
					if (lineBuffer[i + 1] == '/')
						i = lineLength;
					else
					{
						Token* token = new OperatorToken(curLineNumber, i + 1, i + 1, string(1, curSymbol));
						allTokens.push_back(token);
					}
					continue;
				}

				// Блочный комментарий {}
				if (curSymbol == '{')
				{
					blockCommentType = 1;
					continue;
				}

				// Блочный комментарий (**) или просто скобка (
				if (curSymbol == '(')
				{
					if (lineBuffer[i + 1] == '*')
					{
						i++;
						blockCommentType = 2;
						continue;
					}
					else
					{
						Token* token = new OperatorToken(curLineNumber, i + 1, i + 1, string(1, curSymbol));
						allTokens.push_back(token);
					}
				}
			}
		}
	}
	file.close();

	for (int i = 0; i < allTokens.size(); i++)
	{
		cout << allTokens[i]->GetValue() << endl;
		delete allTokens[i];
	}

	for (int i = 0; i < allErrors.size(); i++)
	{
		allErrors[i]->ShowError();
		delete allErrors[i];
	}

	return 0;
}