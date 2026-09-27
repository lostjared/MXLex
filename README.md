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
