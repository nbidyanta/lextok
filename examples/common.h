/**
 * @file common.h
 * @brief Convenience routines provided for the examples.
 */
#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <algorithm>
#include <string>

#include "lextok.h"

namespace CT {

  /**
   * @brief Allow `CT::string_view` to be used with `std::cout`.
   * @param[in,out] out Reference to output stream.
   * @param[in] sv `CT::string_view` that needs to be sent to the output stream.
   * @returns Output stream.
   */
  std::ostream& operator<<(std::ostream& out, const CT::string_view& sv)
  {
    for (const auto& c : sv)
      out << c;
    return out;
  }

  /**
   * @brief Allow equality check of two `CT::string_view`s.
   * @param[in] lhs Left hand side operand of the equality check.
   * @param[in] rhs Right hand side operand of the equality check.
   * @retval true If the content referred to by the views are equal.
   * @retval false If the content referred to by the views are not equal.
   */
  bool operator==(const string_view& lhs, const string_view& rhs)
  {
    if (lhs.size() != rhs.size())
      return false;
    if (std::mismatch(lhs.begin(), lhs.end(),
          rhs.begin(), rhs.end()) == std::make_pair(lhs.end(), rhs.end()))
      return true;
    return false;
  }

  /**
   * @brief Convert a `CT::string_view` into a `std::string`.
   * @param[in] sv A reference to the `CT::string_view` to be copied.
   * @returns A `std::string` that has a copy of the content in the `CT::string_view`.
   */
  std::string get_string(const CT::string_view& sv)
  {
    std::string str;
    for (const auto& c : sv)
      str += c;
    return str;
  }

}

#endif
