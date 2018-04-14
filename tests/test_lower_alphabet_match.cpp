#include <functional>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>

#include "lextok.h"

static Tok::Input input[] = {
  {},                               // Empty input
  {"12-4"sv},                       // No matches
  {"G"sv},                          // Mismatch (upper case)
  {"x"sv},                          // Normal match (lower case)
  {"naB"sv},                        // Multiple matches (non-zero)
  {"00"sv},                         // Multiple matches (zero)
  {"f1"sv},                         // At least one match (positive)
  {"19"sv},                         // At least one match (negative)
  {"thissentenceisalllowercase"sv}  // Count number of tokens matched
};

static constexpr std::size_t number_of_tests = sizeof(input) / sizeof(Tok::Input);

static std::function<bool(Tok::Input& input)> test_drivers[number_of_tests] = {

  [](Tok::Input& input) -> bool {
    const auto token = Tok::lower_alphabet()(input);
    return (!token) ? true : false;
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::lower_alphabet()(input);
    return (!token) ? true : false;
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::lower_alphabet()(input);
    return (!token) ? true : false;
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::lower_alphabet()(input);
    return token && *token == "x";
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::many(Tok::lower_alphabet())(input);
    return token && *token == "na";
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::many(Tok::lower_alphabet())(input);
    return token && *token == "";
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::at_least_one(Tok::lower_alphabet())(input);
    return token && *token == "f";
  },

  [](Tok::Input& input) -> bool {
    const auto token = Tok::at_least_one(Tok::lower_alphabet())(input);
    return (!token) ? true : false;
  },

  [](Tok::Input& input) -> bool {
    std::size_t sz = 0;
    const auto token = Tok::at_least_one(Tok::lower_alphabet([&sz](Tok::Token_view){ sz++; }))(input);
    return token && *token == "thissentenceisalllowercase" && sz == strlen("thissentenceisalllowercase");
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
