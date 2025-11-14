# PEBL Function Reference Documentation Build Guide

This directory contains the Sphinx-based documentation system for PEBL function reference.

## Quick Start

### Build HTML Documentation
```bash
make html
```
Output: `build/html/index.html`

### Build LaTeX Chapter for Main Manual
```bash
make latex-chapter
```
Output: `build/latex/reference-chapter.tex`

This generates a LaTeX chapter file that can be included in the main PEBL manual.

## Workflow

### 1. Edit Documentation Source Files

Documentation source files are in RST (reStructuredText) format:

- **Compiled Functions** (built-in C++ functions):
  - `source/reference/compiled/peblenvironment.rst`
  - `source/reference/compiled/pebllist.rst`
  - `source/reference/compiled/peblobjects.rst`
  - `source/reference/compiled/peblstream.rst`
  - `source/reference/compiled/peblstring.rst`

- **Library Functions** (PEBL .pbl files):
  - `source/reference/library/design.rst`
  - `source/reference/library/graphics.rst`
  - `source/reference/library/html.rst`
  - `source/reference/library/math.rst`
  - `source/reference/library/ui.rst`
  - `source/reference/library/utility.rst`

### 2. Build HTML for Web Viewing

```bash
make html
```

This creates a complete HTML website with:
- Navigation sidebar
- Function index
- Search functionality
- Cross-referenced function links

### 3. Build LaTeX Chapter for PDF Manual

There are **two approaches** for generating LaTeX chapters:

#### Option 1: With Sphinx Style Files (Recommended)

```bash
make latex-chapter-with-styles
```

**What it does:**
- Generates full LaTeX documentation with Sphinx commands
- Extracts chapter content
- Copies 21 Sphinx .sty files to ../pman directory
- Creates `build/latex/reference-chapter-with-styles.tex`

**Advantages:**
- Simpler (no command conversion)
- Preserves exact Sphinx formatting
- Fewer potential errors

#### Option 2: Converted to Standard LaTeX

```bash
make latex-chapter
```

**What it does:**
- Generates full LaTeX documentation
- Extracts chapter content
- Converts all Sphinx commands to standard LaTeX
- Creates `build/latex/reference-chapter-clean.tex`

**Advantages:**
- No extra .sty files needed
- Uses only standard LaTeX commands
- Smaller file size (~340 KB vs ~720 KB)

### 4. Include in Main PEBL Manual

**For Option 1 (with Sphinx styles):**
```bash
# Sphinx .sty files already copied to ../pman
cp build/latex/reference-chapter-with-styles.tex ../pman/reference.tex
cd ../pman && pdflatex main.tex
```

**For Option 2 (converted):**
```bash
cp build/latex/reference-chapter-clean.tex ../pman/reference.tex
cd ../pman && pdflatex main.tex
```

**Note:** Always run pdflatex twice for proper cross-references.

## Available Make Targets

- `make html` - Build HTML documentation
- `make latex` - Build full LaTeX documentation
- `make latex-chapter` - Build LaTeX chapter with standard LaTeX commands
- `make latex-chapter-with-styles` - Build LaTeX chapter with Sphinx styles
- `make pdf` - Build standalone PDF (requires pdflatex)
- `make clean` - Remove all build files
- `make all` - Build both HTML and PDF

## Documentation Tracking

Function documentation status is tracked in:
- `../MASTER_FUNCTION_LIST.csv`

Status values:
- `COMPLETE` - Fully documented with description, usage, examples
- `TO_DOCUMENT` - Stub exists, needs full documentation
- Other statuses indicate various stages of completion

## File Structure

```
sphinx-doc/
├── source/               # RST source files
│   ├── conf.py          # Sphinx configuration
│   ├── index.rst        # Main index page
│   └── reference/       # Function reference
│       ├── compiled/    # Built-in functions
│       └── library/     # Library functions
├── build/               # Generated output
│   ├── html/           # HTML output
│   └── latex/          # LaTeX output
├── Makefile            # Build automation
├── extract_chapter.sh  # LaTeX chapter extraction script
└── README_BUILD.md     # This file
```

## Notes

- The LaTeX chapter extraction removes document preamble and the "Indices and tables" section
- HTML build includes a post-processing step to convert function names to hyperlinks
- Both outputs are generated from the same RST source files
- Changes to RST files automatically update both HTML and LaTeX outputs
