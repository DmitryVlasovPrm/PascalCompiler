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
		throw invalid_argument("Ожидался идентификатор.");
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
		throw invalid_argument("Ожидался оператор: \"" + GetKeyByValue(tokenValue) + "\".");
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
	throw invalid_argument("Ожидался один из операторов: " + operatorsStr + ".");
}

// Программа
void Program()
{
	try
	{
		if (curToken == NULL)
			return;

		// Пропускаем символы для того, чтобы найти program
		if (!IsThisOperatorValue(TokenValue::program_tk))
		{
			auto error = make_unique<Error>("Ошибка: недопустимые токены.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue> {TokenValue::program_tk}, set<Constants::TokenType>(), set<TokenValue> {TokenValue::type_tk, TokenValue::var_tk, TokenValue::begin_tk}))
				goto block;
		}
		AcceptOper(TokenValue::program_tk);
		// Имя
		AcceptIdent();
		AcceptOper(TokenValue::semicolon_tk);
		// Блок
	block:
		Block();
		AcceptOper(TokenValue::point_tk);
	}
	catch (const invalid_argument& ex)
	{
		// Пропустить символы до CompoundStatement
		auto error = make_unique<Error>("Ошибка в разделе \"Программа\". " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
	}
}

// Блок
void Block()
{
	// Раздел типов
	TypePart();
	// Раздел переменных
	VarPart();
	// Раздел операторов (составной оператор)
	CompoundStatement();
}

// Раздел типов
void TypePart()
{
	try
	{
		if (curToken == NULL)
			return;

		// Если нет данного раздела
		if (IsThisOperatorValue(TokenValue::var_tk))
			return;

		// Пропускаем символы для того, чтобы найти type
		if (!IsThisOperatorValue(TokenValue::type_tk))
		{
			auto error = make_unique<Error>("Ошибка: недопустимые токены.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue> {TokenValue::type_tk}, set<Constants::TokenType>(), set<TokenValue> {TokenValue::var_tk, TokenValue::begin_tk}))
				return;
		}

		// В том случае, если type найден
		GetNextToken();
		do
		{
			// Определение типа
			TypeDeclaration();
			AcceptOper(TokenValue::semicolon_tk);
		} while (curToken != NULL && curToken->GetTokenType() == TokenType::Identifier);
	}
	catch (const invalid_argument& ex)
	{
		// Пропустить символы до Раздела переменных или Составного оператора
		auto error = make_unique<Error>("Ошибка в разделе типов. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::var_tk, TokenValue::begin_tk});
	}
}

// Определение типа
void TypeDeclaration()
{
	try
	{
		if (curToken == NULL)
			return;

		// Пропускаем символы для того, чтобы найти идентификатор (имя)
		if (curToken->GetTokenType() != TokenType::Identifier)
		{
			auto error = make_unique<Error>("Ошибка: недопустимые токены.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType> {TokenType::Identifier}, set<TokenValue> {TokenValue::semicolon_tk}))
				return;
		}

		// Имя
		auto customTypeName = AcceptIdent();
		AcceptOper(TokenValue::equal_tk);
		// Тип
		auto type = AcceptType();
		CustomTypes.insert(pair<IdentifierToken, TokenValue>(customTypeName, type));
	}
	catch (const invalid_argument& ex)
	{
		// Пропустить символы до точки с запятой
		auto error = make_unique<Error>("Ошибка в разделе определения типа. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::semicolon_tk});
	}
}

// Раздел переменных
void VarPart()
{
	try
	{
		if (curToken == NULL)
			return;

		// Если нет данного раздела
		if (IsThisOperatorValue(TokenValue::begin_tk))
			return;

		// Пропускаем символы для того, чтобы найти var
		if (!IsThisOperatorValue(TokenValue::var_tk))
		{
			auto error = make_unique<Error>("Ошибка: недопустимые токены.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue> {TokenValue::var_tk}, set<Constants::TokenType>(), set<TokenValue> {TokenValue::begin_tk}))
				return;
		}

		// В том случае, если var найден
		GetNextToken();
		do
		{
			// Описание однотипных переменных
			VarDeclaration();
			AcceptOper(TokenValue::semicolon_tk);
		} while (curToken != NULL && curToken->GetTokenType() == TokenType::Identifier);
	}
	catch (const invalid_argument& ex)
	{
		// Пропустить символы до Составного оператора
		auto error = make_unique<Error>("Ошибка в разделе переменных. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::begin_tk});
	}
}

// Описание однотипных переменных
void VarDeclaration()
{
	try
	{
		if (curToken == NULL)
			return;

		// Пропускаем символы для того, чтобы найти идентификатор (имя)
		if (curToken->GetTokenType() != TokenType::Identifier)
		{
			auto error = make_unique<Error>("Ошибка: недопустимые токены.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType> {TokenType::Identifier}, set<TokenValue> {TokenValue::semicolon_tk}))
				return;
		}

		// Имя
		auto var = AcceptIdent();
		vector<IdentifierToken> varNames = { var };
		while (IsThisOperatorValue(TokenValue::comma_tk))
		{
			GetNextToken();
			// Имя
			var = AcceptIdent();
			varNames.push_back(var);
		}
		AcceptOper(TokenValue::colon_tk);
		// Тип (простой тип)
		auto type = AcceptType();

		// Запоминаем определенные переменные и их типы
		while (!varNames.empty())
		{
			auto curVar = varNames.back();
			varNames.pop_back();

			curVar.SetValueType(type);
			if (DefinedVariables.find(curVar) != DefinedVariables.end())
			{
				auto error = make_unique<Error>("Переменная \"" + curVar.GetName() + "\" уже определена.", curVar.GetLineNumber(), curVar.GetStartPosition());
				SynSemErrors.push_back(move(error));
				continue;
			}
			DefinedVariables.insert(curVar);
		}
	}
	catch (const invalid_argument& ex)
	{
		// Пропустить символы до точки с запятой
		auto error = make_unique<Error>("Ошибка в разделе описания однотипных переменных. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::semicolon_tk});
	}
}

// Составной оператор
void CompoundStatement()
{
	try
	{
		if (curToken == NULL)
			return;

		// Пропускаем символы для того, чтобы найти begin
		if (!IsThisOperatorValue(TokenValue::begin_tk))
		{
			auto error = make_unique<Error>("Ошибка: недопустимые токены.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue> {TokenValue::begin_tk}, set<Constants::TokenType>(), set<TokenValue>{TokenValue::point_tk}))
				return;
		}

		AcceptOper(TokenValue::begin_tk);
		// Оператор
		Statement();
		while (IsThisOperatorValue(TokenValue::semicolon_tk))
		{
			GetNextToken();
			// Оператор
			Statement();
		}
		AcceptOper(TokenValue::end_tk);
	}
	catch (const invalid_argument& ex)
	{
		// Пропустить символы до точки
		auto error = make_unique<Error>("Ошибка в разделе \"Составной оператор\". " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::point_tk});
	}
}

// Оператор
void Statement()
{
	try
	{
		if (curToken == NULL)
			return;

		// Пустой оператор
		if (IsThisOperatorValue(TokenValue::end_tk))
			return;

		// Пропускаем символы для того, чтобы найти идентификатор (переменная) или begin, if, while
		if (curToken->GetTokenType() != TokenType::Identifier &&
			!IsThisOperatorValue(set<TokenValue> {TokenValue::begin_tk, TokenValue::if_tk, TokenValue::while_tk}))
		{
			auto error = make_unique<Error>("Ошибка: недопустимые токены.", curToken->GetLineNumber(), curToken->GetStartPosition());
			SynSemErrors.push_back(move(error));
			if (!SkipToVariousTokens(set<TokenValue> {TokenValue::begin_tk, TokenValue::if_tk, TokenValue::while_tk},
				set<Constants::TokenType> {TokenType::Identifier}, set<TokenValue> {TokenValue::semicolon_tk, TokenValue::end_tk}))
				return;
		}

		// Простой оператор (оператор присваивания)
		if (curToken->GetTokenType() == TokenType::Identifier) // Переменная
		{
			auto leftType = CheckIdentifier();
			GetNextToken();
			auto operation = *curToken;
			AcceptOper(TokenValue::assignation_tk);
			// Выражение
			auto rightType = Expression();
			CompareTypes(leftType, rightType, operation);
		}

		// Условие if, цикл while, сложный оператор
		if (curToken->GetTokenType() == TokenType::Operator)
		{
			// Составной оператор
			if (curToken->GetValueType() == TokenValue::begin_tk)
				CompoundStatement();
			// Условный оператор
			if (curToken->GetValueType() == TokenValue::if_tk)
				IfConditional();
			// Цикл while
			if (curToken->GetValueType() == TokenValue::while_tk)
				WhileCycle();
		}
	}
	catch (const invalid_argument& ex)
	{
		// Пропустить символы до точки с запятой или end
		auto error = make_unique<Error>("Ошибка в разделе \"Оператор\". " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::semicolon_tk, TokenValue::end_tk});
	}
}

// Выражение
TokenValue Expression()
{
	// Простое выражение
	auto leftType = SimpleExpression();
	// Операция отношения
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

// Простое выражение
TokenValue SimpleExpression()
{
	auto curType = TokenValue::none_tk;
	if (curToken == NULL)
		return curType;

	// Пропускаем символы для того, чтобы найти +, - или слагаемое
	auto type = curToken->GetTokenType();
	if (!IsThisOperatorValue(set<TokenValue> {TokenValue::plus_tk, TokenValue::minus_tk, TokenValue::round_open_bracket_tk,
		TokenValue::not_tk}) && type != TokenType::Identifier && type != TokenType::Value)
	{
		auto error = make_unique<Error>("Ошибка: недопустимые токены.", curToken->GetLineNumber(), curToken->GetStartPosition());
		SynSemErrors.push_back(move(error));
		if (!SkipToVariousTokens(set<TokenValue> {TokenValue::plus_tk, TokenValue::minus_tk, TokenValue::round_open_bracket_tk,
			TokenValue::not_tk}, set<Constants::TokenType> {TokenType::Identifier, TokenType::Value}, set<TokenValue> {TokenValue::semicolon_tk, TokenValue::end_tk}))
			return curType;
	}

	// Знак или без знака
	if (IsThisOperatorValue(set<TokenValue> {TokenValue::plus_tk, TokenValue::minus_tk}))
	{
		GetNextToken();
	}

	// Слагаемое
	curType = Item();
	// Аддитивная операция
	while (IsThisOperatorValue(set<TokenValue> {TokenValue::plus_tk, TokenValue::minus_tk, TokenValue::or_tk}))
	{
		auto operation = *curToken;
		GetNextToken();
		auto nextType = Item();
		curType = CompareTypes(curType, nextType, operation);
	}
	return curType;
}

// Слагаемое
TokenValue Item()
{
	auto curType = TokenValue::none_tk;
	if (curToken == NULL)
		return curType;

	// Множитель
	curType = Multiplier();
	// Мультипликативная операция
	while (IsThisOperatorValue(set<TokenValue> {TokenValue::mult_tk, TokenValue::div_tk, TokenValue::int_div_tk,
		TokenValue::mod_tk, TokenValue::and_tk}))
	{
		auto operation = *curToken;
		GetNextToken();
		// Множитель
		auto nextType = Multiplier();
		curType = CompareTypes(curType, nextType, operation);
	}
	return curType;
}

// Множитель
TokenValue Multiplier()
{
	try
	{
		if (curToken == NULL)
			return TokenValue::none_tk;

		auto tokenType = curToken->GetTokenType();
		// Переменная
		if (tokenType == TokenType::Identifier)
		{
			auto type = CheckIdentifier();
			GetNextToken();
			return type;
		}
		// Константа без знака
		if (tokenType == TokenType::Value)
		{
			auto type = curToken->GetValueType();
			GetNextToken();
			return type;
		}
		// (<выражение>) или not <множитель>
		if (tokenType == TokenType::Operator)
		{
			if (curToken->GetValueType() == TokenValue::round_open_bracket_tk)
			{
				// Выражение
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
					auto error = make_unique<Error>("Несовместимость типов для операции \"not\".", notOper.GetLineNumber(), notOper.GetStartPosition());
					SynSemErrors.push_back(move(error));
					return TokenValue::none_tk;
				}
				return type;
			}
		}
	}
	catch (const invalid_argument& ex)
	{
		// Пропустить символы до умножения, деления, плюса, минуса, end, (, точка с запятой
		auto error = make_unique<Error>("Ошибка в множителе. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::mult_tk, TokenValue::div_tk,
			TokenValue::plus_tk, TokenValue::minus_tk, TokenValue::end_tk, TokenValue::round_open_bracket_tk, TokenValue::semicolon_tk});
	}
}

// Условный оператор
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
		// Пропустить символы до умножения, деления, плюса, минуса, end, (, точка с запятой
		auto error = make_unique<Error>("Ошибка в условном операторе. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::semicolon_tk, TokenValue::end_tk});
	}
}

// Цикл while
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
		// Пропустить символы до умножения, деления, плюса, минуса, end, (, точка с запятой
		auto error = make_unique<Error>("Ошибка в цикле с предусловием. " + (string)ex.what(), lineNumber, startPos);
		SynSemErrors.push_back(move(error));
		SkipToVariousTokens(set<TokenValue>(), set<Constants::TokenType>(), set<TokenValue> {TokenValue::semicolon_tk, TokenValue::end_tk});
	}
}

#pragma region Вспомогательные функции

// Получаем следующий токен
void GetNextToken()
{
	curToken = lexer.GetToken();
	if (curToken != NULL)
	{
		lineNumber = curToken->GetLineNumber();
		startPos = curToken->GetStartPosition();
	}
}

// Проверка имени оператора (упрощает код)
bool IsThisOperatorValue(TokenValue tValue)
{
	if (curToken != NULL && curToken->GetTokenType() == TokenType::Operator && curToken->GetValueType() == tValue)
		return true;
	else
		return false;
}

// Проверка нескольких имен операторов (упрощает код)
bool IsThisOperatorValue(set<TokenValue> tValues)
{
	if (curToken != NULL && curToken->GetTokenType() == TokenType::Operator && tValues.find(curToken->GetValueType()) != tValues.end())
		return true;
	else
		return false;
}

// Делаем пропуски до определенных токенов или их типов (учитываем внешние символы)
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

// Объявлен ли идентификатор и берем его тип
TokenValue CheckIdentifier()
{
	auto type = TokenValue::none_tk;
	auto ident = *(IdentifierToken*)(curToken.get());
	auto iterator = DefinedVariables.find(ident);
	if (iterator == DefinedVariables.end())
	{
		auto error = make_unique<Error>("Идентификатор \"" + ident.GetName() + "\" не определен.", lineNumber, startPos);
		SynSemErrors.push_back(move(error));
	}
	else
	{
		auto ident = *iterator;
		type = ident.GetValueType();
	}
	return type;
}

// Проверяем соответствие типов
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
			unique_ptr<Error> error = make_unique<Error>("Несовместимость типов. В правой части ожидался тип \"" + leftKey + "\", но был получен \"" + rightKey + "\".", operationToken.GetLineNumber(), operationToken.GetStartPosition());
			SynSemErrors.push_back(move(error));
		}
		return leftPart;
	}

	// Определение типов операции
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


	// Ранее была допущена ошибка -> тип определить невозможно
	if (leftPart == TokenValue::none_tk || rightPart == TokenValue::none_tk)
	{
		return TokenValue::none_tk;
	}

	// Целое - целое
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

	// Целое или вещественное
	if ((leftPart == TokenValue::integer_tk || leftPart == TokenValue::double_tk) &&
		(rightPart == TokenValue::integer_tk || rightPart == TokenValue::double_tk))
	{
		if (areRelationshipOperations)
			return TokenValue::boolean_tk;
		if (areArithmeticOperations)
			return TokenValue::double_tk;
	}

	// Конкатенация строк
	if ((leftPart == TokenValue::char_tk || leftPart == TokenValue::string_tk) &&
		(rightPart == TokenValue::char_tk || rightPart == TokenValue::string_tk))
	{
		if (areRelationshipOperations)
			return TokenValue::boolean_tk;
		if (operation == TokenValue::plus_tk)
			return TokenValue::string_tk;
	}

	// Логическая операция
	if (leftPart == TokenValue::boolean_tk && leftPart == rightPart)
	{
		if (areRelationshipOperations || areLogicalOperations)
			return TokenValue::boolean_tk;
	}

	// Если не нашлось верных вариантов, то выводим сообщение об ошибке
	string oper = GetKeyByValue(operation);
	auto error = make_unique<Error>("Несовместимость типов для операции \"" + oper + "\".", operationToken.GetLineNumber(), operationToken.GetStartPosition());
	SynSemErrors.push_back(move(error));
	return TokenValue::none_tk;
}

// Получаем строковое значение оператора
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

	// Вывод токенов
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

	// Вывод лексических ошибок
	auto lexicalErrors = lexer.GetLexicalErrors();
	int errorsCount = lexicalErrors.size();
	for (int i = 0; i < errorsCount; i++)
	{
		lexicalErrors[i]->ShowError();
	}

	// Вывод синтаксических и семантических ошибок
	errorsCount = SynSemErrors.size();
	for (int i = 0; i < errorsCount; i++)
	{
		SynSemErrors[i]->ShowError();
	}

	return 0;
}
