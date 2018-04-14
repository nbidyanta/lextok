#include <functional>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>

#include "lextok.h"

static Tok::Input input[] = {
  {},                 // Empty input
  {"12-4"sv},         // No matches
  {"G"sv},            // Mismatch (upper case)
  {"x"sv},            // Normal match (lower case)
  {"BAn"sv},          // Multiple matches (non-zero)
  {"00"sv},           // Multiple matches (zero)
  {"F1"sv},           // At least one match (positive)
  {"19"sv},           // At least one match (negative)
  {"IAMSHOUTING"sv}   // Count number of tokens matched
};

static constexpr std::size_t number_of_tests = sizeof(input) / sizeof(Tok::Input);

static std::function<bool(Tok::Input& input)> test_drivers[number_of_tests] = {

  [](Tok::Input& input) -> bool {
    const auto token = Tok::upper_alphabet()(input);
    return (!token) ? true : false;
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::upper_alphabet()(input);
    return (!token) ? true : false;
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::upper_alphabet()(input);
    return token && *token == "G";
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::upper_alphabet()(input);
    return (!token) ? true : false;
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::many(Tok::upper_alphabet())(input);
    return token && *token == "BA";
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::many(Tok::upper_alphabet())(input);
    return token && *token == "";
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::at_least_one(Tok::upper_alphabet())(input);
    return token && *token == "F";
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::at_least_one(Tok::upper_alphabet())(input);
    return (!token) ? true : false;
  },

  [](Tok::Input& input) -> bool {
    std::size_t sz = 0;
    const auto token = Tok::at_least_one(Tok::upper_alphabet([&sz](Tok::Token_view){ sz++; }))(input);
    return token && *token == "IAMSHOUTING" && sz == strlen("IAMSHOUTING");
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
