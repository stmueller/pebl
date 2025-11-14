#!/bin/bash
# Post-process Sphinx LaTeX output to use standard LaTeX commands
# This removes Sphinx-specific macros and converts them to standard equivalents

INPUT="build/latex/reference-chapter.tex"
OUTPUT="build/latex/reference-chapter-clean.tex"
TEMP="build/latex/temp_clean.tex"

echo "Cleaning Sphinx commands from $INPUT..."

# Read the input file and apply transformations
cat "$INPUT" | \

# Remove \sphinxAtStartPar (just marks paragraph start, not needed)
sed 's/\\sphinxAtStartPar//g' | \

# Convert \sphinxstylestrong{text} to \textbf{text}
sed 's/\\sphinxstylestrong{\([^}]*\)}/\\textbf{\1}/g' | \

# Convert \sphinxstyleemphasis{text} to \emph{text}
sed 's/\\sphinxstyleemphasis{\([^}]*\)}/\\emph{\1}/g' | \

# Convert \sphinxcode{\sphinxupquote{text}} to \texttt{text}
sed 's/\\sphinxcode{\\sphinxupquote{\([^}]*\)}}/\\texttt{\1}/g' | \

# Convert remaining \sphinxcode{text} to \texttt{text}
sed 's/\\sphinxcode{\([^}]*\)}/\\texttt{\1}/g' | \

# Convert \sphinxhyphen{} to just a hyphen
sed 's/\\sphinxhyphen{}/\\-/g' | \

# Convert \DUrole{doc}{text} to just text
sed 's/\\DUrole{doc}{\([^}]*\)}/\1/g' | \

# Convert \sphinxcrossref{text} to just text (hyperref will handle links)
sed 's/\\sphinxcrossref{\([^}]*\)}/\1/g' | \

# Handle Pygments syntax highlighting - convert to basic verbatim style
sed 's/\\PYG{[^}]*}{\([^}]*\)}/\1/g' | \

# Convert sphinxVerbatim to standard verbatim
sed 's/\\begin{sphinxVerbatim}\[commandchars=\\\\\\{\\}\]/\\begin{verbatim}/g' | \
sed 's/\\end{sphinxVerbatim}/\\end{verbatim}/g' | \

# Remove \phantomsection (hyperref will handle this automatically)
sed 's/\\phantomsection//g' | \

# Simplify label references - remove \detokenize
sed 's/\\label{\\detokenize{\([^}]*\)}}/\\label{\1}/g' | \
sed 's/\\hyperref\[\\detokenize{\([^}]*\)}\]/\\hyperref[\1]/g' | \

# Remove \ignorespaces
sed 's/\\ignorespaces//g' | \

# Remove \index entries with @ symbols (Sphinx-specific)
sed 's/\\index{[^}]*@\\spxentry{[^}]*}}//g' \

> "$TEMP"

# Move temp to output
mv "$TEMP" "$OUTPUT"

echo "Generated: $OUTPUT"
echo "File size: $(du -h "$OUTPUT" | cut -f1)"
echo ""
echo "Cleaned the following Sphinx commands:"
echo "  - \\sphinxAtStartPar"
echo "  - \\sphinxstylestrong → \\textbf"
echo "  - \\sphinxstyleemphasis → \\emph"
echo "  - \\sphinxcode → \\texttt"
echo "  - \\sphinxVerbatim → verbatim"
echo "  - Pygments highlighting → plain text"
echo "  - Label/hyperref detokenize"
echo ""
echo "To use this in the main PEBL manual:"
echo "  1. Backup the old reference.tex:"
echo "     cp ../pman/reference.tex ../pman/reference.tex.backup"
echo "  2. Copy the new file:"
echo "     cp $OUTPUT ../pman/reference.tex"
echo "  3. Build the main manual:"
echo "     cd ../pman && pdflatex main.tex"
