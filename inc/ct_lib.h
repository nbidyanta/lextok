#ifndef CT_LIB_H
#define CT_LIB_H

#include <cstddef>
#include <iterator>
#include <algorithm>

/**
 * Compile time library.
 * Certain parts of the standard library aren't still constexpr (as of g++ 7.2).
 * This algorithms library, among other things, tries to emulate them by implementing the minimum functionality.
 */

namespace CT {

  constexpr std::size_t strlen(const char* cstr)
  {
    std::size_t sz = 0;
    while (cstr[sz++] != '\0')
      ;
    return sz - 1;
  }

  class string_view {
    private:
      const char *str;
      std::size_t sz;
    public:
      constexpr string_view() noexcept : str{nullptr}, sz{0} {}
      constexpr string_view(const char* cstr) noexcept : str(cstr), sz{strlen(cstr)} {}
      constexpr string_view(const char* cstr, std::size_t count) noexcept : str{cstr}, sz{count} {}
      constexpr string_view(const string_view& sv, std::size_t count) noexcept :
        str{sv.data()}, sz{std::min(count, sv.size())} {}
      constexpr string_view(const string_view& sv) noexcept : str{sv.data()}, sz{sv.size()} {}
      constexpr void remove_prefix(std::size_t n) noexcept { sz = n > sz ? 0 : sz - n; str += n; }
      constexpr void remove_suffix(std::size_t n) noexcept { sz = n > sz ? 0 : sz - n; }
      constexpr const char* begin() const noexcept { return str; }
      constexpr const char* end() const noexcept { return str + sz; }
      constexpr const char* cbegin() const noexcept { return str; }
      constexpr const char* cend() const noexcept { return str + sz; }
      constexpr std::size_t size() const noexcept { return sz; }
      constexpr bool empty() const noexcept { return sz == 0; }
      constexpr const char* data() const noexcept { return str; }
      constexpr const char& operator[](std::size_t index) const noexcept { return str[index]; }

      constexpr bool starts_with(const string_view& prefix) const noexcept
      {
        if (prefix.size() > sz)
          return false;
        for (std::size_t i = 0; i < prefix.size(); i++)
          if (prefix[i] != str[i])
            return false;
        return true;
      }

  };

  template<class InputIt, class T>
    typename std::iterator_traits<InputIt>::difference_type
    constexpr count(InputIt first, InputIt last, const T& value)
    {
      typename std::iterator_traits<InputIt>::difference_type ret = 0;
      for (; first != last; ++first)
        if (*first == value)
          ret++;
      return ret;
    }

  template<class InputIt1, class InputIt2>
    std::pair<InputIt1, InputIt2>
    constexpr mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
    {
      while (first1 != last1 && first2 != last2 && *first1 == *first2) {
        ++first1, ++first2;
      }
      return std::make_pair(first1, first2);
    }

  constexpr bool isalnum(std::uint16_t ch)
  {
    return (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z');
  }

  constexpr bool isalpha(std::uint16_t ch)
  {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
  }

  constexpr bool isdigit(std::uint16_t ch)
  {
    return ch >= '0' && ch <= '9';
  }

  constexpr bool isxdigit(std::uint16_t ch)
  {
    return (ch >= 'a' && ch <= 'f') || (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F');
  }

}

#endif
