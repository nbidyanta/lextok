#include <iostream>

#include "../inc/lextok.h"

using std::cout;

constexpr CT::string_view i0{"aBCd12434"};

std::ostream& operator<<(std::ostream& out, const CT::string_view& sv)
{
  for (const auto& c : sv)
    out << c;
  return out;
}

constexpr auto lex(const CT::string_view& sv)
{
  std::size_t sz = 0;
  auto i = sv;
  const auto letters = Tok::many(Tok::alphabets([&sz](CT::string_view token){ sz++; }));
  const auto token = letters(i);
  return token;
}

int main()
{
  constexpr auto token = lex(i0);
  if (!token) {
    cout << "Not found\n";
    return 1;
  }
  cout << "Found : " << (*token) << "\n";
  return 0;
}
