# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a C++23 implementation of [sqlgen](https://github.com/getml/sqlgen) that uses [glaze](https://github.com/stephenberry/glaze) for reflection instead of reflect-cpp. The project aims to provide SQL query generation capabilities using glaze's compile-time reflection features.

## Build System Architecture

### Meson + UV Hybrid Setup

This project uses **Meson** for C++ builds and **uv** for Python tooling dependencies (meson, ninja, pytest-cpp). This dual setup is intentional:

- `pyproject.toml` defines Python dependencies only (no C++ code is packaged as Python)
- `meson.build` handles all C++ compilation, dependencies, and tests
- Python tools (meson, pytest-cpp) are installed via uv for consistent tooling

### Subproject Dependency Wrapping

All C++ dependencies are fetched as Meson subprojects. Since glaze, sqlgen, and googletest don't provide native Meson support, custom build files are provided in `subprojects/packagefiles/`:

- `subprojects/packagefiles/glaze/meson.build` - Header-only wrapper for glaze
- `subprojects/packagefiles/sqlgen/meson.build` - Header-only wrapper for sqlgen
- `subprojects/packagefiles/gtest/meson.build` - Compiles GoogleTest/GMock from source

The `.wrap` files reference these via `patch_directory` directives. Meson automatically applies the custom build files when extracting subprojects.

**Important**: When cleaning build artifacts, you may need to delete `subprojects/*-*/` directories (e.g., `glaze-6.0.2/`) to force re-extraction with updated packagefiles.

## Development Workflow

### Initial Setup

```bash
# Create Python virtual environment and install build tools
uv venv
source .venv/bin/activate
uv pip install meson ninja pytest pytest-cpp

# Configure and build
meson setup builddir
meson compile -C builddir
```

### Building

```bash
# Full build
meson compile -C builddir

# Clean build (remove builddir and reconfigure)
rm -rf builddir && meson setup builddir && meson compile -C builddir

# Clean subprojects (if wrap files or packagefiles change)
rm -rf subprojects/*-*/ builddir
meson setup builddir
```

### Testing

```bash
# Run all tests with meson
meson test -C builddir

# Run tests with verbose output
meson test -C builddir -v

# Run specific test by name
meson test -C builddir glz_sqlgen_tests

# Alternative: Run tests via pytest-cpp
pytest
```

### Adding New Source Files

1. Add `.cpp` file to `src/` directory
2. Add corresponding header to `include/glz_sqlgen/`
3. Update `sources` in `meson.build`:
   ```meson
   sources = files(
     'src/query_builder.cpp',
     'src/your_new_file.cpp',
   )
   ```

### Adding New Tests

1. Create `test_*.cpp` file in `tests/` directory
2. Update `test_sources` in `tests/meson.build`:
   ```meson
   test_sources = files(
     'test_query_builder.cpp',
     'test_your_new_feature.cpp',
   )
   ```
3. Tests automatically use Google Test framework with `gtest_main`

## Code Architecture

### Namespace Convention

All code lives in the `glz_sqlgen` namespace. This distinguishes this implementation from the original sqlgen library.

### Current Implementation Status

The codebase currently contains a basic `QueryBuilder` class as a demonstration/template. The main work ahead is:

1. Study sqlgen's API and implementation patterns
2. Implement equivalent functionality using glaze reflection instead of reflect-cpp
3. Leverage glaze's compile-time reflection (`glz::meta`) for type introspection
4. Maintain API compatibility with sqlgen where possible

### Key Dependency Details

- **glaze v6.0.2**: Header-only JSON/reflection library with C++20/23 support
- **sqlgen v0.4.0**: Reference implementation (uses reflect-cpp); study for API design
- **C++23**: Required for latest language features; compiled with `-std=c++23`

## Troubleshooting

### Subproject Build Failures

If you see "Subproject exists but has no meson.build file":
1. Check that `patch_directory` is set in the `.wrap` file
2. Verify `subprojects/packagefiles/{name}/meson.build` exists
3. Clean and rebuild: `rm -rf subprojects/*-*/ builddir && meson setup builddir`

### Dependency Changes

When updating wrap file versions or packagefiles:
```bash
# Force re-download and extraction
rm -rf subprojects/.wraplock subprojects/packagecache subprojects/*-*/
meson setup builddir --wipe
```
