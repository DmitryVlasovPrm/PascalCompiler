#include "Lexer.h"
#include <algorithm>
#include <set>

string file = "semantic.txt";
Lexer lexer = Lexer(file);
unique_ptr<Token> curToken;
int lineNumber = 0, startPos = 0;
vector<unique_ptr<Error>> SyntacticErrors;

// ������� ��� ����������
void Program();
void Block();
void VarPart();
void VarDeclaration();
void CompoundStatement();
void Statement();
void SimpleExpression();
void Item();
void Multiplier();

// ��������������� �������
bool IsThisOperatorName(TokenName tName);
bool IsThisOperatorName(set<TokenName> tNames);
bool SkipToVariousTokens(set<TokenName> sNames, set<Constants::TokenType> sTypes, set<TokenName> fNames);
string GetKeyByValue(TokenName tName);
void GetToken();

void AcceptIdent()
{
	if (curToken != NULL && curToken->GetType() == TokenType::Identifier)
	{
		GetToken();
		return;
	}
	else
	{
		throw invalid_argument("�������� �������������.");
	}
}

void AcceptOper(TokenName tokenName)
{
	if (curToken != NULL && IsThisOperatorName(tokenName))
	{
		GetToken();
		return;
	}
	else
	{
		throw invalid_argument("�������� ��������: \"" + GetKeyByValue(tokenName) + "\".");
	}
}

void AcceptOper(vector<TokenName> tokenNames)
{
	if (curToken != NULL && curToken->GetType() == TokenType::Operator)
	{
		for (TokenName name : tokenNames)
		{
			if (((OperatorToken*)curToken.get())->GetName() == name)
			{
				GetToken();
				return;
			}
		}
	}

	string operatorsStr = "";
	for (TokenName name : tokenNames)
	{
		if (name == tokenNames[tokenNames.size() - 1])
			operatorsStr += GetKeyByValue(name);
		else
			operatorsStr += "\"" + GetKeyByValue(name) + "\", ";

	}
	throw invalid_argument("�������� ���� �� ����������: " + operatorsStr + ".");
}

// ���������
void Program()
{
	if (curToken == NULL)
		return;

	// ���������� ������� ��� ����, ����� ����� program
	if (!IsThisOperatorName(TokenName::program_tk))
	{
		int startPos = curToken->GetStartPosition();
		unique_ptr<Error> error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), startPos, startPos);
		SyntacticErrors.push_back(move(error));
		if (!SkipToVariousTokens(set<TokenName> {TokenName::program_tk}, set<Constants::TokenType>(), set<TokenName> {TokenName::var_tk, TokenName::begin_tk}))
			return;
	}

	try
	{
		AcceptOper(TokenName::program_tk);
		// ���
		AcceptIdent();
		AcceptOper(TokenName::semicolon_tk);
		// ����
		Block();
		AcceptOper(TokenName::point_tk);
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� CompoundStatement
		unique_ptr<Error> error = make_unique<Error>("������ � ������� \"���������\". " + (string)ex.what(), lineNumber, startPos, startPos);
		SyntacticErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenName>(), set<Constants::TokenType>(), set<TokenName> {TokenName::begin_tk});
	}
}

// ����
void Block()
{
	// ������ ����������
	VarPart();
	// ������ ���������� (��������� ��������)
	CompoundStatement();
}

// ������ ����������
void VarPart()
{
	try
	{
		if (curToken == NULL)
			return;

		// ���� ��� ������� �������
		if (IsThisOperatorName(TokenName::begin_tk))
			return;

		// ���������� ������� ��� ����, ����� ����� var
		if (!IsThisOperatorName(TokenName::var_tk))
		{
			int startPos = curToken->GetStartPosition();
			unique_ptr<Error> error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), startPos, startPos);
			SyntacticErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenName> {TokenName::var_tk}, set<Constants::TokenType>(), set<TokenName> {TokenName::begin_tk, TokenName::point_tk}))
				return;
		}

		// � ��� ������, ���� var ������
		GetToken();
		do
		{
			// �������� ���������� ����������
			VarDeclaration();
			AcceptOper(TokenName::semicolon_tk);
		} while (curToken->GetType() == TokenType::Identifier);
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� CompoundStatement
		unique_ptr<Error> error = make_unique<Error>("������ � ������� ����������. " + (string)ex.what(), lineNumber, startPos, startPos);
		SyntacticErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenName>(), set<Constants::TokenType>(), set<TokenName> {TokenName::begin_tk});
	}
}

