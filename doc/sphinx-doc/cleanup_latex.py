#!/usr/bin/env python3
"""
Post-process Sphinx LaTeX output to use standard LaTeX commands.
This removes Sphinx-specific macros and converts them to standard equivalents.
"""

import re
import sys

def cleanup_sphinx_latex(input_file, output_file):
    """Clean Sphinx-specific commands from LaTeX file."""

    print(f"Cleaning Sphinx commands from {input_file}...")

    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()

    # Track original size
    original_size = len(content)

    # FIRST: Handle Pygments syntax highlighting codes
    # Do this BEFORE other replacements to avoid interference

    # Convert Pygments special character codes to actual characters
    # These can appear as \PYGZxx{} or \PYGZxx{content}
    # Note: Inside verbatim, special chars don't need escaping
    pygments_chars = {
        '\\PYGZsh': '#',          # hash
        '\\PYGZlt': '<',          # less than
        '\\PYGZgt': '>',          # greater than
        '\\PYGZhy': '-',          # hyphen
        '\\PYGZus': '_',          # underscore
        '\\PYGZdq': '"',          # double quote
        '\\PYGZsq': "'",          # single quote
        '\\PYGZbs': '\\',         # backslash (in verbatim, just use \)
        '\\PYGZpc': '%',          # percent
        '\\PYGZam': '&',          # ampersand
        '\\PYGZdl': '$',          # dollar
        '\\PYGZca': '^',          # caret
        '\\PYGZti': '~',          # tilde
        '\\PYGZob': '{',          # open brace
        '\\PYGZcb': '}',          # close brace
    }

    # Replace \PYGZxx{content} with char + content (handles both {} and {content})
    for pygments_cmd, char in pygments_chars.items():
        # Match the pattern \PYGZxx{anything} (including empty braces)
        pattern = re.escape(pygments_cmd) + r'\{([^}]*)\}'
        # Replace with: char + whatever was in braces
        replacement = char + r'\1'
        content = re.sub(pattern, replacement, content)

    # Handle remaining Pygments syntax highlighting - convert to basic text
    # This handles nested braces better
    while r'\PYG{' in content:
        content = re.sub(r'\\PYG\{[^}]*\}\{([^}]*)\}', r'\1', content)

    # Remove \sphinxAtStartPar (just marks paragraph start, not needed)
    content = re.sub(r'\\sphinxAtStartPar\s*', '', content)

    # Convert \sphinxstylestrong{text} to \textbf{text}
    content = re.sub(r'\\sphinxstylestrong\{([^}]*)\}', r'\\textbf{\1}', content)

    # Convert \sphinxstyleemphasis{text} to \emph{text}
    content = re.sub(r'\\sphinxstyleemphasis\{([^}]*)\}', r'\\emph{\1}', content)

    # Convert \sphinxcode{\sphinxupquote{text}} to \texttt{text}
    content = re.sub(r'\\sphinxcode\{\\sphinxupquote\{([^}]*)\}\}', r'\\texttt{\1}', content)

    # Convert remaining \sphinxcode{text} to \texttt{text}
    content = re.sub(r'\\sphinxcode\{([^}]*)\}', r'\\texttt{\1}', content)

    # Convert \sphinxhyphen{} to just a hyphen
    content = re.sub(r'\\sphinxhyphen\{\}', r'\\-', content)

    # Convert \DUrole{doc}{text} to just text
    content = re.sub(r'\\DUrole\{doc\}\{([^}]*)\}', r'\1', content)

    # Convert \sphinxcrossref{text} to just text (hyperref will handle links)
    content = re.sub(r'\\sphinxcrossref\{([^}]*)\}', r'\1', content)

    # Convert \sphinxstyletopictitle{text} to \textbf{text}
    content = re.sub(r'\\sphinxstyletopictitle\{([^}]*)\}', r'\\textbf{\1}', content)

    # Convert \sphinxtitleref{text} to just text
    content = re.sub(r'\\sphinxtitleref\{([^}]*)\}', r'\1', content)

    # Remove \sphinxupquote wrapper
    content = re.sub(r'\\sphinxupquote\{([^}]*)\}', r'\1', content)

    # Convert \sphinxurl{text} to \url{text}
    content = re.sub(r'\\sphinxurl\{([^}]*)\}', r'\\url{\1}', content)

    # Remove \sphinxstepscope
    content = re.sub(r'\\sphinxstepscope\s*', '', content)

    # Remove \sphinxlineitem
    content = re.sub(r'\\sphinxlineitem\s*', '', content)

    # Convert sphinxVerbatim to standard verbatim
    content = re.sub(r'\\begin\{sphinxVerbatim\}\[commandchars=\\\\\\{\\}\]',
                     r'\\begin{verbatim}', content)
    content = re.sub(r'\\end\{sphinxVerbatim\}', r'\\end{verbatim}', content)

    # Convert sphinxShadowBox to a simple box (or just remove it)
    content = re.sub(r'\\begin\{sphinxShadowBox\}', r'\\begin{center}', content)
    content = re.sub(r'\\end\{sphinxShadowBox\}', r'\\end{center}', content)

    # Convert sphinxadmonition to a simple paragraph
    # Pattern: \begin{sphinxadmonition}{type}{Title:}
    content = re.sub(r'\\begin\{sphinxadmonition\}\{[^}]*\}\{([^}]*)\}',
                     r'\\paragraph{\1}', content)
    content = re.sub(r'\\end\{sphinxadmonition\}', '', content)

    # Remove \phantomsection (hyperref will handle this automatically)
    content = re.sub(r'\\phantomsection\s*', '', content)

    # Simplify label references - remove \detokenize
    content = re.sub(r'\\label\{\\detokenize\{([^}]*)\}\}', r'\\label{\1}', content)
    content = re.sub(r'\\hyperref\[\\detokenize\{([^}]*)\}\]', r'\\hyperref[\1]', content)

    # Remove \ignorespaces
    content = re.sub(r'\\ignorespaces\s*', '', content)

    # Remove \index entries with @ symbols (Sphinx-specific)
    content = re.sub(r'\\index\{[^}]*@\\spxentry\{[^}]*\}\}\s*', '', content)

    # Write output
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(content)

    # Report results
    new_size = len(content)
    size_reduction = original_size - new_size

    print(f"Generated: {output_file}")
    print(f"File size: {new_size:,} bytes ({size_reduction:,} bytes smaller)")
    print()
    print("Cleaned the following Sphinx commands:")
    print("  - \\sphinxAtStartPar")
    print("  - \\sphinxstylestrong → \\textbf")
    print("  - \\sphinxstyleemphasis → \\emph")
    print("  - \\sphinxcode → \\texttt")
    print("  - \\sphinxVerbatim → verbatim")
    print("  - Pygments highlighting → plain text")
    print("  - Label/hyperref detokenize")
    print()
    print("To use this in the main PEBL manual:")
    print("  1. Backup the old reference.tex:")
    print("     cp ../pman/reference.tex ../pman/reference.tex.backup")
    print("  2. Copy the new file:")
    print(f"     cp {output_file} ../pman/reference.tex")
    print("  3. Build the main manual:")
    print("     cd ../pman && pdflatex main.tex")

if __name__ == '__main__':
    input_file = 'build/latex/reference-chapter.tex'
    output_file = 'build/latex/reference-chapter-clean.tex'

    try:
        cleanup_sphinx_latex(input_file, output_file)
    except FileNotFoundError as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Unexpected error: {e}", file=sys.stderr)
        sys.exit(1)
