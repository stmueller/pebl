#!/bin/bash
# Extract chapter content from Sphinx-generated LaTeX for inclusion in main PEBL manual

INPUT="build/latex/pebl-reference.tex"
OUTPUT="build/latex/reference-chapter.tex"

echo "Extracting chapter content from $INPUT..."

# Create header
cat > "$OUTPUT" << 'HEADER'
% This file is auto-generated from Sphinx documentation
% To regenerate: cd sphinx-prototype && make latex && bash extract_chapter.sh
% Source files: source/reference/

\chapter{Detailed Function and Keyword Reference}
\label{sec:reference}

\setlength{\parindent}{0pt}

HEADER

# Extract content (line 88 to line before "Indices and tables")
sed -n '88,14750p' "$INPUT" >> "$OUTPUT"

echo "Generated: $OUTPUT"
echo "File size: $(du -h "$OUTPUT" | cut -f1)"
echo ""
echo "To use this in the main PEBL manual:"
echo "  1. Backup the old reference.tex:"
echo "     cp ../pman/reference.tex ../pman/reference.tex.backup"
echo "  2. Copy the new file:"
echo "     cp $OUTPUT ../pman/reference.tex"
echo "  3. Build the main manual:"
echo "     cd ../pman && pdflatex main.tex"
