#ifndef CT_LIB_H
#define CT_LIB_H

#include <cstddef>
#include <iterator>

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
      constexpr void remove_prefix(std::size_t n) noexcept { sz = n > sz ? 0 : sz - n; str += n; }
      constexpr void remove_suffix(std::size_t n) noexcept { sz = n > sz ? 0 : sz - n; }
      constexpr std::size_t size() const noexcept { return sz; }
      constexpr bool empty() const noexcept { return sz == 0; }
      constexpr const char& operator[](std::size_t index) const noexcept { return str[index]; }
      constexpr bool starts_with(const char* prefix) const noexcept
      {
        auto prefix_len = strlen(prefix);
        if (sz < prefix_len)
          return 0;
        for (std::size_t i = 0; i < prefix_len; i++)
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
