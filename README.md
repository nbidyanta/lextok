# Lexical Tokenizer	{#mainpage}
An experimental header-only C++ library that provides a toolkit to build lexical tokenization routines that could potentially be run at compile time. For certain trivial cases it could also be used to build a parser. It is based on inspiration drawn from [here](https://fsharpforfunandprofit.com/posts/understanding-parser-combinators/) and [here](https://www.youtube.com/watch?v=PJwd4JLYJJY). The library relies on fairly modern features of the language (C++17), such as (but not limited to):
* [`std::is_invocable_r_v`](http://en.cppreference.com/w/cpp/types/is_invocable)
* [`constexpr` lambda](http://en.cppreference.com/w/cpp/language/lambda)
* [`constexpr std::string_view`](http://en.cppreference.com/w/cpp/string/basic_string_view)
* [`constexpr std::tuple`](http://en.cppreference.com/w/cpp/utility/tuple)
* [`constexpr std::optional`](http://en.cppreference.com/w/cpp/utility/optional)
* [`if` intializers](http://en.cppreference.com/w/cpp/language/if)

The main motivation to write this in C++17 was personal -- it was to gain a better understanding of the latest C++ standard and to pull up my skills and mindset from the level of C. It should be possible to write something similar with older standards. However, it might be more verbose and possibly lack compile-time evaluation.


## Prerequisites
The following prerequisites must be satisfied in order to use the library, build & run the tests, and generate documentation:
* cmake (>= 3.10)
* make
* gcc (>=7.3) or a C++17 capable compiler
* doxygen (>= 1.8.11)
* dot


## Building and testing
This project uses CMake to generate the `Makefiles` and `make` to build the test and example binaries. Since the library itself is header only, no binaries are generated specifically for it. However, if the application integrating it also uses CMake, it should be possible to treat the header file as a library target (named `lextok`).

To build & run tests and generate documentation, execute the following in the root directory of the project:
~~~
$ cmake . -Bbuild -DENABLE_LEXTOK_TESTS=ON -DBUILD_LEXTOK_DOCS=ON -DBUILD_LEXTOK_EXAMPLES=ON
$ cd build
$ make              # Builds the tests and examples
$ make tests        # Runs the tests
~~~


## Generating documentation
Once inside the `build` directory, execute the following to generate the documentation.
~~~
$ make lextok-docs
~~~
The documentation is generated in HTML format in `build/docs/lextok/html`. Look for `index.html`.


## Other options
Some useful options that might come in handy:
~~~
$ make help     # List all targets that can be invoked via make
~~~
~~~
$ cmake -LH ..  # Display a list of options that were set for this build
~~~


## Building blocks
This toolkit provides basic building blocks which are used to make non-trivial tokenizers. By default, all blocks only attempt to match. However, this functionality can be changed by passing a callable object to the tokenizer directly, or mapping a callable object to the result of a tokenizer. Character class and literal matchers do the actual matching whereas modifiers combine / change the behavior of the matchers.

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
`Tok::Token`s are an optionally populated type `std::optional<std::string_view>`. A `Tok::Input` and `Tok::Token_view` are aliases of `std::string_view`. Tokenizers are callable objects of the type `Tok::Token (Tok::Input&)`. Maps are callable objects of the type `void (Tok::Token_view)`.

A tokenizer is called on an input. Depending on whether the tokenizer succeeded or failed, it returns a `Tok::Token` or `std::nullopt` respectively. Used this way, the tokenizer only validates the input. However, in order to do anything interesting with the matched tokens, the tokenizers expect a Map type to be passed to them.

For complete examples, look at the source files under `examples/`. Following is an example showing how to extract the content of a quoted string.
~~~.cpp
std::string str;                  // Will hold the extracted string
std::string input = "\"this is a string\"";
Input input_view(input);

// Construct a tokenizer to validate the input is a quoted string.
// Here, a quoted string is defined as at least one instance of
// any character (other than a double quote) between two double quotes.
// Notice how Tok::at_least_one is being passed a Map in the form of a
// lambda to extract the actual string without the quotes.
const auto quoted_string_tokenizer =
    Tok::char_token('"') &
    Tok::at_least_one(
        Tok::none_of("\""),
        [&str](Tok::Token_view token) {str = token;}
    ) &
    Tok::char_token('"');

// Apply the tokenizer to the input
const auto token = quoted_string_tokenizer(input_view);

// On failure to find a quoted string, 'token' will hold 'std::nullopt'
if (!token) {
    std::cout << "No quoted string found\n";
    return 1;
}

// 'token' now references the full match of the tokenizer, i.e. \"this is a string\".
// However, 'str' contains the actual string itself without the quotes,
// i.e. this is a string.
assert((*token).data() == input);
assert(str == "this is a string");
std::cout << "Assertions succeeded\n";
~~~

The library can also be used to build simple parsers. For this case, it is advisable to build up the tokenizer piece-by-piece. For more complicated cases, it would be better to tokenize the input and build a parse tree.
~~~.cpp
std::string IP;                 // Will hold the extracted IP address
std::string input = "\r\n+CGPADDR: 128.14.178.01\r\n";
Tok::Input input_view(input);

// Create a parser for the response of an AT command from a modem.
// Here's the EBNF the following code replicates:
// DIGIT                  := '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';
// ipv4_octet             := DIGIT+;
// ipv4_dotted_octet      := '.' ipv4_octet;
// ipv4_addr              := ipv4_octet ipv4_dotted_octet ipv4_dotted_octet ipv4_dotted_octet;
// guard                  := {'\r' | \n'} {'\r' | '\n'};
// cmd_CGPADDR            := '+CGPADDR: ';
// at_CGPADDR_cmd_parser  := guard cmd_CGPADDR ipv4_addr guard
const auto ipv4_octet = Tok::at_least_one(Tok::digit());
const auto ipv4_dotted_octet = Tok::char_token('.') & ipv4_octet;
const auto ipv4_addr = Tok::map(ipv4_octet &  Tok::exactly(ipv4_dotted_octet, 3),
    [&IP](Tok::Token_view token) { IP = token; });
const auto guard = Tok::exactly(Tok::newline(), 2);
const auto cmd_CGPADDR = Tok::str_token("+CGPADDR: ");
const auto at_CGPADDR_cmd_parser = guard & cmd_CGPADDR & ipv4_addr & guard;

// Apply the tokenizer to the input
const auto token = at_CGPADDR_cmd_parser(input_view);

// On failure to find a quoted string, 'token' will hold 'std::nullopt'
if (!token) {
    std::cout << "Unable to parse IP address\n";
    return 1;
}

std::cout << "Whole token : " << *token << "\n";
std::cout << "IP address : " << IP << "\n";
~~~

Every application of the tokenizer on the input consumes a part of it only if it succeeds in matching it. If the tokenizer fails to match the input, it is left unconsumed. Instead of applying `at_CGPADDR_cmd_parser` to the input, it is also possible to apply the constituent tokenizers separately to the input as follows:
~~~.c
// 'input_view' contains the entire input.
const auto t1 = guard(input_view);
const auto t2 = cmd_CGPADDR(input_view);
const auto t3 = ipv4_addr(input_view);
const auto t4 = guard(input_view);

// Ensure all tokenizers matched
if (!t1 || !t2 || !t3 || !t4) {
    std::cout << "Failed to match a tokenizer\n";
    return 1;
}

// 'input_view' is now empty since all of it was matched.
assert(*t1 == "\r\n");
assert(*t2 == "+CGPADDR: ");
assert(*t3 == "128.14.178.01");
assert(*t4 == "\r\n");
~~~

## Copyright
Nilangshu Bidyanta <mailto:nbidyanta@gmail.com>

Copyright (c) 2018 Nilangshu Bidyanta. All Rights Reserved.

MIT License.
