# PEBL Function Reference - Sphinx Prototype

This is a prototype demonstrating how to use Sphinx to generate both:
1. **Searchable HTML documentation** for the web
2. **LaTeX chapter** to include in the PEBL manual PDF

## Prerequisites

Install Sphinx and the ReadTheDocs theme:

```bash
pip install sphinx sphinx-rtd-theme
```

Or use the default theme (no installation needed) by editing `source/conf.py`:
```python
html_theme = 'alabaster'  # Instead of 'sphinx_rtd_theme'
```

## Project Structure

```
sphinx-doc/
├── source/
│   ├── index.rst              # Main documentation index
│   ├── conf.py                # Sphinx configuration
│   └── functions/             # Function documentation by category
│       ├── graphics.rst       # Graphics functions
│       ├── sound.rst          # Sound functions
│       ├── input.rst          # Input functions
│       └── math.rst           # Math functions
├── Makefile                   # Build automation
├── build/                     # Generated output (created on build)
│   ├── html/                  # HTML documentation
│   └── latex/                 # LaTeX output
└── README.md                  # This file
```

## Building Documentation

### Build HTML (with search functionality)

```bash
make html
```

Then open `build/html/index.html` in your browser.

Features:
- Full-text search
- Function index
- Cross-references between functions
- Clean, responsive design

### Build LaTeX Chapter

```bash
make latex
```

This generates `build/latex/pebl-reference.tex` which can be included in the main PEBL manual:

```latex
% In doc/pman/main.tex:
\include{../../sphinx-doc/build/latex/pebl-reference}
```

### Build PDF (standalone)

```bash
make pdf
```

Generates `build/latex/pebl-reference.pdf`

### Clean build files

```bash
make clean
```

## How It Works

### Individual Function Files

Each category has its own `.rst` file (e.g., `graphics.rst`) containing multiple functions:

```rst
Graphics Functions
==================

MakeCanvas
----------

.. function:: MakeCanvas(width, height, color)

   Creates a new canvas widget.

   :param width: Canvas width in pixels
   :type width: integer
   :returns: Canvas object
   :rtype: Canvas

   **Example:**

   .. code-block:: pebl

      canvas <- MakeCanvas(800, 600, MakeColor("white"))
```

### Cross-References

Link between functions using `:func:` role:

```rst
See also: :func:`AddObject`, :func:`MakeColor`
```

These work in both HTML (as clickable links) and LaTeX (as references).

### Code Blocks

Use `.. code-block:: pebl` for syntax-highlighted examples:

```rst
.. code-block:: pebl

   x <- Random()
   Print(x)
```

## Integration with PEBL Manual

To integrate the generated LaTeX into the existing PEBL manual:

1. Build the LaTeX output: `make latex`

2. Copy or link the generated file to the manual directory:
   ```bash
   cp build/latex/pebl-reference.tex ../pman/reference.tex
   ```

3. The main PEBL manual (`doc/pman/main.tex`) already includes this:
   ```latex
   \include{reference}
   ```

4. Build the full manual as usual:
   ```bash
   cd ../pman
   pdflatex main.tex
   ```

## Next Steps

After testing this prototype:

1. **Convert existing functions** from `reference.tex` to Sphinx format
2. **Analyze source code** to identify undocumented functions
3. **Organize functions** into logical categories
4. **Set up automated build** that regenerates `reference.tex` when functions change
5. **Deploy HTML** to PEBL website or GitHub Pages

## Benefits

✅ **Single source** - Maintain function docs in one place
✅ **Searchable** - Full-text search in HTML version
✅ **Cross-references** - Automatic links between related functions
✅ **Quality output** - Both HTML and LaTeX look professional
✅ **Easy maintenance** - Simple restructuredText format
✅ **Index generation** - Automatic function index
✅ **Version control friendly** - Plain text files work great with git
