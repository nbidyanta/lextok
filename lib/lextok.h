/**
 * @file lextok.h
 * @author Nilangshu Bidyanta
 * @version 0.4.0
 * @copyright (c) 2018 Nilangshu Bidyanta. MIT License.
 * @brief A lexical tokenization library that can potentially run at compile time.
 * @details This library provides a toolkit for building tokenizers. The tokenizers
 * work on `std::string_view` as input.
 *
 * A tokenizer is a lambda with the type `Tok::Token (Tok::Input& input)`.
 */
#ifndef LEXTOK_H
#define LEXTOK_H

#include <utility>
#include <optional>
#include <tuple>
#include <type_traits>

#include <string_view>

using namespace std::literals::string_view_literals;

/**
 * @brief A helper macro to assert in case of invalid Map type.
 */
#define VALIDATE_MAP_TYPE(map_type) { \
  static_assert(std::is_invocable_r_v<void, map_type, Token_view>, \
      "Map must be a callable type 'void (Tok::Token_view)'"); \
}

/**
 * @brief A helper macro to assert in case of invalid Tokenizer type.
 */
#define VALIDATE_TOKENIZER_TYPE(tokenizer_type) { \
  static_assert(std::is_invocable_r_v<Tok::Token, tokenizer_type, Tok::Input&>, \
      "Tokenizer must be a callable type 'Tok::Token (Tok::Input&)'"); \
}

/**
 * @brief A helper macro to assert in case of invalid Acceptor type.
 */
#define VALIDATE_ACCEPTOR_TYPE(acceptor_type) { \
  static_assert(std::is_invocable_r_v<bool, acceptor_type, char>, \
      "Acceptor_Predicate must be a callable type 'bool (char c)'"); \
}


/// The main namespace for the lexical tokenization library
namespace Tok {

  using Input = std::string_view;  /**< Define an input type. */

  using Token = std::optional<std::string_view>; /**< Define a type to optionally hold a token. */

  using Token_view = std::string_view; /**< Define a type that provides a view of the extracted token. */

  using Predicate = std::string_view;  /**< Define a type that provides a view into characters that serve as predicates. */

  /// Private namespace that holds implementation details
  namespace impl {
    /**
     * @brief Attempt to extract a single character token.
     * @details The token is extracted based on a predicate passed to specialize
     * the function template. On a match, `func` is called on it.
     * @tparam Acceptor_Predicate A callable object that is of the type `bool (char c)`. It returns
     * `true` if the character `c` satisfies the condition to be classified as a token.
     * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
     * @param[in] pred A predicate that decides if a character is an acceptable token.
     * @param[in] func A callable object to further process / map the extracted token.
     * @returns A lambda that extracts a single character token based on the predicate.
     */
    template<typename Acceptor_Predicate, typename Map>
      constexpr auto single_char_tokenizer(Acceptor_Predicate&& pred, Map&& func) noexcept
      {
        VALIDATE_MAP_TYPE(Map);
        VALIDATE_ACCEPTOR_TYPE(Acceptor_Predicate);
        return [p = std::forward<Acceptor_Predicate>(pred),
        func = std::forward<Map>(func)](Input& input) -> Token {
          if (input.size() == 0 || !p(input[0]))
            return {};
          const Token_view token(input.substr(0, 1));
          func(token);
          input.remove_prefix(1);
          return {token};
        };
      }

    /**
     * @brief Compute the size of the largest token obtained by repeatedly applying `Tokenizer` on the input.
     * @tparam Tokenizer A callable type `Tok::Token (Tok::Input& input)`.
     * @param[in] tokenizer A callable object of type `Tokenizer`.
     * @param[in] input The input to the tokenizer.
     * @returns Size of the largest token obtained by repeatedly applying `Tokenizer` on the input.
     */
    template<typename Tokenizer>
      constexpr std::size_t accumulation_size(Tokenizer&& tokenizer, Input input) noexcept
      {
        std::size_t sz = 0;
        while (!input.empty()) {
          const auto token = tokenizer(input);
          if (!token)
            return sz;
          sz += (*token).size();
        }
        return sz;
      }

