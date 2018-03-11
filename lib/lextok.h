/**
 * @file lextok.h
 * @author Nilangshu Bidyanta
 * @copyright (c) 2018 Nilangshu Bidyanta. MIT License.
 * @brief A lexical tokenization library that can potentially run at compile time.
 * @details This library provides a toolkit for building tokenizers. The tokenizers
 * work on `std::string_view` as input. For the time being, a custom implementation --
 * `CT::string_view`, is being used since `std::string_view` cannot be evaluated at
 * compile time. In the future, this will be replaced by `std::string_view`.
 * Inspiration for this library was drawn from [here](https://fsharpforfunandprofit.com/posts/understanding-parser-combinators/)
 * and [here](https://www.youtube.com/watch?v=PJwd4JLYJJY).
 *
 * A tokenizer is a lambda with the type `Tok::Token (Tok::Input& input)`.
 */
#ifndef LEXTOK_H
#define LEXTOK_H

#include <utility>
#include <optional>

#include "ct_lib.h"    // XXX: Only needed until std::string_view is constexpr enabled.

/// The main namespace for the lexical tokenization library
namespace Tok {

  using Input = CT::string_view;  /**< Define an input type. */

  using Token = std::optional<CT::string_view>; /**< Define a type to optionally hold a token. */

  /// Private namespace that holds implementation details
  namespace impl {
    /**
     * @brief Attempt to extract a single character token.
     * @details The token is extracted based on a predicate passed to specialize
     * the function template. On a match, `func` is called on it.
     * @tparam Acceptor_Predicate A callable object that is of the type `bool (char c)`. It returns
     * `true` if the character `c` satisfies the condition to be classified as a token.
     * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
     * @param[in] pred A predicate that decides if a character is an acceptable token.
     * @param[in] func A callable object to further process / map the extracted token.
     * @returns A lambda that extracts a single character token based on the predicate.
     */
    template<typename Acceptor_Predicate, typename Map>
      constexpr auto single_char_tokenizer(Acceptor_Predicate&& pred, Map&& func) noexcept
      {
        return [p = std::forward<Acceptor_Predicate>(pred),
        func = std::forward<Map>(func)](Input& input) -> Token {
          if (input.size() == 0)
            return {};
          char c = input[0];
          if (p(c)) {
            const CT::string_view token{input, 1};
            func(token);
            input.remove_prefix(1);
            return {token};
          }
          return {};
        };
      }
  }

  /// Private namespace that lists default Maps
  namespace mapper {
    /**
     * @brief Define a no-op callable object that can be invoked on a token.
     * @param[in] token A view into the string representing the token.
     */
    constexpr void none(CT::string_view token) {}
  }

  /**
   * @brief Create a tokenizer that matches lower and upper case alphabets.
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches an alphabet [a-zA-Z] as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto alphabet(Map&& func = mapper::none) noexcept
    {
      return impl::single_char_tokenizer(
          [func = std::forward<Map>(func)](char c) -> bool {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
          }, func);
    }

  /**
   * @brief Create a tokenizer that matches only lower case alphabets.
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
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
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
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
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches a decimal digit [0-9] as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto decimal_digit(Map&& func = mapper::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return c >= '0' && c <= '9';
          }, func);
    }

  /**
   * @brief Create a tokenizer that matches only hexadecimal digits.
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
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
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
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
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
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
   * @brief Create a tokenizer to match a character.
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
   * @param[in] c Character to be matched.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches the character specified, as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto char_token(char c, Map&& func = mapper::none) noexcept
    {
      return impl::single_char_tokenizer(
          [c, func = std::forward<Map>(func)](char ch) -> bool {
            return c == ch;
          }, func);
    }

  /**
   * @brief Create a tokenizer to match a literal string pointed to by 'str'.
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
   * @param[in] str A view into the string that needs to be extracted into a token.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches the given string as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto str_token(CT::string_view str, Map&& func = mapper::none) noexcept
    {
      return [=, func = std::forward<Map>(func)](Input& input) -> Token {
        if (!input.starts_with(str))
          return {};
        func(str);
        input.remove_prefix(str.size());
        return {str};
      };
    }

  /**
   * @brief Create a tokenizer that matches a single character out of the group of characters provided.
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
   * @param[in] char_group A view into the group of characters that could be matched.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches any of the characters (only one) in the group as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto any_char_of(CT::string_view char_group, Map&& func = mapper::none) noexcept
    {
      return [=, func = std::forward<Map>(func)](Input& input) -> Token {
        for (const auto& c : char_group)
          if (input.starts_with(c)) {
            const CT::string_view token{input, 1};
            func(token);
            input.remove_prefix(1);
            return {token};
          }
        return {};
      };
    }

  /**
   * @brief Create a tokenizer that will match a character **not** in the group of characters provided.
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
   * @param[in] char_group A view into the group of characters that will not be matched.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that matches a character **not** in the group of characters, as a token.
   */
  template<typename Map = decltype(mapper::none)>
    constexpr auto none_of(CT::string_view char_group, Map&& func = mapper::none) noexcept
    {
      return [=, func = std::forward<Map>(func)](Input& input) -> Token {
        for (const auto& c : char_group)
          if (input.starts_with(c))
            return {};
        const CT::string_view token{input, 1};
        func(token);
        input.remove_prefix(1);
        return {token};
      };
    }

  namespace impl {
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
  }

