#include <cassert>
#include "common.h"

static std::string get_string(const CT::string_view& sv)
{
  std::string str;
  for (const auto& c : sv)
    str += c;
  return str;
}

int main()
{
  std::string IP;                 // Will hold the extracted IP address
  std::string input = "\r\n+CGPADDR: 128.14.178.01\r\n";
  CT::string_view input_view{input};

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
      [&IP](CT::string_view token) { IP = get_string(token); });
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
