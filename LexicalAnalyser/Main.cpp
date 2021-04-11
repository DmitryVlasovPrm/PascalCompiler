#include "Main.h"

string file = "semantic.txt";
Lexer lexer = Lexer(file);
unique_ptr<Token> curToken;
int lineNumber = 0, startPos = 0;
vector<unique_ptr<Error>> SynSemErrors;
map<IdentifierToken, TokenValue> CustomTypes;
set<IdentifierToken> DefinedVariables;

IdentifierToken AcceptIdent()
{
	if (curToken != NULL && curToken->GetTokenType() == TokenType::Identifier)
	{
		auto ident = *(IdentifierToken*)curToken.get();
		GetNextToken();
		return ident;
	}
	else
	{
		throw invalid_argument("�������� �������������.");
	}
}

void AcceptOper(TokenValue tokenValue)
{
	if (IsThisOperatorValue(tokenValue))
	{
		GetNextToken();
		return;
	}
	else
	{
		throw invalid_argument("�������� ��������: \"" + GetKeyByValue(tokenValue) + "\".");
	}
}

TokenValue AcceptType()
{
	auto tokenType = curToken->GetTokenType();
	if (curToken != NULL && tokenType == TokenType::Operator)
	{
		auto it = StandardTypes.find(curToken->GetValueType());
		if (it != StandardTypes.end())
		{
			GetNextToken();
			return *it;
		}
	}

	if (curToken != NULL && tokenType == TokenType::Identifier)
	{
		auto it = CustomTypes.find(*(IdentifierToken*)curToken.get());
		if (it != CustomTypes.end())
		{
			GetNextToken();
			return (*it).second;
		}
	}

	string operatorsStr = StandardTypesStr;
	for (auto it = CustomTypes.begin(); it != CustomTypes.end(); it++)
	{
		operatorsStr += ", ";
		if (next(it) == CustomTypes.end())
			operatorsStr += "\"" + ((IdentifierToken)(*it).first).GetName() + "\"";
		else
			operatorsStr += "\"" + ((IdentifierToken)(*it).first).GetName() + "\", ";
	}
	throw invalid_argument("�������� ���� �� ����������: " + operatorsStr + ".");
}

// ���������
void Program()
{
	try
	{
		if (curToken == NULL)
			return;

		// ���������� ������� ��� ����, ����� ����� program
		if (!IsThisOperatorValue(TokenValue::program_tk))
		{
			auto error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue> {TokenValue::program_tk}, set<Constants::TokenType>(), set<TokenValue> {TokenValue::type_tk, TokenValue::var_tk, TokenValue::begin_tk}))
				goto block;
		}
		AcceptOper(TokenValue::program_tk);
		// ���
		AcceptIdent();
		AcceptOper(TokenValue::semicolon_tk);
		// ����
	block:
		Block();
		AcceptOper(TokenValue::point_tk);
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� CompoundStatement
		auto error = make_unique<Error>("������ � ������� \"���������\". " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
	}
}

// ����
void Block()
{
	// ������ �����
	TypePart();
	// ������ ����������
	VarPart();
	// ������ ���������� (��������� ��������)
	CompoundStatement();
}

// ������ �����
void TypePart()
{
	try
	{
		if (curToken == NULL)
			return;

		// ���� ��� ������� �������
		if (IsThisOperatorValue(TokenValue::var_tk))
			return;

		// ���������� ������� ��� ����, ����� ����� type
		if (!IsThisOperatorValue(TokenValue::type_tk))
		{
			auto error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue> {TokenValue::type_tk}, set<Constants::TokenType>(), set<TokenValue> {TokenValue::var_tk, TokenValue::begin_tk}))
				return;
		}

		// � ��� ������, ���� type ������
		GetNextToken();
		do
		{
			// ����������� ����
			TypeDeclaration();
			AcceptOper(TokenValue::semicolon_tk);
		} while (curToken != NULL && curToken->GetTokenType() == TokenType::Identifier);
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� ������� ���������� ��� ���������� ���������
		auto error = make_unique<Error>("������ � ������� �����. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::var_tk, TokenValue::begin_tk});
	}
}

