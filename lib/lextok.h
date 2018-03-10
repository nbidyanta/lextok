/**
 * @file lextok.h
 * @brief A lexical tokenization library that can potentially be run at compile time.
 * @details This library provides a toolkit for building tokenizers. The tokenizers
 * work on `string_view`s as input. For the time being, a custom implementation,
 * `CT::string_view`, is being used. In the future, this will be replaced by `std::string_view`.
 * Inspiration for this library was drawn from [here](https://fsharpforfunandprofit.com/posts/understanding-parser-combinators/)
 * and [here](https://www.youtube.com/watch?v=PJwd4JLYJJY).
 */
#ifndef _LEXTOK_H
#define _LEXTOK_H

#include <utility>
#include <optional>

#include "ct_lib.h"    // XXX: Only needed until std::string_view is constexpr enabled.

namespace Tok {

  using Input = CT::string_view;
  using Token = std::optional<CT::string_view>;

  namespace Mod {
    constexpr void none(CT::string_view token) {}
  }

  namespace impl {
    template<typename Acceptor_Predicate, typename Modifier>
      constexpr auto single_char_tokenizer(Acceptor_Predicate&& pred, Modifier&& func) noexcept
      {
        return [p = std::forward<Acceptor_Predicate>(pred),
        func = std::forward<Modifier>(func)](Input& input) -> Token {
          char c = input[0];
          if (p(c)) {
            const CT::string_view token{input, 1};
            input.remove_prefix(1);
            func(token);
            return {token};
          }
          return {};
        };
      }
  }

  // Create tokenizer that accepts lower and upper case alphabets.
  template<typename Modifier = decltype(Mod::none)>
    constexpr auto alphabets(Modifier&& func = Mod::none) noexcept
    {
      return impl::single_char_tokenizer(
          [func = std::forward<Modifier>(func)](char c) -> bool {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
          }, func);
    }

  // Create a tokenizer that accepts only lower case alphabets.
  template<typename Modifier = decltype(Mod::none)>
    constexpr auto lower_alphabets(Modifier&& func = Mod::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return c >= 'a' && c <= 'z';
          }, func);
    }

  // Create a tokenizer that accepts only upper case alphabets.
  template<typename Modifier = decltype(Mod::none)>
    constexpr auto upper_alphabets(Modifier&& func = Mod::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return c >= 'A' && c <= 'Z';
          }, func);
    }

  // Create a tokenizer that accepts only decimal digits.
  template<typename Modifier = decltype(Mod::none)>
    constexpr auto decimal_digits(Modifier&& func = Mod::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return c >= '0' && c <= '9';
          }, func);
    }

  // Create a tokenizer that accepts only hexadecimal digits.
  template<typename Modifier = decltype(Mod::none)>
    constexpr auto hex_digits(Modifier&& func = Mod::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
          }, func);
    }

  // Create a tokenizer that accepts whitespace.
  template<typename Modifier = decltype(Mod::none)>
    constexpr auto whitespace(Modifier&& func = Mod::none) noexcept
    {
      return impl::single_char_tokenizer(
          [](char c) -> bool {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r';
          }, func);
    }

  // Create a tokenizer to match a character 'c'.
  template<typename Modifier = decltype(Mod::none)>
    constexpr auto char_token(char c, Modifier&& func = Mod::none) noexcept
    {
      return impl::single_char_tokenizer(
          [c, func = std::forward<Modifier>(func)](char ch) -> bool {
            return c == ch;
          }, func);
    }

  // Create a tokenizer to match a literal string pointed to by 'str'.
  template<typename Modifier = decltype(Mod::none)>
    constexpr auto str_token(CT::string_view str, Modifier&& func = Mod::none) noexcept
    {
      return [=, func = std::forward<Modifier>(func)](Input& input) -> Token {
        if (!input.starts_with(str))
          return {};
        input.remove_prefix(str.size());
        func(str);
        return {str};
      };
    }

  // Create a tokenizer that matches a single character out of the group of characters provided.
  constexpr auto any_char_of(CT::string_view char_group) noexcept
  {
    return [=](Input& input) -> Token {
      for (const auto& c : char_group)
        if (c == input[0]) {
          input.remove_prefix(1);
          return {{input, 1}};
        }
      return {};
    };
  }

  // Create a tokenizer that will match a character NOT in the group of characters provided.
  constexpr auto none_of(CT::string_view char_group) noexcept
  {
    return [=](Input& input) -> Token {
      for (const auto& c : char_group)
        if (c == input[0])
          return {};
      input.remove_prefix(1);
      return {{input, 1}};
    };
  }

  namespace impl {
    template<typename Tokenizer>
      constexpr std::size_t accumulate(Tokenizer&& tokenizer, Input input) noexcept
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

  // Create a tokenizer that matches multiple instances (zero or many) of another tokenizer.
  template<typename Tokenizer>
    constexpr auto many(Tokenizer&& tokenizer) noexcept
    {
      return [t = std::forward<Tokenizer>(tokenizer)](Input& input) -> Token {
        const auto token_size = impl::accumulate(t, input);
        return {{input, token_size}};
      };
    }

  // Create a tokenizer that matches at least one instance of another tokenizer.
  template<typename Tokenizer>
    constexpr auto at_least_one(Tokenizer&& tokenizer) noexcept
    {
      return [t = std::forward<Tokenizer>(tokenizer)](Input& input) -> Token {
        const auto input_tokenize = input;
        const auto first_token = t(input);
        if (!first_token)
          return {};
        std::size_t token_size = (*first_token).size() + impl::accumulate(t, input);
        return {{input_tokenize, token_size}};
      };
    }

  // Create a tokenizer that optionally accepts another tokenizer.
  template<typename Tokenizer>
    constexpr auto maybe(Tokenizer&& tokenizer) noexcept
    {
      return [t = std::forward<Tokenizer>(tokenizer)](Input& input) -> Token {
        const auto token = t(input);
        if (!token)
          return {{}};
        std::size_t token_size = (*token).size();
        return {{input, token_size}};
      };
    }

  // Create a tokenizer that concatenates other tokenizers in sequence.
  template<typename TokenizerL, typename TokenizerR>
    constexpr auto operator&(TokenizerL&& tl, TokenizerR&& tr) noexcept
    {
      return [tl = std::forward<TokenizerL>(tl),
             tr = std::forward<TokenizerR>(tr)](Input& input) -> Token {
               const auto input_tokenize = input;
               const auto first_token = tl(input);
               if (!first_token)
                 return {};
               const auto second_token = tr(input);
               if (!second_token)
                 return {};
               const auto token_size = (*first_token).size() + (*second_token).size();
               return {{input_tokenize, token_size}};
             };
    }

  // Create a tokenizer that alternates between other tokenizer, choosing the one that succeeds.
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
             };
    }

}

#endif
