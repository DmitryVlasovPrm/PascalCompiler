#include "Lexer.h"
#include <vector>

string file = "semantic.txt";
Lexer lexer = Lexer(file);
unique_ptr<Token> curToken;

void AcceptIdent()
{
	if (curToken->GetType() == TokenType::Identifier)
	{
		curToken = lexer.GetToken();
		return;
	}
	else
	{
		throw invalid_argument("Error");
	}
}

void AcceptOper(TokenName tokenName)
{
	if (curToken->GetType() == TokenType::Operator && ((OperatorToken*)curToken.get())->GetName() == tokenName)
	{
		curToken = lexer.GetToken();
		return;
	}
	else
	{
		throw invalid_argument("Error");
	}
}

void AcceptOper(vector<TokenName> tokenNames)
{
	if (curToken->GetType() == TokenType::Operator)
	{
		for (TokenName name : tokenNames)
		{
			if (((OperatorToken*)curToken.get())->GetName() == name)
			{
				curToken = lexer.GetToken();
				return;
			}
		}
	}

	throw invalid_argument("Error");
}

// Предопределение функций
void Program();
	void Block();
		void VarPart();
			void VarDeclaration();
		void CompoundStatement();
			void Statement();
				void SimpleExpression();
					void Item();
					void Multiplier();

// Программа
void Program()
{
	AcceptOper(TokenName::program_tk);
	// Имя
	AcceptIdent();
	AcceptOper(TokenName::semicolon_tk);
	// Блок
	Block();
	AcceptOper(TokenName::point_tk);
}

// Блок
void Block()
{
	// Раздел переменных
	VarPart();
	// Раздел операторов (составной оператор)
	CompoundStatement();
}

// Раздел переменных
void VarPart()
{
	if (curToken->GetType() == TokenType::Operator && ((OperatorToken*)curToken.get())->GetName() == TokenName::var_tk)
	{
		curToken = lexer.GetToken();
		do
		{
			// Описание однотипных переменных
			VarDeclaration();
			AcceptOper(TokenName::semicolon_tk);
		}
		while (curToken->GetType() == TokenType::Identifier);
	}
}

// Описание однотипных переменных
void VarDeclaration()
{
	// Имя
	AcceptIdent();
	while (curToken->GetType() == TokenType::Operator && ((OperatorToken*)curToken.get())->GetName() == TokenName::comma_tk)
	{
		curToken = lexer.GetToken();
		// Имя
		AcceptIdent();
	}
	AcceptOper(TokenName::colon_tk);
	// Тип (простой тип)
	AcceptOper(vector<TokenName>{TokenName::integer_tk, TokenName::double_tk, TokenName::char_tk,
		TokenName::string_tk, TokenName::boolean_tk});
}

// Составной оператор
void CompoundStatement()
{
	AcceptOper(TokenName::begin_tk);
	// Оператор
	Statement();
	while (curToken->GetType() == TokenType::Operator && ((OperatorToken*)curToken.get())->GetName() == TokenName::semicolon_tk)
	{
		curToken = lexer.GetToken();
		// Оператор
		Statement();
	}
	AcceptOper(TokenName::end_tk);
}

// Оператор
void Statement()
{
	// Простой оператор (оператор присваивания)
	if (curToken->GetType() == TokenType::Identifier) // Переменная
	{
		curToken = lexer.GetToken();
		AcceptOper(TokenName::assignation_tk);
		// Выражение (простое выражение)
		SimpleExpression();
	}
	// Сложный оператор
	else
	{
		// Составной оператор
		CompoundStatement();
	}
}

// Простое выражение
void SimpleExpression()
{
	// Знак или без знака
	if (curToken->GetType() == TokenType::Operator && (((OperatorToken*)curToken.get())->GetName() == TokenName::plus_tk ||
		((OperatorToken*)curToken.get())->GetName() == TokenName::minus_tk))
	{
		curToken = lexer.GetToken();
	}

	// Слагаемое
	Item();
	while (curToken->GetType() == TokenType::Operator &&
		(((OperatorToken*)curToken.get())->GetName() == TokenName::plus_tk ||   // Аддитивная операция
			((OperatorToken*)curToken.get())->GetName() == TokenName::minus_tk))
	{
		curToken = lexer.GetToken();
		Item();
	}
}

// Слагаемое
void Item()
{
	// Множитель
	Multiplier();
	while (curToken->GetType() == TokenType::Operator &&
		(((OperatorToken*)curToken.get())->GetName() == TokenName::mult_tk ||   // Мультипликативная операция
			((OperatorToken*)curToken.get())->GetName() == TokenName::div_tk))
	{
		curToken = lexer.GetToken();
		// Множитель
		Multiplier();
	}
}

// Множитель
void Multiplier()
{
	auto tokenType = curToken->GetType();
	// Переменная константа без знака
	if (tokenType == TokenType::Identifier || tokenType == TokenType::Value)
	{
		curToken = lexer.GetToken();
		return;
	}
	else
	{
		// (Выражение (простое выражение))
		AcceptOper(TokenName::round_open_bracket_tk);
		SimpleExpression();
		AcceptOper(TokenName::round_close_bracket_tk);
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

	curToken = lexer.GetToken();
	Program();

	auto lexicalErrors = lexer.GetLexicalErrors();
	int errorsCount = lexicalErrors.size();
	for (int i = 0; i < errorsCount; i++)
	{
		lexicalErrors[i]->ShowError();
	}

	return 0;
}
