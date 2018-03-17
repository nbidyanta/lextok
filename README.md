# Lexical Tokenizer	{#mainpage}
An experimental header-only C++ library that provides a toolkit to build lexical tokenization routines that could potentially be run at compile time. It is based on inspiration drawn from [here](https://fsharpforfunandprofit.com/posts/understanding-parser-combinators/) and [here](https://www.youtube.com/watch?v=PJwd4JLYJJY). The library relies on a fairly modern version of the language (C++17), such as (but not limited to):
* [`std::is_invocable_r_v`](http://en.cppreference.com/w/cpp/types/is_invocable)
* [`constexpr` lambda](http://en.cppreference.com/w/cpp/language/lambda)
* [`constexpr std::string_view`](http://en.cppreference.com/w/cpp/string/basic_string_view)
* [`constexpr std::tuple`](http://en.cppreference.com/w/cpp/utility/tuple)
* [`constexpr std::optional`](http://en.cppreference.com/w/cpp/utility/optional)
* [`if` intializers](http://en.cppreference.com/w/cpp/language/if)

The main motivation to write this in C++17 was personal -- it was to gain a better understanding of the latest C++ standard and to pull up my skills from the C level. It should be possible to write something similar with earlier standards, just more inconvenient and possibly without compile-time evaluation.

As of Q1 2018, most compilers are still implementing the remaining parts of the C++17 standard. In particular, `std::string_view` is still not `constexpr` in GCC 7.2. Therefore, an auxiliary header-only library (`ct_lib.h`) is provided to make up for the missing functionality. Using this library, references to `std::string_view` are replaced with `CT::string_view`. Once compilers are fully C++17 compliant, `ct_lib.h` will no longer be necessary.


## Prerequisites
The following prerequisites must be satisfied in order to use the library, build & run the tests, and generate documentation:
* CMake (>= 3.10)
* Make
* GCC (>=7.2) or a C++17 capable compiler
* Doxygen (>= 1.8.11)
* Dot


## Building and testing
This project uses CMake to generate the `Makefiles` and `make` to build the test binaries. Since the library itself is header only, no binaries are generated specifically for it. However, if the application integrating them also uses CMake, it should be possible to treat the header file as a library target (`lextok`).

To build & run tests and generate documentation, execute the following in the root directory of the project:
```
$ cmake . -Bbuild -DENABLE_LEXTOK_TESTS=ON -DBUILD_LEXTOK_DOCS=ON
$ cd build
$ make              # Builds the tests
$ make tests        # Runs the tests
```


## Generating documentation
Once inside the `build` directory, execute the following to generate the documentation.
```
$ make lextok-docs
```
The documentation is generated in HTML format in `build/docs/lextok/html`. Look for `index.html`.


## Other options
Some useful options that might come in handy:
```
$ make help     # List all targets that can be invoked via make
```
```
$ cmake -LH ..  # Display a list of options that were set for this build
```

## Building blocks
This toolkit provides basic building blocks which are used to make non-trivial tokenizers. By default, all blocks only attempt to match. However, this functionality can be changed by passing a callable object to the tokenizer directly, or mapping a callable object to a tokenizer. Character class and literal matchers do the actual matching whereas modifiers combine / change the behavior of multiple character class and literal matchers.

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

Tokenizers are concatenated by overloading `operator&`, whereas they are alternated between by overloading `operator|`.


## Examples
Usage examples go here.


## Copyright
Nilangshu Bidyanta <mailto:nbidyanta@gmail.com>

Copyright (c) 2018 Nilangshu Bidyanta. All Rights Reserved.

MIT License.
