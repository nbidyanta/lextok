#include <functional>
#include <string>

#include "common.h"

static std::string get_string(const CT::string_view& sv)
{
  std::string str;
  for (const auto& c : sv)
    str += c;
  return str;
}

static CT::string_view input[] = {
  {"\r\n+CGPADDR: 128.14.178.01\r\n"},    // Extract IP address
  {"\"quoted string\""},                  // Extract string without quotes
  {"-33"},                                // Negative numbers
  {"19C"},                                // Temperature
};

static constexpr std::size_t number_of_tests = sizeof(input) / sizeof(CT::string_view);

static std::function<bool(CT::string_view& input)> test_drivers[number_of_tests] = {

  [](CT::string_view& input) -> bool {
    std::string IP;
    // Create a tokenizer for the IP address that stores the match into a string.
    const auto IPv4_addr = Tok::map_seq(Tok::sequence(
        Tok::at_least_one(Tok::digit()),
        Tok::exactly(
          Tok::sequence(
            Tok::char_token('.'),
            Tok::at_least_one(Tok::digit())
          ), 3
        )
      ), [&IP](CT::string_view token) { IP = get_string(token); });
    // Create a tokenizer to match the overall string.
    const auto ip_at_cmd_resp = Tok::sequence(
        Tok::str_token("\r\n+CGPADDR: "),
        IPv4_addr,
        Tok::exactly(Tok::newline(), 2)
    );
    const auto token = ip_at_cmd_resp(input);
    // Ensure the token was extracted properly and matched.
    return token && IP == "128.14.178.01";
  },

  [](CT::string_view& input) -> bool {
    std::string str;
    const auto token = Tok::sequence(
        Tok::char_token('"'),
        Tok::many(Tok::none_of("\""), [&str](CT::string_view token) {
          str = get_string(token);
        }),
        Tok::char_token('"')
      )(input);
    return token && str == "quoted string";
  },

  [](CT::string_view& input) -> bool {
    int value = 0;
    const auto token = Tok::sequence(
        Tok::maybe(Tok::char_token('-', [&value](CT::string_view){ value = -1; })),
        Tok::at_least_one(Tok::digit(), [&value](CT::string_view token) {
          value *= static_cast<int>(stoul(get_string(token)));
        }))(input);
    return token && value == -33;
  },

  [](CT::string_view& input) -> bool {
    int value = 1;
    const auto token = Tok::sequence(
        Tok::maybe(Tok::char_token('-', [&value](CT::string_view){ value = -1; })),
        Tok::at_least_one(Tok::digit(), [&value](CT::string_view token) {
          value *= static_cast<int>(stoul(get_string(token)));
        }), Tok::char_token('C') | Tok::char_token('F'))(input);
    std::cerr << "val = " << value << "\n";
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
