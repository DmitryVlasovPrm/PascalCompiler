#include "Lexer.h"
#include "Constants.h"
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

				// ����� ��������� ������ ��� ������ ����� �������� �����������
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

				// ���������� �������
				if (curSymbol == ' ')
					continue;

				// ����������� �������
				if (curSymbol == '?' || curSymbol == '&' || curSymbol == '%')
				{
					string errorMsg = "����������� ������";
					Error error = Error(errorMsg, curLineNumber, i + 1, i + 1);
					AllErrors.push_back(error);
					continue;
				}

				// ��������������
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
						Token* curToken = new ValueToken(curLineNumber, curStartPosition + 1, i + 1, value);
						Tokens.push(curToken);
					}
					else
					{
						bool isReservedWord = Constants::ReservedWords.find(curLexem) != Constants::ReservedWords.end();
						Token* curToken = new IdentifierToken(curLineNumber, curStartPosition + 1, i + 1, isReservedWord, curLexem);
						Tokens.push(curToken);
					}
					continue;
				}

				// ����� � ������������ �����
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
								Token* valToken = new ValueToken(curLineNumber, curStartPosition + 1, pointPos, curIntNumber);
								Tokens.push(valToken);
								Token* operToken = new OperatorToken(curLineNumber, pointPos + 1, i + 1, "..");
								Tokens.push(operToken);
								isColon = true;
								break;
							}
							if (pointPos != -1)
							{
								isError = true;
								errorMsg = "�������� ������ �����";
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
										errorMsg = "�������� ��������� ������";
									}
								}
							}
						}
						else
						{
							break;
						}
					}

					if (isError && (errorMsg != "�������� ��������� ������" || pointPos == -1))
					{
						Error error = Error(errorMsg, curLineNumber, curStartPosition + 1, i + 1);
						AllErrors.push_back(error);
					}
					else if (!isColon)
					{
						if (pointPos != -1)
						{
							double result = stod(strNumber);
							Token* curToken = new ValueToken(curLineNumber, curStartPosition + 1, i + 1, result);
							Tokens.push(curToken);
						}
						else
						{
							Token* curToken = new ValueToken(curLineNumber, curStartPosition + 1, i + 1, curIntNumber);
							Tokens.push(curToken);
						}
					}
					continue;
				}

				// ������ � �������
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
								Token* curToken = new ValueToken(curLineNumber, curStartPosition + 1, i + 1, ch);
								Tokens.push(curToken);
							}
							else
							{
								Token* curToken = new ValueToken(curLineNumber, curStartPosition + 1, i + 1, curStr);
								Tokens.push(curToken);
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
						Error error = Error("����������� ����������� ������", curLineNumber, curStartPosition + 1, i + 1);
						AllErrors.push_back(error);
					}
					continue;
				}

				// ����� ���������
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

					Token* curToken = new OperatorToken(curLineNumber, curStartPosition + 1, i + 1, curOper);
					Tokens.push(curToken);
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

					Token* curToken = new OperatorToken(curLineNumber, curStartPosition + 1, i + 1, curOper);
					Tokens.push(curToken);
					continue;
				}

				// ���������
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

					Token* curToken = new OperatorToken(curLineNumber, curStartPosition + 1, i + 1, curOper);
					Tokens.push(curToken);
					continue;
				}

				// �����
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

					Token* curToken = new OperatorToken(curLineNumber, curStartPosition + 1, i + 1, curOper);
					Tokens.push(curToken);
					continue;
				}

				// ������, �������������� ��������� � �� �������
				if (curSymbol == '+' || curSymbol == '-' || curSymbol == '*' || curSymbol == '=' ||
					curSymbol == ';' || curSymbol == '^' || curSymbol == ',' ||
					curSymbol == ')' || curSymbol == '[' || curSymbol == ']')
				{
					Token* curToken = new OperatorToken(curLineNumber, i + 1, i + 1, string(1, curSymbol));
					Tokens.push(curToken);
					continue;
				}

				// �������� ����������� � ���� �������
				if (curSymbol == '/')
				{
					if (lineBuffer[i + 1] == '/')
					{
						i = lineLength;
					}
					else
					{
						Token* curToken = new OperatorToken(curLineNumber, i + 1, i + 1, string(1, curSymbol));
						Tokens.push(curToken);
					}
					continue;
				}

				// ������� ����������� {}
				if (curSymbol == '{')
				{
					blockCommentType = 1;
					continue;
				}

				// ������� ����������� (**) ��� ������ ������ (
				if (curSymbol == '(')
				{
					if (lineBuffer[i + 1] == '*')
					{
						i++;
						blockCommentType = 2;
					}
					else
					{
						Token* curToken = new OperatorToken(curLineNumber, i + 1, i + 1, string(1, curSymbol));
						Tokens.push(curToken);
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

Token* Lexer::GetToken()
{
	if (IsLastToken && Tokens.empty())
		return NULL;

	while (Tokens.empty())
		Sleep(2);

	Token* curToken = Tokens.front();
	Tokens.pop();
	return curToken;
}