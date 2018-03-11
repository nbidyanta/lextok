#include "common.h"
#include <functional>

static CT::string_view input[] = {
  {},                         // Empty input
  {"Text"},                   // Mismatch
  {"9"},                      // Regular match
  {"1234567890"},             // Count number of tokens matched
  {"AA535"}                   // Airline code
};

static constexpr std::size_t number_of_tests = sizeof(input) / sizeof(CT::string_view);

static std::function<bool(CT::string_view& input)> test_drivers[number_of_tests] = {

  [](CT::string_view& input) -> bool {
    const auto token = Tok::decimal_digit()(input);
    return (!token) ? true : false;
  },

  [](CT::string_view& input) -> bool {
    const auto token = Tok::decimal_digit()(input);
    return (!token) ? true : false;
  },

  [](CT::string_view& input) -> bool {
    const auto token = Tok::decimal_digit()(input);
    return token && *token == "9";
  },

  [](CT::string_view& input) -> bool {
    std::size_t sz = 0;
    const auto token = Tok::many(Tok::decimal_digit([&sz](CT::string_view){ sz++; }))(input);
    return token && *token == "1234567890" && sz == CT::strlen("1234567890");
  },

  [](CT::string_view& input) -> bool {
    const auto token = Tok::sequence(
        Tok::at_least_one(Tok::upper_alphabet()),
        Tok::at_least_one(Tok::decimal_digit()))(input);
    return token && *token == "AA535";
  },

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