    /**
     * @brief Check if a std::string_view starts with the contents of another std::string_view.
     * @details This routine is in lieu of a C++20 feature where it is a member function
     * of std::string_view.
     * @param[in] target The target of the check.
     * @param[in] value std::string_view that is supposed to be a sub-string.
     * @retval true `value` is the start of the `target` std::string_view.
     * @retval false `value` is not the start of the `target` std::string_view.
     */
    constexpr bool starts_with(const std::string_view& target, const std::string_view& value)
    {
      return target.size() >= value.size() && target.compare(0, value.size(), value) == 0;
    }

    /**
     * @brief Check if a std::string_view starts with a given character.
     * @details This routine is in lieu of a C++20 feature where it is a member function
     * of std::string_view.
     * @param[in] target The target of the check.
     * @param[in] value Character to compare against.
     * @retval true `value` is the start of the `target` std::string_view.
     * @retval false `value` is not the start of the `target` std::string_view.
     */
    constexpr bool starts_with(const std::string_view& target, char value)
    {
      return target.size() >= 1 && target[0] == value;
    }
  }

  /// Private namespace that lists default Maps
  namespace mapper {
    /**
     * @brief Define a no-op callable object that can be invoked on a token.
     * @param[in] token A view into the string representing the token.
     */
    constexpr void none(Token_view token) noexcept {}
  }

  /**
   * @brief Create a tokenizer that matches lower and upper case alphabets.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches an alphabet [a-zA-Z] as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto alphabet(Map&& func = mapper::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
          }, func);
    }

  /**
   * @brief Create a tokenizer that matches only lower case alphabets.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches a lower case alphabet [a-z] as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto lower_alphabet(Map&& func = mapper::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return c >= 'a' && c <= 'z';
          }, func);
    }

  /**
   * @brief Create a tokenizer that matches only upper case alphabets.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches an upper case alphabet [A-Z] as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto upper_alphabet(Map&& func = mapper::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return c >= 'A' && c <= 'Z';
          }, func);
    }

  /**
   * @brief Create a tokenizer that matches only decimal digits.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches a decimal digit [0-9] as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto digit(Map&& func = mapper::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return c >= '0' && c <= '9';
          }, func);
    }

  /**
   * @brief Create a tokenizer that matches only hexadecimal digits.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches a hexadecimal digit [a-fA-F0-9] as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto hex_digit(Map&& func = mapper::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
          }, func);
    }

  /**
   * @brief Create a tokenizer that matches whitespace.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches a whitespace [ \\t\\r\\n] as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto whitespace(Map&& func = mapper::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r';
          }, func);
    }

  /**
   * @brief Create a tokenizer that matches any 8-bit character.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches an 8-bit character [.] as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto any(Map&& func = mapper::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return true;
          }, func);
    }

  /**
   * @brief Create a tokenizer that matches a newline character.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches a newline characters [\\r\\n] as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto newline(Map&& func = mapper::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return c == '\r' || c == '\n';
          }, func);
    }

  /**
   * @brief Create a tokenizer to match a character.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] c Character to be matched.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches the character specified, as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto char_token(char c, Map&& func = mapper::none) noexcept
    {
      VALIDATE_MAP_TYPE(Map);
      return impl::single_char_tokenizer(
          [c](char ch) -> bool {
            return c == ch;
          }, func);
    }

  /**
   * @brief Create a tokenizer to match a literal string pointed to by 'str'.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] str A view into the string that needs to be extracted into a token.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches the given string as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto str_token(Predicate str, Map&& func = mapper::none) noexcept
    {
      VALIDATE_MAP_TYPE(Map);
      return [=, func = std::forward<Map>(func)](Input& input) -> Token {
        if (!impl::starts_with(input, str))
          return {};
        func(str);
        input.remove_prefix(str.size());
        return {str};
      };
    }

  /**
   * @brief Create a tokenizer that matches a single character out of the group of characters provided.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] char_group A view into the group of characters that could be matched.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches any of the characters (only one) in the group as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto any_of(Predicate char_group, Map&& func = mapper::none) noexcept
    {
      VALIDATE_MAP_TYPE(Map);
      return [=, func = std::forward<Map>(func)](Input& input) -> Token {
        for (const auto& c : char_group)
          if (impl::starts_with(input, c)) {
            const Token_view token(input.substr(0, 1));
            func(token);
            input.remove_prefix(1);
            return {token};
          }
        return {};
      };
    }

  /**
   * @brief Create a tokenizer that will match a character **not** in the group of characters provided.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] char_group A view into the group of characters that will not be matched.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches a character **not** in the group of characters, as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto none_of(Predicate char_group, Map&& func = mapper::none) noexcept
    {
      VALIDATE_MAP_TYPE(Map);
      return [=, func = std::forward<Map>(func)](Input& input) -> Token {
        if (input.size() == 0)
          return {};
        for (const auto& c : char_group)
          if (impl::starts_with(input, c))
            return {};
        const Token_view token(input.substr(0, 1));
        func(token);
        input.remove_prefix(1);
        return {token};
      };
    }

  /**
   * @brief Create a tokenizer that matches multiple instances (zero or many) of another tokenizer.
   * @tparam Tokenizer A callable type `Tok::Token (Tok::Input& input)`.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] tokenizer A callable object of type `Tokenizer`.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that accepts zero or more number of tokens matched by `tokenizer`.
   */
  template<typename Tokenizer, typename Map = decltype(mapper::none)>
    constexpr auto many(Tokenizer&& tokenizer, Map&& func = mapper::none) noexcept
    {
      VALIDATE_TOKENIZER_TYPE(Tokenizer);
      VALIDATE_MAP_TYPE(Map);
      return [tokenizer = std::forward<Tokenizer>(tokenizer),
             func = std::forward<Map>(func)](Input& input) -> Token {
               const auto token_size = impl::accumulation_size(tokenizer, input);
               const Token_view token(input.substr(0, token_size));
               func(token);
               input.remove_prefix(token_size);
               return {token};
             };
    }

