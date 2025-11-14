# Makefile for Sphinx documentation
# PEBL Function Reference Prototype

# You can set these variables from the command line
SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = source
BUILDDIR      = build

# Put it first so that "make" without argument is like "make help"
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile html latex latex-chapter pdf clean all

# Build HTML documentation
html:
	@echo "Building HTML documentation..."
	@$(SPHINXBUILD) -b html "$(SOURCEDIR)" "$(BUILDDIR)/html" $(SPHINXOPTS)
	@echo ""
	@echo "Converting function references to hyperlinks..."
	@python3 make_func_links.py
	@echo ""
	@echo "Build finished. The HTML pages are in $(BUILDDIR)/html"
	@echo "Open $(BUILDDIR)/html/index.html in your browser"

# Build LaTeX/PDF documentation
latex:
	@echo "Building LaTeX documentation..."
	@$(SPHINXBUILD) -b latex "$(SOURCEDIR)" "$(BUILDDIR)/latex" $(SPHINXOPTS)
	@echo ""
	@echo "Build finished. LaTeX files are in $(BUILDDIR)/latex"
	@echo "Generated file: $(BUILDDIR)/latex/pebl-reference.tex"

# Extract chapter for inclusion in main PEBL manual (with Sphinx style conversion)
latex-chapter: latex
	@echo ""
	@echo "Extracting chapter content for main PEBL manual..."
	@bash extract_chapter.sh
	@echo ""
	@python3 cleanup_latex.py

# Extract chapter with Sphinx styles (simpler, no conversion needed)
latex-chapter-with-styles: latex
	@echo ""
	@echo "Extracting chapter with Sphinx style files..."
	@python3 extract_with_styles.py

# Build PDF (requires pdflatex)
pdf: latex
	@echo "Building PDF from LaTeX..."
	@cd $(BUILDDIR)/latex && pdflatex pebl-reference.tex
	@cd $(BUILDDIR)/latex && pdflatex pebl-reference.tex
	@echo ""
	@echo "PDF build finished: $(BUILDDIR)/latex/pebl-reference.pdf"

# Clean build files
clean:
	@rm -rf $(BUILDDIR)
	@echo "Cleaned build directory"

# Build everything
all: html pdf

# Catch-all target: route all unknown targets to Sphinx
%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
