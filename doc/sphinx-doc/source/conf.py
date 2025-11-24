# Configuration file for the Sphinx documentation builder.
# PEBL Function Reference Prototype

import os
import sys

# Add custom Pygments lexer directory to path
sys.path.insert(0, os.path.abspath('_pygments'))

# Add custom extensions directory to path
sys.path.insert(0, os.path.abspath('_ext'))

# -- Project information -----------------------------------------------------

project = 'PEBL Function Reference'
copyright = '2025, Shane T. Mueller'
author = 'Shane T. Mueller'
version = '2.1'
release = '2.1'

# -- General configuration ---------------------------------------------------

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.intersphinx',
    'sphinx.ext.todo',
    'sphinx.ext.viewcode',
    'pebl_lexer',  # Custom PEBL syntax highlighter
]

templates_path = ['_templates']
exclude_patterns = [
    'syntax_test.rst',  # Test file for syntax highlighting, not part of main docs
    'reference/index.rst',  # Duplicate index, using main index.rst instead
    'reference/pebl_math.rst',  # Duplicate of peblmath.rst
]

# The master document
master_doc = 'index'

# -- Options for HTML output -------------------------------------------------

# html_theme = 'sphinx_rtd_theme'  # ReadTheDocs theme (install with: pip install sphinx-rtd-theme)
html_theme = 'alabaster'  # Default theme (no install needed)

html_static_path = ['_static']
html_css_files = ['custom.css']
html_title = "PEBL Function Reference"

# Theme options
html_theme_options = {
    # Note: navigation_depth, collapse_navigation, sticky_navigation, and includehidden
    # are sphinx_rtd_theme options. Using alabaster theme instead.
    'extra_nav_links': {
        'PEBL Documentation': '../documentation.html',
        'Function Index': '../genindex.html',
    }
}

# -- Options for LaTeX output ------------------------------------------------

latex_engine = 'pdflatex'

# LaTeX customization
latex_elements = {
    'papersize': 'letterpaper',
    'pointsize': '10pt',
    'preamble': r'''
\usepackage{booktabs}
\usepackage{longtable}
''',
    'figure_align': 'htbp',
}

# Generate just the function reference as a standalone LaTeX file
# that can be included in the main PEBL manual
latex_documents = [
    ('index', 'pebl-reference.tex', 'PEBL Function Reference',
     'Shane T. Mueller', 'manual', False),
]

# LaTeX domain indices (function index, etc.)
latex_domain_indices = True

# -- Options for code highlighting -------------------------------------------

# Syntax highlighting style
pygments_style = 'sphinx'

# Default language for code blocks
highlight_language = 'pebl'