  /**
   * @brief Create a tokenizer that matches an exact number number of instances of another tokenizer.
   * @tparam Tokenizer A callable type `Tok::Token (Tok::Input& input)`.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] tokenizer A callable object of type `Tokenizer`.
   * @param[in] n The number of times the token needs to be matched.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches a target tokenizer an exact number of times.
   */
  template<typename Tokenizer, typename Map = decltype(mapper::none)>
    constexpr auto exactly(Tokenizer&& tokenizer, std::size_t n, Map&& func = mapper::none) noexcept
    {
      VALIDATE_TOKENIZER_TYPE(Tokenizer);
      VALIDATE_MAP_TYPE(Map);
      return [=, tokenizer = std::forward<Tokenizer>(tokenizer),
             func = std::forward<Map>(func)](Input& input) -> Token {
               const auto input_tokenize = input;
               std::size_t token_size = 0;
               for (std::size_t i = 0; i < n; i++) {
                 auto token = tokenizer(input);
                 if (!token)
                   return {};
                 token_size += (*token).size();
               }
               const Token_view token(input_tokenize.substr(0, token_size));
               func(token);
               return {token};
             };
    }

  /**
   * @brief Create a tokenizer that matches at least one instance of another tokenizer.
   * @tparam Tokenizer A callable type `Tok::Token (Tok::Input& input)`.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] tokenizer A callable object of type `Tokenizer`.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that accepts at least one instance of the tokens matched by `tokenizer`.
   */
  template<typename Tokenizer, typename Map = decltype(mapper::none)>
    constexpr auto at_least_one(Tokenizer&& tokenizer, Map&& func = mapper::none) noexcept
    {
      VALIDATE_TOKENIZER_TYPE(Tokenizer);
      VALIDATE_MAP_TYPE(Map);
      return [tokenizer = std::forward<Tokenizer>(tokenizer),
             func = std::forward<Map>(func)](Input& input) -> Token {
               const auto input_tokenize = input;
               const auto first_token = tokenizer(input);
               if (!first_token)
                 return {};
               std::size_t token_size_trailing = impl::accumulation_size(tokenizer, input);
               const Token_view token(input_tokenize.substr(0, token_size_trailing + (*first_token).size()));
               func(token);
               input.remove_prefix(token_size_trailing);
               return {token};
             };
    }

