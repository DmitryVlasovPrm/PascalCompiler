#include "Lexer.h"
#include <cmath>

Lexer::Lexer(string fileName)
{
	FileName = fileName;
	IsLastToken = false;
}

void Lexer::Start()
{
	string lineBuffer;
	int curLineNumber = 0;
	int curSymbolNumber = 0;
	int blockCommentType = 0;

	ifstream file(FileName);
	if (file.is_open())
	{
		while (getline(file, lineBuffer))
		{
			curLineNumber++;
			int lineLength = lineBuffer.length();
			for (int i = 0; i < lineLength; i++)
			{
				while (Tokens.size() > 20)
					Sleep(2);

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
					string errorMsg = "Ошибка: запрещенный символ.";
					unique_ptr<Error> error = make_unique<Error>(errorMsg, curLineNumber, i + 1, i + 1);
					LexicalErrors.push_back(move(error));
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
						{
							break;
						}
					}

					if (curLexem == "true" or curLexem == "false")
					{
						bool value = curLexem == "true" ? true : false;
						unique_ptr<Token> curToken = make_unique<ValueToken>(curLineNumber, curStartPosition + 1, i + 1, value);
						Tokens.push(move(curToken));
					}
					else
					{
						unique_ptr<Token> curToken;
						auto it = KeyTokenName.find(curLexem);
						if (it == KeyTokenName.end())
						{
							curToken = make_unique<IdentifierToken>(curLineNumber, curStartPosition + 1, i + 1, curLexem);
						}
						else
						{
							curToken = make_unique<OperatorToken>(curLineNumber, curStartPosition + 1, i + 1, it->second);
						}
						Tokens.push(move(curToken));
					}
					continue;
				}

				// Целые и вещественные числа
				if (curSymbol >= '0' && curSymbol <= '9')
				{
					int curStartPosition = i;
					int curIntNumber = 0;
					curIntNumber += curSymbol - '0';
					string strNumber = "";
					strNumber += curSymbol;
					bool isError = false;
					bool isColon = false;
					int pointPos = -1;
					string errorMsg = "";

					while (i < lineLength - 1)
					{
						char nextSymbol = lineBuffer[i + 1];
						strNumber += nextSymbol;

						if (nextSymbol == '.')
						{
							i++;
							if (pointPos == i - 1)
							{
								unique_ptr<Token> valToken = make_unique<ValueToken>(curLineNumber, curStartPosition + 1, pointPos, curIntNumber);
								Tokens.push(move(valToken));
								unique_ptr<Token> operToken = make_unique<OperatorToken>(curLineNumber, pointPos + 1, i + 1, TokenName::points_tk);
								Tokens.push(move(operToken));
								isColon = true;
								break;
							}
							if (pointPos != -1)
							{
								isError = true;
								errorMsg = "Неверный формат числа";
							}
							pointPos = i;
							continue;
						}

						if (nextSymbol >= '0' && nextSymbol <= '9')
						{
							i++;
							if (!isError)
							{
								int digit = nextSymbol - '0';
								if (pointPos == -1)
								{
									if (curIntNumber < INT_MAX / 10 || (curIntNumber == INT_MAX / 10 && digit <= INT_MAX % 10))
									{
										curIntNumber = 10 * curIntNumber + digit;
									}
									else
									{
										isError = true;
										errorMsg = "Лексическая ошибка: значение превышает предел.";
									}
								}
							}
						}
						else
						{
							break;
						}
					}

					if (isError && (errorMsg != "Ошибка: значение превышает предел." || pointPos == -1))
					{
						unique_ptr<Error> error = make_unique<Error>(errorMsg, curLineNumber, curStartPosition + 1, i + 1);
						LexicalErrors.push_back(move(error));
					}
					else if (!isColon)
					{
						if (pointPos != -1)
						{
							double result = stod(strNumber);
							unique_ptr<Token> curToken = make_unique<ValueToken>(curLineNumber, curStartPosition + 1, i + 1, result);
							Tokens.push(move(curToken));
						}
						else
						{
							unique_ptr<Token> curToken = make_unique<ValueToken>(curLineNumber, curStartPosition + 1, i + 1, curIntNumber);
							Tokens.push(move(curToken));
						}
					}
					continue;
				}

				// Строки и символы
				if (curSymbol == '\'')
				{
					int curStartPosition = i;
					string curStr = "";
					bool isTokenReceived = false;

					while (i < lineLength - 1)
					{
						i++;
						char nextSymbol = lineBuffer[i];
						if (nextSymbol == '\'')
						{
							if (curStr.length() == 1)
							{
								char ch = curStr[0];
								unique_ptr<Token> curToken = make_unique<ValueToken>(curLineNumber, curStartPosition + 1, i + 1, ch);
								Tokens.push(move(curToken));
							}
							else
							{
								unique_ptr<Token> curToken = make_unique<ValueToken>(curLineNumber, curStartPosition + 1, i + 1, curStr);
								Tokens.push(move(curToken));
							}
							isTokenReceived = true;
							break;
						}
						else
						{
							curStr += nextSymbol;
						}
					}

					if (!isTokenReceived)
					{
						unique_ptr<Error> error = make_unique<Error>("Ошибка: отсутствует закрывающий символ.", curLineNumber, curStartPosition + 1, i + 1);
						LexicalErrors.push_back(move(error));
					}
					continue;
				}

				// Знаки сравнения
				if (curSymbol == '<')
				{
					int curStartPosition = i;
					string curOper = "";
					curOper += curSymbol;

					char nextSymbol = lineBuffer[i + 1];
					if (nextSymbol == '=' || nextSymbol == '>')
					{
						i++;
						curOper += nextSymbol;
					}

					TokenName name = KeyTokenName.find(curOper)->second;
					unique_ptr<Token> curToken = make_unique<OperatorToken>(curLineNumber, curStartPosition + 1, i + 1, name);
					Tokens.push(move(curToken));
					continue;
				}

				if (curSymbol == '>')
				{
					int curStartPosition = i;
					string curOper = "";
					curOper += curSymbol;

					char nextSymbol = lineBuffer[i + 1];
					if (nextSymbol == '=')
					{
						i++;
						curOper += nextSymbol;
					}

					TokenName name = KeyTokenName.find(curOper)->second;
					unique_ptr<Token> curToken = make_unique<OperatorToken>(curLineNumber, curStartPosition + 1, i + 1, name);
					Tokens.push(move(curToken));
					continue;
				}

				// Двоеточие
				if (curSymbol == ':')
				{
					int curStartPosition = i;
					string curOper = "";
					curOper += curSymbol;

					char nextSymbol = lineBuffer[i + 1];
					if (nextSymbol == '=')
					{
						i++;
						curOper += nextSymbol;
					}

					TokenName name = KeyTokenName.find(curOper)->second;
					unique_ptr<Token> curToken = make_unique<OperatorToken>(curLineNumber, curStartPosition + 1, i + 1, name);
					Tokens.push(move(curToken));
					continue;
				}

				// Точки
				if (curSymbol == '.')
				{
					int curStartPosition = i;
					string curOper = "";
					curOper += curSymbol;

					char nextSymbol = lineBuffer[i + 1];
					if (nextSymbol == '.')
					{
						i++;
						curOper += nextSymbol;
					}

					TokenName name = KeyTokenName.find(curOper)->second;
					unique_ptr<Token> curToken = make_unique<OperatorToken>(curLineNumber, curStartPosition + 1, i + 1, name);
					Tokens.push(move(curToken));
					continue;
				}

				// Скобки, арифметические операторы и др символы
				if (curSymbol == '+' || curSymbol == '-' || curSymbol == '*' || curSymbol == '=' ||
					curSymbol == ';' || curSymbol == ',' || curSymbol == ')' || curSymbol == '[' || curSymbol == ']')
				{
					TokenName name = KeyTokenName.find(string(1, curSymbol))->second;
					unique_ptr<Token> curToken = make_unique<OperatorToken>(curLineNumber, i + 1, i + 1, name);
					Tokens.push(move(curToken));
					continue;
				}

				// Строчные комментарии и знак деления
				if (curSymbol == '/')
				{
					if (lineBuffer[i + 1] == '/')
					{
						i = lineLength;
					}
					else
					{
						TokenName name = KeyTokenName.find(string(1, curSymbol))->second;
						unique_ptr<Token> curToken = make_unique<OperatorToken>(curLineNumber, i + 1, i + 1, name);
						Tokens.push(move(curToken));
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
					}
					else
					{
						TokenName name = KeyTokenName.find(string(1, curSymbol))->second;
						unique_ptr<Token> curToken = make_unique<OperatorToken>(curLineNumber, i + 1, i + 1, name);
						Tokens.push(move(curToken));
					}
					continue;
				}
			}
		}
	}
	file.close();
	IsLastToken = true;
}

Lexer::~Lexer() {}

unique_ptr<Token> Lexer::GetToken()
{
	while (Tokens.empty())
	{
		Sleep(2);
		if (IsLastToken && Tokens.empty())
			return NULL;
	}

	unique_ptr<Token> curToken = move(Tokens.front());
	Tokens.pop();
	return curToken;
}