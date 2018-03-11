#ifndef _COMMON_H
#define _COMMON_H

#include <iostream>
#include <algorithm>
#include <string>

#include "lextok.h"

namespace CT {

  std::ostream& operator<<(std::ostream& out, const CT::string_view& sv)
  {
    for (const auto& c : sv)
      out << c;
    return out;
  }

  bool operator==(const string_view& lhs, const string_view& rhs)
  {
    if (lhs.size() != rhs.size())
      return false;
    if (std::mismatch(lhs.begin(), lhs.end(),
          rhs.begin(), rhs.end()) == std::make_pair(lhs.end(), rhs.end()))
      return true;
    return false;
  }

}

#endif
