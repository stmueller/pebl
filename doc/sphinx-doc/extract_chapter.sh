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

# Extract content: from line after \pagestyle{normal} to line before \chapter{Indices and tables}
# \pagestyle{normal} appears after \sphinxmaketitle and \sphinxtableofcontents — skip those
start_line=$(grep -n '\\pagestyle{normal}' "$INPUT" | head -1 | cut -d: -f1)
end_line=$(grep -n '\\chapter{Indices and tables}' "$INPUT" | head -1 | cut -d: -f1)
start_line=$((start_line + 1))
end_line=$((end_line - 1))
echo "Extracting lines $start_line to $end_line..."
sed -n "${start_line},${end_line}p" "$INPUT" >> "$OUTPUT"

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
