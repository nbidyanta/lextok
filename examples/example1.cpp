/**
 * @file example1.cpp
 * @brief Shows the basic usage of the library.
 * @details For more details look in the `examples/` directory.
 * @example example1.cpp
 */
#include <cassert>
#include "common.h"

/**
 * @brief Main entry point.
 * @retval 0 On success
 * @retval 1 On failure
 */
int main()
{
	std::string str;                  // Will hold the extracted string
	std::string input = "\"this is a string\"";
  Tok::Input input_view{input};

  // Construct a tokenizer to validate the input is a quoted string.
  // Here, a quoted string is defined as at least one instance of
  // any character (other than a double quote) between two double quotes.
  // Notice how Tok::at_least_one is being passed a Map in the form of a
  // lambda to extract the actual string without the quotes. 'get_string'
  // is assumed to be a routine that converts a Tok::Token_view into a
  // std::string.
  const auto quoted_string_tokenizer =
    Tok::char_token('"') &
    Tok::at_least_one(
        Tok::none_of("\""),
        [&str](Tok::Token_view token) {str = CT::get_string(token);}
    ) &
    Tok::char_token('"');

	// Apply the tokenizer to the input
	const auto token = quoted_string_tokenizer(input_view);

	// On failure to find a quoted string, 'token' will hold 'std::nullopt'
	if (!token) {
		std::cout << "No quoted string found\n";
		return 1;
	}

	// 'token' now contains the entire quoted string, i.e. \"this is a string\".
	// However, 'str' contains the actual string itself without the quotes,
	// i.e. this is a string.
	assert((*token).data() == input);
	assert(str == "this is a string");
  std::cout << "Assertions succeeded\n";
	return 0;
}