// �������� ���������� ����������
void VarDeclaration()
{
	try
	{
		if (curToken == NULL)
			return;

		// ���������� ������� ��� ����, ����� ����� ������������� (���)
		if (curToken->GetType() != TokenType::Identifier)
		{
			int startPos = curToken->GetStartPosition();
			unique_ptr<Error> error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), startPos, startPos);
			SyntacticErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenName>(), set<Constants::TokenType> {TokenType::Identifier}, set<TokenName> {TokenName::semicolon_tk}))
				return;
		}

		// ���
		AcceptIdent();
		while (IsThisOperatorName(TokenName::comma_tk))
		{
			GetToken();
			// ���
			AcceptIdent();
		}
		AcceptOper(TokenName::colon_tk);
		// ��� (������� ���)
		AcceptOper(vector<TokenName>{TokenName::integer_tk, TokenName::double_tk, TokenName::char_tk, TokenName::string_tk, TokenName::boolean_tk});
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� ����� � �������
		unique_ptr<Error> error = make_unique<Error>("������ � ������� �������� ���������� ����������. " + (string)ex.what(), lineNumber, startPos, startPos);
		SyntacticErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenName>(), set<Constants::TokenType>(), set<TokenName> {TokenName::semicolon_tk});
	}
}

// ��������� ��������
void CompoundStatement()
{
	try
	{
		if (curToken == NULL)
			return;

		// ���������� ������� ��� ����, ����� ����� begin
		if (!IsThisOperatorName(TokenName::begin_tk))
		{
			int startPos = curToken->GetStartPosition();
			unique_ptr<Error> error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), startPos, startPos);
			SyntacticErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenName> {TokenName::begin_tk}, set<Constants::TokenType>(), set<TokenName>{TokenName::point_tk}))
				return;
		}

		AcceptOper(TokenName::begin_tk);
		// ��������
		Statement();
		while (IsThisOperatorName(TokenName::semicolon_tk))
		{
			GetToken();
			// ��������
			Statement();
		}
		AcceptOper(TokenName::end_tk);
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� �����
		unique_ptr<Error> error = make_unique<Error>("������ � ������� \"��������� ��������\". " + (string)ex.what(), lineNumber, startPos, startPos);
		SyntacticErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenName>(), set<Constants::TokenType>(), set<TokenName> {TokenName::point_tk});
	}
}

// ��������
void Statement()
{
	try
	{
		if (curToken == NULL)
			return;

		// ������ ��������
		if (IsThisOperatorName(TokenName::end_tk))
			return;

		// ���������� ������� ��� ����, ����� ����� ������������� (����������) ��� �������� begin
		if (curToken->GetType() != TokenType::Identifier && !IsThisOperatorName(TokenName::begin_tk))
		{
			int startPos = curToken->GetStartPosition();
			unique_ptr<Error> error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), startPos, startPos);
			SyntacticErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenName> {TokenName::begin_tk}, set<Constants::TokenType> {TokenType::Identifier}, set<TokenName> {TokenName::semicolon_tk, TokenName::end_tk}))
				return;
		}

		// ������� �������� (�������� ������������)
		if (curToken->GetType() == TokenType::Identifier) // ����������
		{
			GetToken();
			AcceptOper(TokenName::assignation_tk);
			// ��������� (������� ���������)
			SimpleExpression();
		}
		// ������� ��������
		else
		{
			// ��������� ��������
			CompoundStatement();
		}
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� ����� � ������� ��� end
		unique_ptr<Error> error = make_unique<Error>("������ � ������� \"��������\". " + (string)ex.what(), lineNumber, startPos, startPos);
		SyntacticErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenName>(), set<Constants::TokenType>(), set<TokenName> {TokenName::semicolon_tk, TokenName::end_tk});
	}
}

// ������� ���������
void SimpleExpression()
{
	if (curToken == NULL)
		return;

	// ���������� ������� ��� ����, ����� ����� +, - ��� ���������
	auto type = curToken->GetType();
	if (!IsThisOperatorName(TokenName::plus_tk) && !IsThisOperatorName(TokenName::minus_tk) && !IsThisOperatorName(TokenName::round_open_bracket_tk) && type != TokenType::Identifier && type != TokenType::Value)
	{
		int startPos = curToken->GetStartPosition();
		unique_ptr<Error> error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), startPos, startPos);
		SyntacticErrors.push_back(move(error));
		if (!SkipToVariousTokens(set<TokenName> {TokenName::plus_tk, TokenName::minus_tk, TokenName::round_open_bracket_tk}, set<Constants::TokenType> {TokenType::Identifier, TokenType::Value}, set<TokenName> {TokenName::semicolon_tk, TokenName::end_tk}))
			return;
	}

	// ���� ��� ��� �����
	if (IsThisOperatorName(TokenName::plus_tk) || IsThisOperatorName(TokenName::minus_tk))
		GetToken();

	// ���������
	Item();
	// ���������� ��������
	while (IsThisOperatorName(TokenName::plus_tk) || IsThisOperatorName(TokenName::minus_tk))
	{
		GetToken();
		Item();
	}
}

