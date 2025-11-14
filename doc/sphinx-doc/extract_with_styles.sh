#!/bin/bash
# Extract chapter content and prepare it with Sphinx style files

INPUT="build/latex/pebl-reference.tex"
OUTPUT="build/latex/reference-chapter.tex"
OUTPUT_DIR="../pman"

echo "Extracting chapter content from $INPUT..."

# Create header that loads Sphinx styles
cat > "$OUTPUT" << 'HEADER'
% This file is auto-generated from Sphinx documentation
% To regenerate: cd sphinx-prototype && make latex-chapter-with-styles
% Source files: source/reference/

% IMPORTANT: This file requires Sphinx style files to be in the same directory
% The following .sty files must be present:
%   - sphinx.sty (main style file)
%   - sphinxhighlight.sty
%   - sphinxlatex*.sty (various modules)
%   - sphinxpackage*.sty (various packages)
%   - sphinxmessages.sty

% Load required packages (most are already in main.tex but repeat for safety)
\usepackage{color}
\usepackage{fancyvrb}
\usepackage{titlesec}

% Load Sphinx style package (this loads all the sub-packages)
\usepackage{sphinx}

\chapter{Detailed Function and Keyword Reference}
\label{sec:reference}

\setlength{\parindent}{0pt}

HEADER

# Extract content (line 88 to line before "Indices and tables")
sed -n '88,14750p' "$INPUT" >> "$OUTPUT"

echo "Generated: $OUTPUT"
echo "File size: $(du -h "$OUTPUT" | cut -f1)"
echo ""
echo "Copying Sphinx style files to $OUTPUT_DIR..."

# Copy all Sphinx style files to pman directory
cp build/latex/sphinx*.sty "$OUTPUT_DIR/" 2>/dev/null
num_files=$(ls build/latex/sphinx*.sty 2>/dev/null | wc -l)

echo "Copied $num_files Sphinx style files"
echo ""
echo "To use this in the main PEBL manual:"
echo "  1. Backup the old reference.tex:"
echo "     cp $OUTPUT_DIR/reference.tex $OUTPUT_DIR/reference.tex.backup"
echo "  2. Copy the new file:"
echo "     cp $OUTPUT $OUTPUT_DIR/reference.tex"
echo "  3. The style files are already copied to $OUTPUT_DIR"
echo "  4. Build the main manual:"
echo "     cd $OUTPUT_DIR && pdflatex main.tex"
