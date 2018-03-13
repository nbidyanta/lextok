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
  {"\r\n+CGPADDR: 128.14.178.01\r\n"}     // Extract IP address
};

static constexpr std::size_t number_of_tests = sizeof(input) / sizeof(CT::string_view);

static std::function<bool(CT::string_view& input)> test_drivers[number_of_tests] = {
  [](CT::string_view& input) -> bool {
    std::string IP;
    const auto IPv4_addr = Tok::map_seq(Tok::sequence(
        Tok::at_least_one(Tok::digit()),
        Tok::exactly(
          Tok::sequence(
            Tok::char_token('.'),
            Tok::at_least_one(Tok::digit())
          ), 3
        )
      ), [&IP](CT::string_view token) { IP = get_string(token); });
    const auto ip_at_cmd_resp = Tok::sequence(
        Tok::str_token("\r\n+CGPADDR: "),
        IPv4_addr,
        Tok::exactly(Tok::newline(), 2)
    );
    const auto token = ip_at_cmd_resp(input);
    return token && IP == "128.14.178.01";
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
