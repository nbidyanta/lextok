#include "common.h"
#include <functional>

static CT::string_view input[] = {
  {},                           // Empty input
  {"12-4"},                     // No matches
  {"G"},                        // Normal match (upper case)
  {"x"},                        // Normal match (lower case)
  {"nB"},                       // Multiple matches (non-zero)
  {"00"},                       // Multiple matches (zero)
  {"f1"},                       // At least one match (positive)
  {"19"},                       // At least one match (negative)
  {"ThisIsACamelCaseSentence"}  // Count number of tokens matched
};

static constexpr std::size_t number_of_tests = sizeof(input) / sizeof(CT::string_view);

static std::function<bool(CT::string_view& input)> test_drivers[number_of_tests] = {

  [](CT::string_view& input) -> bool {
    const auto token = Tok::alphabet()(input);
    return !token ? true : false;
  },

  [](CT::string_view& input) -> bool {
    const auto token = Tok::alphabet()(input);
    return !token ? true : false;
  },

  [](CT::string_view& input) -> bool {
    const auto token = Tok::alphabet()(input);
    return token && *token == "G";
  },

  [](CT::string_view& input) -> bool {
    const auto token = Tok::alphabet()(input);
    return token && *token == "x";
  },

  [](CT::string_view& input) -> bool {
    const auto token = Tok::many(Tok::alphabet())(input);
    return token && *token == "nB";
  },

  [](CT::string_view& input) -> bool {
    const auto token = Tok::many(Tok::alphabet())(input);
    return token && *token == "";
  },

  [](CT::string_view& input) -> bool {
    const auto token = Tok::at_least_one(Tok::alphabet())(input);
    return token && *token == "f";
  },

  [](CT::string_view& input) -> bool {
    const auto token = Tok::at_least_one(Tok::alphabet())(input);
    return !token ? true : false;
  },

  [](CT::string_view& input) -> bool {
    std::size_t sz = 0;
    const auto token = Tok::at_least_one(Tok::alphabet([&sz](CT::string_view){ sz++; }))(input);
    return token && *token == "ThisIsACamelCaseSentence" && sz == CT::strlen("ThisIsACamelCaseSentence");
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