  /**
   * @brief Create a tokenizer that matches multiple instances (zero or many) of another tokenizer.
   * @tparam Tokenizer A callable type `Tok::Token (Tok::Input& input)`.
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
   * @param[in] tokenizer A callable object of type `Tokenizer`.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that accepts zero or more number of tokens matched by `tokenizer`.
   */
  template<typename Tokenizer, typename Map = decltype(mapper::none)>
    constexpr auto many(Tokenizer&& tokenizer, Map&& func = mapper::none) noexcept
    {
      return [t = std::forward<Tokenizer>(tokenizer),
             func = std::forward<Map>(func)](Input& input) -> Token {
               const auto token_size = impl::accumulation_size(t, input);
               const CT::string_view token{input, token_size};
               func(token);
               input.remove_prefix(token_size);
               return {token};
             };
    }

  /**
   * @brief Create a tokenizer that matches at least one instance of another tokenizer.
   * @tparam Tokenizer A callable type `Tok::Token (Tok::Input& input)`.
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
   * @param[in] tokenizer A callable object of type `Tokenizer`.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that accepts at least one instance of the tokens matched by `tokenizer`.
   */
  template<typename Tokenizer, typename Map = decltype(mapper::none)>
    constexpr auto at_least_one(Tokenizer&& tokenizer, Map&& func = mapper::none) noexcept
    {
      return [t = std::forward<Tokenizer>(tokenizer),
             func = std::forward<Map>(func)](Input& input) -> Token {
               const auto input_tokenize = input;
               const auto first_token = t(input);
               if (!first_token)
                 return {};
               std::size_t token_size = (*first_token).size() + impl::accumulation_size(t, input);
               const CT::string_view token{input_tokenize, token_size};
               func(token);
               input.remove_prefix(token_size);
               return {token};
             };
    }

  /**
   * @brief Create a tokenizer that optionally accepts matches of another tokenizer.
   * @details In case of an empty match, `func` is still called.
   * @tparam Tokenizer A callable type `Tok::Token (Tok::Input& input)`.
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
   * @param[in] tokenizer A callable object of type `Tokenizer`.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that optionally accepts one instance of the tokens matched by `tokenizer`.
   */
  template<typename Tokenizer, typename Map = decltype(mapper::none)>
    constexpr auto maybe(Tokenizer&& tokenizer, Map&& func = mapper::none) noexcept
    {
      return [t = std::forward<Tokenizer>(tokenizer),
             func = std::forward<Map>(func)](Input& input) -> Token {
               const auto token = t(input);
               if (!token) {
                 func({});
                 return {{}};
               }
               func(*token);
               std::size_t token_size = (*token).size();
               input.remove_prefix(token_size);
               return token;
             };
    }

  /**
   * @brief Create a tokenizer that accepts matches of two tokenizers in sequence.
   * @details Both tokenizers must succeed for a successful match. `func` is called on the final matched token.
   * @tparam TokenizerL A callable type `Tok::Token (Tok::Input& input)`.
   * @tparam TokenizerR A callable type `Tok::Token (Tok::Input& input)`.
   * @tparam Map A callable type `void (CT::string_view)`. It is called on the extracted token.
   * @param[in] tl A callable object of type `TokenizerL`. This is the first option to try.
   * @param[in] tr A callable object of type `TokenizerR`. This is the second option to try.
   * @param[in] func A callable object of type `Map`.
   * @returns A lambda that accepts an ordered sequence of matches by two tokenizers.
   */
  template<typename TokenizerL, typename TokenizerR, typename Map = decltype(mapper::none)>
    constexpr auto sequence(TokenizerL&& tl, TokenizerR&& tr, Map&& func = mapper::none) noexcept
    {
      return [tl = std::forward<TokenizerL>(tl),
             tr = std::forward<TokenizerR>(tr),
             func = std::forward<Map>(func)](Input& input) -> Token {
               const auto input_tokenize = input;
               const auto first_token = tl(input);
               if (!first_token)
                 return {};
               const auto second_token = tr(input);
               if (!second_token)
                 return {};
               const auto token_size = (*first_token).size() + (*second_token).size();
               const CT::string_view token{input_tokenize, token_size};
               func(token);
               input.remove_prefix(token_size);
               return {token};
             };
    }

  /**
   * @brief Create a tokenizer that chooses a successful match between two tokenizers.
   * @details If both tokenizers fail, this tokenizer also fails.
   * @tparam TokenizerL A callable type `Tok::Token (Tok::Input& input)`.
   * @tparam TokenizerR A callable type `Tok::Token (Tok::Input& input)`.
   * @param[in] tl A callable object of type `TokenizerL`. This is the first option to try.
   * @param[in] tr A callable object of type `TokenizerR`. This is the second option to try.
   * @returns A lambda that chooses the tokenizer that succeeds. In case all options fail,
   * the overall tokenizer also fails.
   */
  template<typename TokenizerL, typename TokenizerR>
    constexpr auto operator|(TokenizerL&& tl, TokenizerR&& tr) noexcept
    {
      return [tl = std::forward<TokenizerL>(tl),
             tr = std::forward<TokenizerR>(tr)](Input& input) -> Token {
               if (const auto first_token = tl(input); first_token) {
                 const auto token_size = (*first_token).size();
                 return {{input, token_size}};
               }
               if (const auto second_token = tr(input); second_token) {
                 const auto token_size = (*second_token).size();
                 return {{input, token_size}};
               }
               return {};
             };
    }

}

#endif
