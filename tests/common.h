#ifndef COMMON_H
#define COMMON_H

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

  std::string get_string(const CT::string_view& sv)
  {
    std::string str;
    for (const auto& c : sv)
      str += c;
    return str;
  }

}

#endif
