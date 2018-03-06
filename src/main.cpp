#include <iostream>

#include <utility>
#include <optional>

#include "../inc/ct_lib.h"

using std::cout;

constexpr CT::string_view input{"\r\n+CEREG: 1\r\n"};

template <typename T>
using token = std::optional<std::pair<T, CT::string_view>>;

constexpr auto char_token(char c)
{
  return [=](CT::string_view input) -> token<char> {
    if (input[0] != c)
      return std::nullopt;
    input.remove_prefix(1);
    return {{c, input}};
  };
}

constexpr auto str_token(CT::string_view str)
{
  return [=](CT::string_view input) -> token<CT::string_view> {
    if (!input.starts_with(str))
      return std::nullopt;
    input.remove_prefix(str.size());
    return {{str, input}};
  };
}

int main()
{
  constexpr auto t = str_token("\r+");
  if (auto res = t(input);
      !res) {
    cout << "Not found\n";
    return 1;
  }
  cout << "Found!\n";
  return 0;
}
