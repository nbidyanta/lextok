/**
 * @file example2.cpp
 * @brief Shows how the library can be used as a basic command parser.
 * @details For more details look in the `examples/` directory.
 * @example example2.cpp
 */
#include "common.h"

/**
 * @brief Main entry point.
 * @retval 0 On success
 * @retval 1 On failure
 */
int main()
{
  std::string IP;                 // Will hold the extracted IP address
  std::string input = "\r\n+CGPADDR: 128.14.178.01\r\n";
  Tok::Input input_view{input};

  // Create a parser for the response of an AT command from a modem.
  // Here's the EBNF the following code replicates:
  // DIGIT                  := '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';
  // ipv4_octet             := DIGIT+;
  // ipv4_dotted_octet      := '.' ipv4_octet;
  // ipv4_addr              := ipv4_octet ipv4_dotted_octet ipv4_dotted_octet ipv4_dotted_octet;
  // guard                  := {'\r' | \n'} {'\r' | '\n'};
  // cmd_CGPADDR            := '+CGPADDR: ';
  // at_CGPADDR_cmd_parser  := guard cmd_CGPADDR ipv4_addr guard
  const auto ipv4_octet = Tok::at_least_one(Tok::digit());
  const auto ipv4_dotted_octet = Tok::char_token('.') & ipv4_octet;
  const auto ipv4_addr = Tok::map(ipv4_octet &  Tok::exactly(ipv4_dotted_octet, 3),
      [&IP](Tok::Token_view token) { IP = CT::get_string(token); });
  const auto guard = Tok::exactly(Tok::newline(), 2);
  const auto cmd_CGPADDR = Tok::str_token("+CGPADDR: ");
  const auto at_CGPADDR_cmd_parser = guard & cmd_CGPADDR & ipv4_addr & guard;

  // Apply the tokenizer to the input
  const auto token = at_CGPADDR_cmd_parser(input_view);

	// On failure to find a quoted string, 'token' will hold 'std::nullopt'
  if (!token) {
    std::cout << "Unable to parse IP address\n";
    return 1;
  }

  std::cout << "Whole token : " << *token << "\n";
  std::cout << "IP address : " << IP << "\n";
  return 0;
}
