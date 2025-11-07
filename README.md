# glz-sqlgen

A C++23 implementation of [sqlgen](https://github.com/getml/sqlgen) using [glaze](https://github.com/stephenberry/glaze) instead of reflect-cpp.

## Build System

This project uses Meson build system with C++23 standard.

## Setup

### Install Python dependencies with uv

```bash
uv venv
source .venv/bin/activate  # On Windows: .venv\Scripts\activate
uv pip install -e .
```

### Configure and build with Meson

```bash
meson setup builddir
meson compile -C builddir
```

### Run tests with Meson

```bash
meson test -C builddir
```

### Run tests with pytest-cpp

```bash
pytest
```

## Project Structure

```
glz-sqlgen/
├── include/glz_sqlgen/  # Public headers
├── src/                 # Source files
├── tests/               # Test files (Google Test)
├── subprojects/         # Meson wrap files
│   ├── glaze.wrap       # glaze v6.0.2
│   ├── sqlgen.wrap      # sqlgen v0.4.0
│   └── gtest.wrap       # googletest v1.17.0
├── meson.build          # Main build configuration
└── pyproject.toml       # Python/uv configuration
```

## Dependencies

- C++23 compatible compiler
- Meson >= 1.3.0
- [glaze](https://github.com/stephenberry/glaze) v6.0.2 (auto-fetched)
- [sqlgen](https://github.com/getml/sqlgen) v0.4.0 (auto-fetched)
- [googletest](https://github.com/google/googletest) v1.17.0 (auto-fetched)

## License

See LICENSE file for details.