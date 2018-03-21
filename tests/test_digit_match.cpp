#include <functional>
#include <string>

#include "common.h"

static Tok::Input input[] = {
  {},                         // Empty input
  {"Text"},                   // Mismatch
  {"9"},                      // Regular match
  {"1234567890"},             // Count number of tokens matched
  {"AA535"},                  // Airline code
  {"0xA22b3a"}                // Hexadecimal digits
};

static constexpr std::size_t number_of_tests = sizeof(input) / sizeof(Tok::Input);

static std::function<bool(Tok::Input& input)> test_drivers[number_of_tests] = {

  [](Tok::Input& input) -> bool {
    const auto token = Tok::digit()(input);
    return (!token) ? true : false;
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::digit()(input);
    return (!token) ? true : false;
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::digit()(input);
    return token && *token == "9";
  },

  [](Tok::Input& input) -> bool {
    std::size_t sz = 0;
    const auto token = Tok::many(Tok::digit([&sz](Tok::Token_view){ sz++; }))(input);
    return token && *token == "1234567890" && sz == CT::strlen("1234567890");
  },

  [](Tok::Input& input) -> bool {
    std::string airline_name;
    std::uint16_t flight_number;
    const auto token = (
        Tok::at_least_one(Tok::upper_alphabet(), [&airline_name](Tok::Token_view token) {
          airline_name = CT::get_string(token);
        }) & Tok::at_least_one(Tok::digit(), [&flight_number](Tok::Token_view token) {
          flight_number = static_cast<std::uint16_t>(std::stoul(token.data()));
        }))(input);
    return token && *token == "AA535" && airline_name == "AA" && flight_number == 535;
  },

  [](Tok::Input& input) -> bool {
    std::uint32_t hex_data = 0;
    const auto token = (
        Tok::str_token("0x") &
        Tok::at_least_one(Tok::hex_digit([&hex_data](Tok::Token_view token) {
            hex_data *= 16;
            hex_data += (token[0] >= 'a' ?
                token[0] - 'a' + 10 : token[0] >= 'A' ?
                token[0] - 'A' + 10 : token[0] - '0');
        })))(input);
    return token && hex_data == 0xA22B3A;
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
