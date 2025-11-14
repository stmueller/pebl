#!/usr/bin/env python3
"""
Post-process Sphinx HTML to convert :func: references into actual hyperlinks.

This script:
1. Scans all HTML files to find function section IDs
2. Updates all HTML files to convert <code class="xref py py-func"> to proper <a> tags
"""

import re
import os
from pathlib import Path
from html.parser import HTMLParser


class FunctionSectionParser(HTMLParser):
    """Parse HTML to extract function section IDs."""

    def __init__(self):
        super().__init__()
        self.function_map = {}
        self.current_section_id = None
        self.in_title = False
        self.title_text = []

    def handle_starttag(self, tag, attrs):
        if tag == 'section':
            # Look for id attribute
            for attr, value in attrs:
                if attr == 'id':
                    self.current_section_id = value
        elif tag in ['h1', 'h2', 'h3'] and self.current_section_id:
            self.in_title = True
            self.title_text = []

    def handle_endtag(self, tag):
        if tag in ['h1', 'h2', 'h3'] and self.in_title:
            self.in_title = False
            title = ''.join(self.title_text).strip()

            # Remove the pilcrow (¶) that Sphinx adds
            title = title.rstrip('¶').strip()

            # Check if this looks like a function (ends with ())
            if title.endswith('()') and self.current_section_id:
                # Store the mapping
                self.function_map[title] = self.current_section_id

            self.title_text = []

    def handle_data(self, data):
        if self.in_title:
            self.title_text.append(data)


def scan_html_files(build_dir):
    """Scan all HTML files to build function name -> (file, anchor) mapping."""
    function_map = {}

    for html_file in build_dir.rglob('*.html'):
        # Get relative path from build_dir
        rel_path = html_file.relative_to(build_dir)

        try:
            with open(html_file, 'r', encoding='utf-8') as f:
                html = f.read()

            parser = FunctionSectionParser()
            parser.feed(html)

            # Add mappings from this file
            for func_name, anchor in parser.function_map.items():
                function_map[func_name] = (str(rel_path), anchor)

        except Exception as e:
            print(f"Warning: Error parsing {rel_path}: {e}")
            continue

    return function_map


def process_html_file(filepath, function_map, build_dir):
    """Process a single HTML file to add function links."""
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    # Count changes for reporting
    changes = 0

    # Get current file's relative path for calculating relative URLs
    current_rel_path = filepath.relative_to(build_dir)
    current_dir = current_rel_path.parent

    # Find all <code class="xref py py-func"> elements
    # Pattern: <code class="xref py py-func docutils literal notranslate"><span class="pre">FunctionName()</span></code>
    pattern = r'<code class="xref py py-func docutils literal notranslate"><span class="pre">([^<]+)</span></code>'

    def replace_func(match):
        nonlocal changes
        func_name = match.group(1)

        if func_name in function_map:
            target_file, anchor = function_map[func_name]

            # Calculate relative URL from current file to target
            target_path = Path(target_file)

            if str(current_rel_path) == target_file:
                # Same file - just use anchor
                relative_url = f'#{anchor}'
            else:
                # Different file - calculate relative path
                if current_dir == Path('.'):
                    relative_url = f'{target_file}#{anchor}'
                else:
                    # Calculate relative path
                    relative_url = os.path.relpath(target_file, current_dir)
                    relative_url = f'{relative_url}#{anchor}'

            # Create a link wrapping the code element
            changes += 1
            return f'<a class="reference internal" href="{relative_url}"><code class="xref py py-func docutils literal notranslate"><span class="pre">{func_name}</span></code></a>'
        else:
            # No mapping found, leave as is
            return match.group(0)

    new_content = re.sub(pattern, replace_func, content)

    if changes > 0:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"  {current_rel_path}: {changes} links")

    return changes


def main():
    """Main entry point."""
    script_dir = Path(__file__).parent
    build_dir = script_dir / 'build' / 'html'

    if not build_dir.exists():
        print(f"Error: Build directory not found: {build_dir}")
        return 1

    # Scan all HTML files to build function mapping
    print("Scanning HTML files for function sections...")
    function_map = scan_html_files(build_dir)
    print(f"Found {len(function_map)} functions")

    if len(function_map) == 0:
        print("Warning: No functions found!")
        return 1

    # Show sample mappings
    sample_funcs = list(function_map.items())[:5]
    for func, (file, anchor) in sample_funcs:
        print(f"  {func} -> {file}#{anchor}")

    # Process all HTML files
    print("\nProcessing HTML files...")
    total_changes = 0

    for html_file in build_dir.rglob('*.html'):
        changes = process_html_file(html_file, function_map, build_dir)
        total_changes += changes

    print(f"\nTotal: {total_changes} function references converted to links")
    return 0


if __name__ == '__main__':
    exit(main())
