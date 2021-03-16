#include "Lexer.h"
#include "Constants.h"

string file = "pascal.txt";

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	Lexer lexer = Lexer(file);
	thread th(&Lexer::Start, &lexer);
	th.detach();

	while (true)
	{
		Token* curToken = lexer.GetToken();
		if (curToken == NULL)
			break;

		cout << Constants::TokenTypeString[(int)curToken->GetType()] << ": " << curToken->GetValue() << endl;
		delete curToken;
	}

	auto lexicalErrors = lexer.GetLexicalErrors();
	int errorsCount = lexicalErrors.size();
	for (int i = 0; i < errorsCount; i++)
	{
		lexicalErrors[i].ShowError();
	}

	return 0;
}