// ����������� ����
void TypeDeclaration()
{
	try
	{
		if (curToken == NULL)
			return;

		// ���������� ������� ��� ����, ����� ����� ������������� (���)
		if (curToken->GetTokenType() != TokenType::Identifier)
		{
			auto error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType> {TokenType::Identifier}, set<TokenValue> {TokenValue::semicolon_tk}))
				return;
		}

		// ���
		auto customTypeName = AcceptIdent();
		AcceptOper(TokenValue::equal_tk);
		// ���
		auto type = AcceptType();
		CustomTypes.insert(pair<IdentifierToken, TokenValue>(customTypeName, type));
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� ����� � �������
		auto error = make_unique<Error>("������ � ������� ����������� ����. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::semicolon_tk});
	}
}

// ������ ����������
void VarPart()
{
	try
	{
		if (curToken == NULL)
			return;

		// ���� ��� ������� �������
		if (IsThisOperatorValue(TokenValue::begin_tk))
			return;

		// ���������� ������� ��� ����, ����� ����� var
		if (!IsThisOperatorValue(TokenValue::var_tk))
		{
			auto error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue> {TokenValue::var_tk}, set<Constants::TokenType>(), set<TokenValue> {TokenValue::begin_tk}))
				return;
		}

		// � ��� ������, ���� var ������
		GetNextToken();
		do
		{
			// �������� ���������� ����������
			VarDeclaration();
			AcceptOper(TokenValue::semicolon_tk);
		} while (curToken != NULL && curToken->GetTokenType() == TokenType::Identifier);
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� ���������� ���������
		auto error = make_unique<Error>("������ � ������� ����������. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::begin_tk});
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
		if (curToken->GetTokenType() != TokenType::Identifier)
		{
			auto error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType> {TokenType::Identifier}, set<TokenValue> {TokenValue::semicolon_tk}))
				return;
		}

		// ���
		auto var = AcceptIdent();
		vector<IdentifierToken> varNames = { var };
		while (IsThisOperatorValue(TokenValue::comma_tk))
		{
			GetNextToken();
			// ���
			var = AcceptIdent();
			varNames.push_back(var);
		}
		AcceptOper(TokenValue::colon_tk);
		// ��� (������� ���)
		auto type = AcceptType();

		// ���������� ������������ ���������� � �� ����
		while (!varNames.empty())
		{
			auto curVar = varNames.back();
			varNames.pop_back();

			curVar.SetValueType(type);
			if (DefinedVariables.find(curVar) != DefinedVariables.end())
			{
				auto error = make_unique<Error>("���������� \"" + curVar.GetName() + "\" ��� ����������.", curVar.GetLineNumber(), curVar.GetStartPosition());
				SynSemErrors.push_back(move(error));
				continue;
			}
			DefinedVariables.insert(curVar);
		}
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� ����� � �������
		auto error = make_unique<Error>("������ � ������� �������� ���������� ����������. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::semicolon_tk});
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
		if (!IsThisOperatorValue(TokenValue::begin_tk))
		{
			auto error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue> {TokenValue::begin_tk}, set<Constants::TokenType>(), set<TokenValue>{TokenValue::point_tk}))
				return;
		}

		AcceptOper(TokenValue::begin_tk);
		// ��������
		Statement();
		while (IsThisOperatorValue(TokenValue::semicolon_tk))
		{
			GetNextToken();
			// ��������
			Statement();
		}
		AcceptOper(TokenValue::end_tk);
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� �����
		auto error = make_unique<Error>("������ � ������� \"��������� ��������\". " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::point_tk});
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
		if (IsThisOperatorValue(TokenValue::end_tk))
			return;

		// ���������� ������� ��� ����, ����� ����� ������������� (����������) ��� begin, if, while
		if (curToken->GetTokenType() != TokenType::Identifier &&
			!IsThisOperatorValue(set<TokenValue> {TokenValue::begin_tk, TokenValue::if_tk, TokenValue::while_tk}))
		{
			auto error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue> {TokenValue::begin_tk, TokenValue::if_tk, TokenValue::while_tk},
				set<Constants::TokenType> {TokenType::Identifier}, set<TokenValue> {TokenValue::semicolon_tk, TokenValue::end_tk}))
				return;
		}

		// ������� �������� (�������� ������������)
		if (curToken->GetTokenType() == TokenType::Identifier) // ����������
		{
			auto leftType = CheckIdentifier();
			GetNextToken();
			auto operation = *curToken;
			AcceptOper(TokenValue::assignation_tk);
			// ���������
			auto rightType = Expression();
			CompareTypes(leftType, rightType, operation);
		}

		// ������� if, ���� while, ������� ��������
		if (curToken->GetTokenType() == TokenType::Operator)
		{
			// ��������� ��������
			if (curToken->GetValueType() == TokenValue::begin_tk)
				CompoundStatement();
			// �������� ��������
			if (curToken->GetValueType() == TokenValue::if_tk)
				IfConditional();
			// ���� while
			if (curToken->GetValueType() == TokenValue::while_tk)
				WhileCycle();
		}
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� ����� � ������� ��� end
		auto error = make_unique<Error>("������ � ������� \"��������\". " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::semicolon_tk, TokenValue::end_tk});
	}
}

