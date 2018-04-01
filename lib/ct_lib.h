/**
 * @file ct_lib.h
 * @author Nilangshu Bidyanta
 * @copyright (c) 2018 Nilangshu Bidyanta. MIT License.
 * @brief Compile Time LIBrary substitute
 * @details This is a compile time support library needed by the tokenization library.
 * All of the functionality contained here can be replaced with the standard C++17 library.
 * However, as of g++ 7.2, these are **not** marked `constexpr` in the standard library
 * and thus cannot be used at compile time.
 */
#ifndef CT_LIB_H
#define CT_LIB_H

#include <cstddef>
#include <iterator>
#include <algorithm>
#include <string>

/// Main namespace for the Compile Time library
namespace CT {

  /**
   * @brief Compute string length of a NULL terminated string.
   * @param[in] cstr Pointer to the NULL terminated string.
   * @returns Length of the string in bytes.
   */
  constexpr std::size_t strlen(const char* cstr)
  {
    std::size_t sz = 0;
    while (cstr[sz++] != '\0')
      ;
    return sz - 1;
  }

  /**
   * @brief Provide a read-only view to a string.
   * @details This class emulates `std::string_view`. The implementation is almost exactly
   * the same except that here it is `constexpr` qualified.
   * @note If using a version of g++ with a `constexpr` qualified version of `std::string_view`,
   * do not use this class.
   * @warning This class does not own the underlying string.
   */
  class string_view {
    private:
      const char *str;  /**< Pointer to the beginning of the string data. */
      std::size_t sz;   /**< Size of the data in bytes. */

    public:
      /**
       * @brief Default construct a `string_view`.
       */
      constexpr string_view() noexcept : str{nullptr}, sz{0} {}

      /**
       * @brief Construct a `string_view` from a NULL terminated string.
       * @param[in] cstr Pointer to a NULL terminated string.
       */
      constexpr string_view(const char* cstr) noexcept : str{cstr}, sz{strlen(cstr)} {}

      /**
       * @brief Consturct a `string_view` from a `std::string`.
       * @param[in] std_str A reference to a `std::string`.
       */
      string_view(const std::string& std_str) : str{std_str.c_str()}, sz{strlen(std_str.c_str())} {}

      /**
       * @brief Construct a `string_view` from a slice of `string_view`.
       * @param[in] sv Reference to a `string_view`.
       * @param[in] count Maximum number of bytes to take from the beginning of `sv`
       */
      constexpr string_view(const string_view& sv, std::size_t count) noexcept :
        str{sv.data()}, sz{std::min(count, sv.size())} {}

      /**
       * @brief Trivially construct a `string_view` from another `string_view`.
       * @param[in] sv Reference to a `string_view`.
       */
      constexpr string_view(const string_view& sv) noexcept = default;

      /**
       * @brief Remove a prefix of size `n`.
       * @param[in] n Number of bytes to remove from the beginning of the `string_view`.
       */
      constexpr void remove_prefix(std::size_t n) noexcept { sz = n > sz ? 0 : sz - n; str += n; }

      /**
       * @brief Remove a suffix of size `n`.
       * @param[in] n Number of bytes to remove from the end of the `string_view`.
       */
      constexpr void remove_suffix(std::size_t n) noexcept { sz = n > sz ? 0 : sz - n; }

      /**
       * @brief Retrieve the `begin` iterator.
       * @returns An iterator to the beginning of the `string_view`.
       */
      constexpr const char* begin() const noexcept { return str; }

      /**
       * @brief Retrieve the `end` iterator.
       * @returns An iterator to the end of the `string_view`.
       */
      constexpr const char* end() const noexcept { return str + sz; }

      /**
       * @brief Retrieve the `begin` iterator of a constant `string_view`.
       * @returns An iterator to the beginning of the `string_view`.
       */
      constexpr const char* cbegin() const noexcept { return str; }

      /**
       * @brief Retrieve the `end` iterator of a constant `string_view`.
       * @returns An iterator to the end of the `string_view`.
       */
      constexpr const char* cend() const noexcept { return str + sz; }

      /**
       * @brief Retrieve the size of the view.
       * @returns Number of bytes seen through the view.
       */
      constexpr std::size_t size() const noexcept { return sz; }

      /**
       * @brief Test whether the view can see anything.
       * @retval true Nothing to show.
       * @retval false Data viewable through the view.
       */
      constexpr bool empty() const noexcept { return sz == 0; }

      /**
       * @brief Retrieve the data contained within the view.
       * @returns Pointer to the data being viewed.
       */
      constexpr const char* data() const noexcept { return str; }

      /**
       * @brief Implement the indexing operator.
       * @param[in] index Index of the data to be retrieved.
       * @returns Reference to the character at the index inside the `string_view`.
       */
      constexpr const char& operator[](std::size_t index) const noexcept { return str[index]; }

      /**
       * @brief Equality operator.
       * @param[in] Reference to rhs.
       * @retval true Views match.
       * @retval false Views mismatch.
       */
      constexpr bool operator==(const string_view& rhs) const noexcept
      {
        if (sz != rhs.size())
          return false;
        for (std::size_t i = 0; i < sz; i++)
          if (str[i] != rhs[i])
            return false;
        return true;
      }

      /**
       * @brief Verify if the `string_view` starts with a given prefix.
       * @param[in] prefix Constant reference to a view holding the prefix.
       * @retval true Prefix was found.
       * @retval false Prefix was not found.
       */
      constexpr bool starts_with(const string_view& prefix) const noexcept
      {
        if (prefix.size() > sz)
          return false;
        for (std::size_t i = 0; i < prefix.size(); i++)
          if (prefix[i] != str[i])
            return false;
        return true;
      }

      /**
       * @brief Verify if the `string_view` starts with a given prefix.
       * @param[in] prefix Character prefix to look for.
       * @retval true Prefix was found.
       * @retval false Prefix was not found.
       */
      constexpr bool starts_with(char prefix) const noexcept
      {
        if (sz == 0)
          return false;
        if (prefix == str[0])
          return true;
        return false;
      }

      /**
       * @brief Verify if the `string_view` starts with a given prefix.
       * @param[in] prefix NULL terminated prefix to look for.
       * @retval true Prefix was found.
       * @retval false Prefix was not found.
       */
      constexpr bool starts_with(const char* prefix) const noexcept
      {
        auto prefix_size = strlen(prefix);
        if (prefix_size > sz)
          return false;
        for (std::size_t i = 0; i < prefix_size; i++)
          if (prefix[i] != str[i])
            return false;
        return true;
      }
  };

}

#endif