// ���������
void Item()
{
	if (curToken == NULL)
		return;

	// ���������
	Multiplier();
	// ����������������� ��������
	while (IsThisOperatorName(TokenName::mult_tk) || IsThisOperatorName(TokenName::div_tk))
	{
		GetToken();
		// ���������
		Multiplier();
	}
}

// ���������
void Multiplier()
{
	try
	{
		if (curToken == NULL)
			return;

		auto tokenType = curToken->GetType();
		// ���������� ��������� ��� �����
		if (tokenType == TokenType::Identifier || tokenType == TokenType::Value)
		{
			GetToken();
			return;
		}
		else
		{
			// (��������� (������� ���������))
			AcceptOper(TokenName::round_open_bracket_tk);
			SimpleExpression();
			AcceptOper(TokenName::round_close_bracket_tk);
		}
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� ���������, �������, �����, ������, end, (, ����� � �������
		unique_ptr<Error> error = make_unique<Error>("������ � ���������. " + (string)ex.what(), lineNumber, startPos, startPos);
		SyntacticErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenName>(), set<Constants::TokenType>(), set<TokenName> {TokenName::mult_tk, TokenName::div_tk,
			TokenName::plus_tk, TokenName::minus_tk, TokenName::end_tk, TokenName::round_open_bracket_tk, TokenName::semicolon_tk});
	}
}

// �������� ����� ��������� (�������� ���)
bool IsThisOperatorName(TokenName tName)
{
	if (curToken->GetType() == TokenType::Operator && ((OperatorToken*)curToken.get())->GetName() == tName)
		return true;
	else
		return false;
}

// �������� ���������� ���� ���������� (�������� ���)
bool IsThisOperatorName(set<TokenName> tNames)
{
	if (curToken->GetType() == TokenType::Operator)
	{
		auto name = ((OperatorToken*)curToken.get())->GetName();
		if (tNames.find(name) != tNames.end())
			return true;
		else
			return false;
	}
}

// ������ �������� �� ������������ ������� ��� �� ����� (��������� ������� �������)
bool SkipToVariousTokens(set<TokenName> sNames, set<Constants::TokenType> sTypes, set<TokenName> fNames)
{
	while (true)
	{
		if (curToken == NULL)
			return false;

		if (IsThisOperatorName(sNames))
			return true;

		if (sTypes.size() != 0 && sTypes.find(curToken->GetType()) == sTypes.end())
			return true;

		if (IsThisOperatorName(fNames))
			return false;

		GetToken();
	}
}

// �������� ��������� �������� ���������
string GetKeyByValue(TokenName tName)
{
	string key = "";
	for (auto& item : KeyTokenName)
	{
		if (item.second == tName)
		{
			key = item.first;
			break;
		}
	}
	return key;
}

void GetToken()
{
	curToken = lexer.GetToken();
	if (curToken != NULL)
	{
		lineNumber = curToken->GetLineNumber();
		startPos = curToken->GetStartPosition();
	}
}

//////////////////////////////////////////////

int main()
{
	auto t = KeyTokenName;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	thread th(&Lexer::Start, &lexer);
	th.detach();

	/*
	while (true)
	{
		auto curToken = lexer.GetToken();
		if (curToken == NULL)
			break;

		switch (curToken->GetType())
		{
			case TokenType::Identifier:
				cout << "Identifier: " << ((IdentifierToken*)curToken.get())->GetName() << endl;
				break;

			case TokenType::Operator:
				for (auto it = KeyTokenName.begin(); it != KeyTokenName.end(); it++)
				{
					if (it->second == ((OperatorToken*)curToken.get())->GetName())
					{
						cout << "Operator: " << it->first << endl;
						break;
					}
				}
				break;

			case TokenType::Value:
				cout << "Value: " << ((ValueToken*)curToken.get())->GetValue() << endl;
				break;
		}
	}
	*/

	GetToken();
	Program();

	// ����� ����������� ������
	auto lexicalErrors = lexer.GetLexicalErrors();
	int errorsCount = lexicalErrors.size();
	for (int i = 0; i < errorsCount; i++)
	{
		lexicalErrors[i]->ShowError();
	}

	// ����� �������������� ������
	errorsCount = SyntacticErrors.size();
	for (int i = 0; i < errorsCount; i++)
	{
		SyntacticErrors[i]->ShowError();
	}

	return 0;
}