// ���������
TokenValue Expression()
{
	// ������� ���������
	auto leftType = SimpleExpression();
	// �������� ���������
	if (IsThisOperatorValue(set<TokenValue> {TokenValue::equal_tk, TokenValue::not_equal_tk,
		TokenValue::less_tk, TokenValue::less_equal_tk, TokenValue::bigger_tk, TokenValue::bigger_equal_tk}))
	{
		auto operation = *curToken;
		GetNextToken();
		auto rightType = SimpleExpression();
		return CompareTypes(leftType, rightType, operation);
	}
	return leftType;
}

// ������� ���������
TokenValue SimpleExpression()
{
	auto curType = TokenValue::none_tk;
	if (curToken == NULL)
		return curType;

	// ���������� ������� ��� ����, ����� ����� +, - ��� ���������
	auto type = curToken->GetTokenType();
	if (!IsThisOperatorValue(set<TokenValue> {TokenValue::plus_tk, TokenValue::minus_tk, TokenValue::round_open_bracket_tk,
		TokenValue::not_tk}) && type != TokenType::Identifier && type != TokenType::Value)
	{
		auto error = make_unique<Error>("������: ������������ ������.", curToken->GetLineNumber(), curToken->GetStartPosition());
		SynSemErrors.push_back(move(error));
		if (!SkipToVariousTokens(set<TokenValue> {TokenValue::plus_tk, TokenValue::minus_tk, TokenValue::round_open_bracket_tk,
			TokenValue::not_tk}, set<Constants::TokenType> {TokenType::Identifier, TokenType::Value}, set<TokenValue> {TokenValue::semicolon_tk, TokenValue::end_tk}))
			return curType;
	}

	// ���� ��� ��� �����
	if (IsThisOperatorValue(set<TokenValue> {TokenValue::plus_tk, TokenValue::minus_tk}))
	{
		GetNextToken();
	}

	// ���������
	curType = Item();
	// ���������� ��������
	while (IsThisOperatorValue(set<TokenValue> {TokenValue::plus_tk, TokenValue::minus_tk, TokenValue::or_tk}))
	{
		auto operation = *curToken;
		GetNextToken();
		auto nextType = Item();
		curType = CompareTypes(curType, nextType, operation);
	}
	return curType;
}

// ���������
TokenValue Item()
{
	auto curType = TokenValue::none_tk;
	if (curToken == NULL)
		return curType;

	// ���������
	curType = Multiplier();
	// ����������������� ��������
	while (IsThisOperatorValue(set<TokenValue> {TokenValue::mult_tk, TokenValue::div_tk, TokenValue::int_div_tk,
		TokenValue::mod_tk, TokenValue::and_tk}))
	{
		auto operation = *curToken;
		GetNextToken();
		// ���������
		auto nextType = Multiplier();
		curType = CompareTypes(curType, nextType, operation);
	}
	return curType;
}

// ���������
TokenValue Multiplier()
{
	try
	{
		if (curToken == NULL)
			return TokenValue::none_tk;

		auto tokenType = curToken->GetTokenType();
		// ����������
		if (tokenType == TokenType::Identifier)
		{
			auto type = CheckIdentifier();
			GetNextToken();
			return type;
		}
		// ��������� ��� �����
		if (tokenType == TokenType::Value)
		{
			auto type = curToken->GetValueType();
			GetNextToken();
			return type;
		}
		// (<���������>) ��� not <���������>
		if (tokenType == TokenType::Operator)
		{
			if (curToken->GetValueType() == TokenValue::round_open_bracket_tk)
			{
				// ���������
				GetNextToken();
				auto type = Expression();
				AcceptOper(TokenValue::round_close_bracket_tk);
				return type;
			}
			else
			{
				auto notOper = *curToken;
				AcceptOper(TokenValue::not_tk);
				auto type = Multiplier();
				if (type != TokenValue::boolean_tk)
				{
					auto error = make_unique<Error>("��������������� ����� ��� �������� \"not\".", notOper.GetLineNumber(), notOper.GetStartPosition());
					SynSemErrors.push_back(move(error));
					return TokenValue::none_tk;
				}
				return type;
			}
		}
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� ���������, �������, �����, ������, end, (, ����� � �������
		auto error = make_unique<Error>("������ � ���������. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::mult_tk, TokenValue::div_tk,
			TokenValue::plus_tk, TokenValue::minus_tk, TokenValue::end_tk, TokenValue::round_open_bracket_tk, TokenValue::semicolon_tk});
	}
}

