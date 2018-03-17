# Lexical Tokenizer	{#mainpage}
An experimental header-only C++ library that provides a toolkit to build lexical tokenization routines that could potentially be run at compile time. It is based on inspiration drawn from [here](https://fsharpforfunandprofit.com/posts/understanding-parser-combinators/) and [here](https://www.youtube.com/watch?v=PJwd4JLYJJY). The library relies on a fairly modern version of the language (C++17), such as (but not limited to):
* [`std::string_view`](http://en.cppreference.com/w/cpp/string/basic_string_view)
* [`std::is_invocable_r_v`](http://en.cppreference.com/w/cpp/types/is_invocable)
* [`constexpr` lambda](http://en.cppreference.com/w/cpp/language/lambda)
* [`constexpr std::tuple`](http://en.cppreference.com/w/cpp/utility/tuple)
* [`constexpr std::optional`](http://en.cppreference.com/w/cpp/utility/optional)
* [Fold expressions](http://en.cppreference.com/w/cpp/language/fold)
* [`if` intializers](http://en.cppreference.com/w/cpp/language/if)

While it is probably possible to write the library without the above features, it would likely be very verbose and difficult to follow.

As of Q1 2018, most compilers are still implementing the remaining parts of the C++17 standard. In particular, `std::string_view` is still not `constexpr` in GCC 7.2. Therefore, an auxiliary header-only library (`ct_lib.h`) is provided to make up for the missing functionality. Using this library, references to `std::string_view` are replaced with `CT::string_view`. Once compilers are fully C++17 compliant, `ct_lib.h` will no longer be necessary.

## Building blocks
This toolkit provides basic building blocks which are used to make non-trivial tokenizers. By default, all blocks only attempt to match. However, this functionality can be changed by passing in a callable object. Character class and literal matchers do the actual matching whereas modifiers combine / change the behavior of multiple character class and literal matchers.

### Character Classes
|Character Class Matchers|Equivalent Regular Expression|
|:---:|:---:|
|`Tok::alphabet`|`[a-zA-Z]`|
|`Tok::lower_alphabet`|`[a-z]`|
|`Tok::upper_alphabet`|`[A-Z]`|
|`Tok::digit`|`[0-9]`|
|`Tok::hex_digit`|`[0-9a-fA-F]`|
|`Tok::whitespace`|`[ \t\r\n]`|
|`Tok::any`|`[.]`|
|`Tok::newline`|`[\r\n]`|

### Literal Matchers
|Literal Matcher|Equivalent Regular Expression|
|:---:|:---:|
|`Tok::any_of`|`[abcd]`|
|`Tok::none_of`|`[^abcd]`|
|`Tok::char_token`|`[a]`|
|`Tok::str_token`|`(string)`|

### Modifiers
|Modifier|Equivalent Regular Expression|
|:---:|:---:|
|`Tok::many`|`(...)*`|
|`Tok::exactly`|`(...){n}`|
|`Tok::at_least_one`|`(...)+`|
|`Tok::maybe`|`(...)?`|
|`Tok::sequence`|`(...)(...)..(...)`|
|`operator\|`|`(...)\|(...)\|..(...)`|


## Copyright
Nilangshu Bidyanta <mailto:nbidyanta@gmail.com>

Copyright (c) 2018 Nilangshu Bidyanta. All Rights Reserved.

MIT License.
