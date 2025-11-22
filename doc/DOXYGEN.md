# Generating PEBL C++ Code Documentation with Doxygen

This directory contains the Doxygen configuration for generating HTML, PDF, and man page documentation from the PEBL C++ source code.

## Prerequisites

Install Doxygen:
```bash
sudo apt-get install doxygen graphviz  # Ubuntu/Debian
```

## Generating Documentation

### Option 1: Using Makefile (Recommended)

From the project root directory:

```bash
make dox
```

### Option 2: Running Doxygen Directly

From the `doc/` directory:

```bash
cd doc
doxygen Doxyfile
```

This will generate documentation in the `doc/dox/` directory:
- **HTML**: `doc/dox/html/index.html` (open in browser)
- **LaTeX/PDF**: `doc/dox/latex/` (run `make` to build PDF)
- **RTF**: `doc/dox/rtf/`
- **Man pages**: `doc/dox/man/`

## Configuration

The `Doxyfile` configuration:
- **Project**: PEBL version 2.3
- **Input**: All C++ source files in `../src/` (recursively)
- **Excludes**: Generated parser files (grammar.tab.*, lex.yy.c)
- **Output**: HTML, LaTeX, RTF, and man pages

## Customization

To modify the documentation settings, edit `Doxyfile`. Key settings:
- `PROJECT_NUMBER`: Update version number
- `INPUT`: Source directories to document
- `EXCLUDE`: Files/directories to skip
- `GENERATE_HTML/LATEX/RTF/MAN`: Enable/disable output formats

For more information, see: https://www.doxygen.nl/manual/
