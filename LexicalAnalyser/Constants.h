#pragma once

#include <map>
#include <string>
#include <set>
using namespace std;

namespace Constants
{
	enum class TokenValue
	{
		program_tk,
		var_tk,
		begin_tk,
		end_tk,
		function_tk,
		procedure_tk,
		read_tk,
		readln_tk,
		write_tk,
		writeln_tk,
		file_tk,

		none_tk,
		integer_tk,
		char_tk,
		string_tk,
		double_tk,
		boolean_tk,
		const_tk,
		type_tk,
		array_tk,
		case_tk,

		if_tk,
		do_tk,
		then_tk,
		else_tk,
		for_tk,
		while_tk,

		int_div_tk,
		div_tk,
		mod_tk,
		plus_tk,
		minus_tk,
		mult_tk,
		in_tk,
		of_tk,

		and_tk,
		not_tk,
		or_tk,
		equal_tk,
		less_tk,
		less_equal_tk,
		not_equal_tk,
		bigger_tk,
		bigger_equal_tk,

		colon_tk,
		assignation_tk,
		point_tk,
		points_tk,
		semicolon_tk,
		comma_tk,
		round_open_bracket_tk,
		round_close_bracket_tk,
		square_open_bracket_tk,
		square_close_bracket_tk
	};

	const map<string, TokenValue> KeyTokenValue =
	{
		{"program", TokenValue::program_tk},
		{"var", TokenValue::var_tk},
		{"begin", TokenValue::begin_tk},
		{"end", TokenValue::end_tk},
		{"function", TokenValue::function_tk},
		{"procedure", TokenValue::procedure_tk},
		{"read", TokenValue::read_tk},
		{"readln", TokenValue::readln_tk},
		{"write", TokenValue::write_tk},
		{"writeln", TokenValue::writeln_tk},
		{"file", TokenValue::file_tk},
		{"case", TokenValue::case_tk},

		{"integer", TokenValue::integer_tk},
		{"char", TokenValue::char_tk},
		{"string", TokenValue::string_tk},
		{"double", TokenValue::double_tk},
		{"boolean", TokenValue::boolean_tk},
		{"const", TokenValue::const_tk},
		{"type", TokenValue::type_tk},

		{"if", TokenValue::if_tk},
		{"do", TokenValue::do_tk},
		{"then", TokenValue::then_tk},
		{"else", TokenValue::else_tk},
		{"for", TokenValue::for_tk},
		{"while", TokenValue::while_tk},

		{"div", TokenValue::int_div_tk},
		{"/", TokenValue::div_tk},
		{"mod", TokenValue::mod_tk},
		{"+", TokenValue::plus_tk},
		{"-", TokenValue::minus_tk},
		{"*", TokenValue::mult_tk},

		{"and", TokenValue::and_tk},
		{"not", TokenValue::not_tk},
		{"or", TokenValue::or_tk},
		{"=", TokenValue::equal_tk},
		{"<", TokenValue::less_tk},
		{"<=", TokenValue::less_equal_tk},
		{"<>", TokenValue::not_equal_tk},
		{">", TokenValue::bigger_tk},
		{">=", TokenValue::bigger_equal_tk},

		{":", TokenValue::colon_tk},
		{":=", TokenValue::assignation_tk},
		{".", TokenValue::point_tk},
		{"..", TokenValue::points_tk},
		{";", TokenValue::semicolon_tk},
		{",", TokenValue::comma_tk},
		{"(", TokenValue::round_open_bracket_tk},
		{")", TokenValue::round_close_bracket_tk},
		{"[", TokenValue::square_open_bracket_tk},
		{"]", TokenValue::square_close_bracket_tk}
	};

	enum class TokenType
	{
		Operator,
		Identifier,
		Value
	};

	const set<TokenValue> StandardTypes = { TokenValue::integer_tk, TokenValue::char_tk, TokenValue::string_tk, TokenValue::double_tk, TokenValue::boolean_tk };
	const string StandardTypesStr = "\"integer\", \"char\", \"string\", \"double\", \"boolean\"";
}