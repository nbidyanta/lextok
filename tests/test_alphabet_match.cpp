#include "common.h"

constexpr CT::string_view i0{"aBCd12434"};

constexpr auto lex(const CT::string_view& sv)
{
  std::size_t sz = 0;
  auto i = sv;
  const auto letters = Tok::many(Tok::alphabet([&sz](CT::string_view token){ sz++; }));
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
