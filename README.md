# MXLex

MXLex is a simple lexical analyzer for a C-dialect language.

## Requirements

- A C++23 compiler
- Pcons through [uv](https://docs.astral.sh/uv/), or CMake 3.10+

## Building with Pcons

From the MXLex repository:

```bash
uvx pcons -B build/pcons --reconfigure
```

This builds the static `mxlex` library and the `test-lex` command-line
program. To scan a source file:

```bash
./build/pcons/test-lex /path/to/source-file
```

Use `VARIANT=debug` for a debug build or `PROGRAMS=0` to build only the
library:

```bash
uvx pcons -B build/pcons VARIANT=debug --reconfigure
uvx pcons -B build/pcons PROGRAMS=0 --reconfigure
```

### Installation

The default installation is staged under `dist`:

```bash
uvx pcons -B build/pcons all install
```

To install to another writable prefix, set both the staging prefix and the
prefix recorded in the generated `mxlex.pc` pkg-config file:

```bash
uvx pcons -B build/pcons \
    PCONS_INSTALL_PREFIX=/path/to/prefix \
    PCONS_FINAL_PREFIX=/path/to/prefix \
    all install
```

For a non-standard prefix, downstream Pcons projects can locate MXLex with
`PREFIX=/path/to/prefix`. Other build systems can add
`/path/to/prefix/lib/pkgconfig` to `PKG_CONFIG_PATH`.

## Building with CMake

```bash
cmake -S . -B build/cmake
cmake --build build/cmake
cmake --install build/cmake --prefix /path/to/prefix
```

## Recognized syntax

- ASCII identifiers (including underscores); keywords such as `int`, `if`, and
  `return` remain `IDENTIFIER` tokens so a parser can choose its own dialect.
- C punctuation and operators, including compound assignments, shifts, `->`,
  `...`, `#`, and `##`. Alternative spellings `<:`, `:>`, `<%`, `%>`, `%:`, and
  `%:%:` retain their spelling and share the corresponding punctuation's
  `CHAR_TYPE`. Existing C++ operators `::`, `.*`, `->*`, and `<=>` are supported.
- Decimal, octal, hexadecimal, and binary integers with `u`/`U`, `l`/`L`, and
  `ll`/`LL` suffix combinations; decimal and hexadecimal floating literals
  with exponents and optional `f`/`F` or `l`/`L` suffixes.
- Quoted strings (`STRING_VALUE`) and characters (`CHARACTER_VALUE`), including
  escaped quotes and the `L`, `u`, `U`, and `u8` prefixes. Token text includes
  prefixes, quotes, and escape spellings; escapes are not decoded or checked
  for their semantic value. Adjacent string literals remain separate tokens.
- Line and block comments, all six ASCII whitespace characters, and
  backslash-newline continuations. LF, CRLF, and CR line endings are accepted.
  Token line numbers refer to the first physical source line of each token.

Malformed numbers, empty character literals, unterminated literals, and
unterminated block comments throw `mx::ScannerError`. Unknown bytes return
`TOKEN_ERROR`. The command-line scanner exits unsuccessfully for either error.

This library tokenizes a C-style dialect; it does not parse declarations or
expressions, expand macros, evaluate directives, or specially tokenize include
header names. Unicode identifiers, universal character names in identifiers,
C23 digit separators and additional numeric suffixes, and C++ raw strings are
not currently supported.

## Tests

The CMake build enables scanner regression tests by default:

```bash
cmake -S . -B build/cmake -DBUILD_TESTING=ON
cmake --build build/cmake
ctest --test-dir build/cmake --output-on-failure
```
