#include <iostream>

#include "../inc/tokenizer.h"

using std::cout;

CT::string_view i0{"aBCd12434"};

std::ostream& operator<<(std::ostream& out, const CT::string_view& sv)
{
  for (const auto& c : sv)
    out << c;
  return out;
}

int main()
{
  constexpr auto letters = Tok::many(Tok::alphabets());
  if (auto token = letters(i0); token)
    cout << "Found : " << (*token) << "\n";
  else
    cout << "Not found\n";
  return 0;
}
