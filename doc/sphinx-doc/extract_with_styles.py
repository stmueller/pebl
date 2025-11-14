#!/usr/bin/env python3
"""
Extract chapter content with Sphinx style files for inclusion in main PEBL manual.
This approach keeps Sphinx macros and just copies the style files.
"""

import os
import shutil
import glob

def extract_with_styles():
    """Extract chapter and copy Sphinx style files."""

    input_file = 'build/latex/pebl-reference.tex'
    output_file = 'build/latex/reference-chapter-with-styles.tex'
    pman_dir = '../pman'

    print(f"Extracting chapter content from {input_file}...")

    # Create header
    header = r'''% This file is auto-generated from Sphinx documentation
% To regenerate: cd sphinx-prototype && make latex-chapter-with-styles
% Source files: source/reference/

% IMPORTANT: This file requires the Sphinx package to be loaded in the preamble.
% Add this line to main.tex BEFORE \begin{document}:
%   \usepackage{sphinx}
%
% The sphinx.sty file and its dependencies must be in the same directory as main.tex
% (21 .sty files are automatically copied by the build process)

\chapter{Detailed Function and Keyword Reference}
\label{sec:reference}

% Suppress section marks in headers for this chapter
% This prevents section titles from appearing in running headers
\renewcommand{\sectionmark}[1]{}

\setlength{\parindent}{0pt}

'''

    # Read input and extract content
    with open(input_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    # Extract lines 88 to 14750 (before "Indices and tables")
    content_lines = lines[87:14750]  # 0-indexed, so 87 = line 88

    # Fix: Convert the second \chapter command to remove it
    # The Sphinx content has "\chapter{Function Reference by Namespace}" which
    # conflicts with our main chapter. Remove it.
    content = ''.join(content_lines)

    # Remove the "Function Reference by Namespace" chapter - it's redundant
    # Just keep the intro text
    import re
    # Match across newlines with DOTALL flag
    content = re.sub(
        r'\\chapter\{Function Reference by Namespace\}\s*\n\\label\{\\detokenize\{[^}]*\}\}\s*\n',
        '',
        content,
        flags=re.DOTALL
    )

    # Write output
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(header)
        f.write(content)

    file_size = os.path.getsize(output_file)
    print(f"Generated: {output_file}")
    print(f"File size: {file_size:,} bytes")
    print()

    # Copy Sphinx style files
    print(f"Copying Sphinx style files to {pman_dir}...")
    style_files = glob.glob('build/latex/sphinx*.sty')

    if not os.path.exists(pman_dir):
        print(f"Warning: {pman_dir} does not exist!")
        return

    copied = 0
    for style_file in style_files:
        try:
            shutil.copy(style_file, pman_dir)
            copied += 1
        except Exception as e:
            print(f"Warning: Could not copy {style_file}: {e}")

    print(f"Copied {copied} Sphinx style files")
    print()
    print("To use this in the main PEBL manual:")
    print()
    print("  1. Add Sphinx package to main.tex preamble:")
    print(f"     Edit {pman_dir}/main.tex")
    print("     Add this line BEFORE \\begin{document}:")
    print("       \\usepackage{sphinx}")
    print()
    print("  2. Backup the old reference.tex:")
    print(f"     cp {pman_dir}/reference.tex {pman_dir}/reference.tex.backup")
    print()
    print("  3. Copy the new file:")
    print(f"     cp {output_file} {pman_dir}/reference.tex")
    print()
    print("  4. Build the main manual:")
    print(f"     cd {pman_dir} && pdflatex main.tex")

if __name__ == '__main__':
    try:
        extract_with_styles()
    except FileNotFoundError as e:
        print(f"Error: {e}")
        exit(1)
    except Exception as e:
        print(f"Unexpected error: {e}")
        exit(1)