// �������� ��������
void IfConditional()
{
	try
	{
		GetNextToken();
		Expression();
		AcceptOper(TokenValue::then_tk);
		Statement();
		if (curToken->GetValueType() == TokenValue::else_tk)
		{
			GetNextToken();
			Statement();
		}
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� ���������, �������, �����, ������, end, (, ����� � �������
		auto error = make_unique<Error>("������ � �������� ���������. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::semicolon_tk, TokenValue::end_tk});
	}
}

// ���� while
void WhileCycle()
{
	try
	{
		GetNextToken();
		Expression();
		AcceptOper(TokenValue::do_tk);
		Statement();
	}
	catch (const invalid_argument& ex)
	{
		// ���������� ������� �� ���������, �������, �����, ������, end, (, ����� � �������
		auto error = make_unique<Error>("������ � ����� � ������������. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::semicolon_tk, TokenValue::end_tk});
	}
}

#pragma region ��������������� �������

// �������� ��������� �����
void GetNextToken()
{
	curToken = lexer.GetToken();
	if (curToken != NULL)
	{
		lineNumber = curToken->GetLineNumber();
		startPos = curToken->GetStartPosition();
	}
}

// �������� ����� ��������� (�������� ���)
bool IsThisOperatorValue(TokenValue tValue)
{
	if (curToken != NULL && curToken->GetTokenType() == TokenType::Operator && curToken->GetValueType() == tValue)
		return true;
	else
		return false;
}

// �������� ���������� ���� ���������� (�������� ���)
bool IsThisOperatorValue(set<TokenValue> tValues)
{
	if (curToken != NULL && curToken->GetTokenType() == TokenType::Operator && tValues.find(curToken->GetValueType()) != tValues.end())
		return true;
	else
		return false;
}

// ������ �������� �� ������������ ������� ��� �� ����� (��������� ������� �������)
bool SkipToVariousTokens(set<TokenValue> sValues, set<Constants::TokenType> sTypes, set<TokenValue> fValues)
{
	while (true)
	{
		if (curToken == NULL)
			return false;

		if (IsThisOperatorValue(sValues))
			return true;

		if (sTypes.find(curToken->GetTokenType()) != sTypes.end())
			return true;

		if (IsThisOperatorValue(fValues))
			return false;

		GetNextToken();
	}
}

// �������� �� ������������� � ����� ��� ���
TokenValue CheckIdentifier()
{
	auto type = TokenValue::none_tk;
	auto ident = *(IdentifierToken*)(curToken.get());
	auto iterator = DefinedVariables.find(ident);
	if (iterator == DefinedVariables.end())
	{
		auto error = make_unique<Error>("������������� \"" + ident.GetName() + "\" �� ���������.", lineNumber, startPos);
		SynSemErrors.push_back(move(error));
	}
	else
	{
		auto ident = *iterator;
		type = ident.GetValueType();
	}
	return type;
}

// ��������� ������������ �����
TokenValue CompareTypes(TokenValue leftPart, TokenValue rightPart, Token operationToken)
{
	auto operation = operationToken.GetValueType();
	if (operation == TokenValue::assignation_tk)
	{
		if (leftPart != TokenValue::none_tk && rightPart != TokenValue::none_tk && leftPart != rightPart)
		{
			if (leftPart == TokenValue::string_tk && rightPart == TokenValue::char_tk)
				return leftPart;
			string leftKey = GetKeyByValue(leftPart);
			string rightKey = GetKeyByValue(rightPart);
			unique_ptr<Error> error = make_unique<Error>("��������������� �����. � ������ ����� �������� ��� \"" + leftKey + "\", �� ��� ������� \"" + rightKey + "\".", operationToken.GetLineNumber(), operationToken.GetStartPosition());
			SynSemErrors.push_back(move(error));
		}
		return leftPart;
	}

	// ����������� ����� ��������
	bool areArithmeticOperations = false;
	bool areRelationshipOperations = false;
	bool areLogicalOperations = false;
	if (operation == TokenValue::plus_tk || operation == TokenValue::minus_tk || operation == TokenValue::mult_tk ||
		operation == TokenValue::div_tk || operation == TokenValue::int_div_tk || operation == TokenValue::mod_tk)
		areArithmeticOperations = true;
	else if (operation == TokenValue::equal_tk || operation == TokenValue::not_equal_tk || operation == TokenValue::less_tk ||
		operation == TokenValue::less_equal_tk || operation == TokenValue::bigger_tk || operation == TokenValue::bigger_equal_tk)
		areRelationshipOperations = true;
	else if (operation == TokenValue::and_tk || operation == TokenValue::or_tk)
		areLogicalOperations = true;


	// ����� ���� �������� ������ -> ��� ���������� ����������
	if (leftPart == TokenValue::none_tk || rightPart == TokenValue::none_tk)
	{
		return TokenValue::none_tk;
	}

	// ����� - �����
	if (leftPart == TokenValue::integer_tk && rightPart == TokenValue::integer_tk)
	{
		if (areRelationshipOperations)
			return TokenValue::boolean_tk;
		if (areArithmeticOperations)
		{
			if (operation == TokenValue::div_tk)
				return TokenValue::double_tk;
			else
				return TokenValue::integer_tk;
		}
	}

	// ����� ��� ������������
	if ((leftPart == TokenValue::integer_tk || leftPart == TokenValue::double_tk) &&
		(rightPart == TokenValue::integer_tk || rightPart == TokenValue::double_tk))
	{
		if (areRelationshipOperations)
			return TokenValue::boolean_tk;
		if (areArithmeticOperations)
			return TokenValue::double_tk;
	}

	// ������������ �����
	if ((leftPart == TokenValue::char_tk || leftPart == TokenValue::string_tk) &&
		(rightPart == TokenValue::char_tk || rightPart == TokenValue::string_tk))
	{
		if (areRelationshipOperations)
			return TokenValue::boolean_tk;
		if (operation == TokenValue::plus_tk)
			return TokenValue::string_tk;
	}

	// ���������� ��������
	if (leftPart == TokenValue::boolean_tk && leftPart == rightPart)
	{
		if (areRelationshipOperations || areLogicalOperations)
			return TokenValue::boolean_tk;
	}

	// ���� �� ������� ������ ���������, �� ������� ��������� �� ������
	string oper = GetKeyByValue(operation);
	auto error = make_unique<Error>("��������������� ����� ��� �������� \"" + oper + "\".", operationToken.GetLineNumber(), operationToken.GetStartPosition());
	SynSemErrors.push_back(move(error));
	return TokenValue::none_tk;
}

// �������� ��������� �������� ���������
string GetKeyByValue(TokenValue tValue)
{
	string key = "";
	for (auto& item : KeyTokenValue)
	{
		if (item.second == tValue)
		{
			key = item.first;
			break;
		}
	}
	return key;
}
#pragma endregion

////////////////////////////////////////////////////

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	thread th(&Lexer::Start, &lexer);
	th.detach();

	// ����� �������
	/*
	while (true)
	{
		auto curToken = lexer.GetToken();
		if (curToken == NULL)
			break;

		switch (curToken->GetTokenType())
		{
			case TokenType::Identifier:
				cout << "Identifier: " << ((IdentifierToken*)curToken.get())->GetName() << endl;
				break;

			case TokenType::Operator:
				for (auto it = KeyTokenValue.begin(); it != KeyTokenValue.end(); it++)
				{
					if (it->second == ((OperatorToken*)curToken.get())->GetValueType())
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

	GetNextToken();
	Program();

	// ����� ����������� ������
	auto lexicalErrors = lexer.GetLexicalErrors();
	int errorsCount = lexicalErrors.size();
	for (int i = 0; i < errorsCount; i++)
	{
		lexicalErrors[i]->ShowError();
	}

	// ����� �������������� � ������������� ������
	errorsCount = SynSemErrors.size();
	for (int i = 0; i < errorsCount; i++)
	{
		SynSemErrors[i]->ShowError();
	}

	return 0;
}