  /**
   * @brief Create a tokenizer that optionally accepts matches of another tokenizer.
   * @details In case of an empty match, `func` is still called.
   * @tparam Tokenizer A callable type `Tok::Token (Tok::Input& input)`.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] tokenizer A callable object of type `Tokenizer`.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that optionally accepts one instance of the tokens matched by `tokenizer`.
   */
  template<typename Tokenizer, typename Map = decltype(mapper::none)>
    constexpr auto maybe(Tokenizer&& tokenizer, Map&& func = mapper::none) noexcept
    {
      VALIDATE_TOKENIZER_TYPE(Tokenizer);
      VALIDATE_MAP_TYPE(Map);
      return [tokenizer = std::forward<Tokenizer>(tokenizer),
             func = std::forward<Map>(func)](Input& input) -> Token {
               const auto token = tokenizer(input);
               if (!token) {
                 func({});
                 return {Token_view{}};
               }
               func(*token);
               return token;
             };
    }

  /**
   * @brief Apply a callable object to the output of a sequence tokenizer.
   * @tparam Tokenizer A callable type `Tok::Token (Tok::Input& input)`.
   * @tparam Map A callable type `void (Tok::Token_view)`. It is called on the extracted token.
   * @param[in] seq A sequence tokenizer (returned by Tok::sequence).
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that accepts an ordered sequence of matches by two tokenizers and
   * applies a callable object to the result.
   */
  template<typename Tokenizer, typename Map>
    constexpr auto map(Tokenizer&& seq, Map&& func) noexcept
    {
      VALIDATE_TOKENIZER_TYPE(Tokenizer);
      VALIDATE_MAP_TYPE(Map);
      return [seq = std::forward<Tokenizer>(seq),
             func = std::forward<Map>(func)](Input& input) -> Token {
               const auto token = seq(input);
               if (!token)
                 return {};
               func(*token);
               return {token};
             };
    }

}

/**
 * @brief Create a tokenizer that accepts matches of multiple tokenizers in sequence.
 * @details All tokenizers must succeed for a successful match.
 * @tparam TokenizerL A callable type `Tok::Token (Tok::Input& input)`.
 * @tparam TokenizerR A callable type `Tok::Token (Tok::Input& input)`.
 * @param[in] tl A callable object of type `TokenizerL`. This is first to be evaluated.
 * @param[in] tr A callable object of type `TokenizerR`. This is second to be evaluated.
 * @returns A lambda that accepts an ordered sequence of matches by two tokenizers.
 */
template<typename TokenizerL, typename TokenizerR,
  typename std::enable_if<
    std::is_invocable_r_v<Tok::Token, TokenizerL, Tok::Input&> &&
    std::is_invocable_r_v<Tok::Token, TokenizerR, Tok::Input&>
  >* = nullptr>
constexpr auto operator&(TokenizerL&& tl, TokenizerR&& tr) noexcept
{
  return [tl = std::forward<TokenizerL>(tl),
         tr = std::forward<TokenizerR>(tr)](Tok::Input& input) -> Tok::Token {
           const auto input_tokenize = input;
           const auto tokenL = tl(input);
           if (!tokenL) {
             input = input_tokenize;
             return {};
           }
           const auto tokenR = tr(input);
           if (!tokenR) {
             input = input_tokenize;
             return {};
           }
           return {input_tokenize.substr(0, (*tokenL).size() + (*tokenR).size())};
         };
}

/**
 * @brief Create a tokenizer that chooses a successful match between two tokenizers.
 * @details At least one tokenizer must succeed for this tokenizer to succeed.
 * @tparam TokenizerL A callable type `Tok::Token (Tok::Input& input)`.
 * @tparam TokenizerR A callable type `Tok::Token (Tok::Input& input)`.
 * @param[in] tl A callable object of type `TokenizerL`. This is the first option to try.
 * @param[in] tr A callable object of type `TokenizerR`. This is the second option to try.
 * @returns A lambda that chooses the tokenizer that succeeds. In case all options fail,
 * the overall tokenizer also fails.
 */
template<typename TokenizerL, typename TokenizerR,
  typename std::enable_if<
    std::is_invocable_r_v<Tok::Token, TokenizerL, Tok::Input&> &&
    std::is_invocable_r_v<Tok::Token, TokenizerR, Tok::Input&>
  >* = nullptr>
constexpr auto operator|(TokenizerL&& tl, TokenizerR&& tr) noexcept
{
  return [tl = std::forward<TokenizerL>(tl),
         tr = std::forward<TokenizerR>(tr)](Tok::Input& input) -> Tok::Token {
           if (const auto token = tl(input); token)
             return token;
           return tr(input);
         };
}

#endif
