#ifndef _COMMON_H
#define _COMMON_H

#include <iostream>
#include "lextok.h"

using std::cout;

std::ostream& operator<<(std::ostream& out, const CT::string_view& sv)
{
  for (const auto& c : sv)
    out << c;
  return out;
}

#endif
