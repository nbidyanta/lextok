#include <functional>
#include <string>

#include <iostream>
#include <algorithm>

#include "lextok.h"

static Tok::Input input[] = {
  {"\r\n+CGPADDR: 128.14.178.01\r\n"sv},    // Extract IP address
  {"\"quoted string\""sv},                  // Extract string without quotes
  {"-33"sv},                                // Negative numbers
  {"19C"sv},                                // Temperature
};

static constexpr std::size_t number_of_tests = sizeof(input) / sizeof(Tok::Input);

static std::function<bool(Tok::Input& input)> test_drivers[number_of_tests] = {

  [](Tok::Input& input) -> bool {
    std::string IP;
    // Create a tokenizer for the IP address that stores the match into a string.
    const auto IPv4_addr = Tok::map(
        Tok::at_least_one(Tok::digit()) &
        Tok::exactly(Tok::char_token('.') & Tok::at_least_one(Tok::digit()) , 3),
        [&IP](Tok::Token_view token) { IP = token; });
    // Create a tokenizer to match the overall string.
    const auto ip_at_cmd_resp = Tok::str_token("\r\n+CGPADDR: ") &
        IPv4_addr & Tok::exactly(Tok::newline(), 2);
    const auto token = ip_at_cmd_resp(input);
    // Ensure the token was extracted properly and matched.
    return token && IP == "128.14.178.01";
  },

  [](Tok::Input& input) -> bool {
    std::string str;
    const auto token = (
        Tok::char_token('"') &
        Tok::many(Tok::none_of("\""), [&str](Tok::Token_view token) {
          str = token;
        }) &
        Tok::char_token('"')
      )(input);
    return token && str == "quoted string";
  },

  [](Tok::Input& input) -> bool {
    int value = 0;
    const auto token = (
        Tok::maybe(Tok::char_token('-', [&value](Tok::Token_view){ value = -1; })) &
        Tok::at_least_one(Tok::digit(), [&value](Tok::Token_view token) {
          value *= static_cast<int>(std::stoul(std::string(token)));
        }))(input);
    return token && value == -33;
  },

  [](Tok::Input& input) -> bool {
    int value = 1;
    const auto token = (
        Tok::maybe(Tok::char_token('-', [&value](Tok::Token_view){ value = -1; })) &
        Tok::at_least_one(Tok::digit(), [&value](Tok::Token_view token) {
          value *= static_cast<int>(std::stoul(std::string(token)));
        }) & (Tok::char_token('C') | Tok::char_token('F')))(input);
    return token && value == 19;
  }

};

int main()
{
  for (std::size_t i = 0; i < number_of_tests; i++)
    if (!test_drivers[i](input[i])) {
      std::cerr << "Subtest " << i << " has failed\n";
      return 1;
    }
  return 0;
}
