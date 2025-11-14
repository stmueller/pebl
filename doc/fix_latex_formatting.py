#!/usr/bin/env python3
r"""
Script to fix LaTeX formatting in PEBL RST documentation files.

This script:
1. Removes \item[Usage] and \item[Description] LaTeX commands
2. Converts \emph{text} to *text* (RST italic)
3. Converts \url{url} to proper RST hyperlinks
"""

import re
import sys
from pathlib import Path

def fix_latex_formatting(filepath):
    """Fix all LaTeX formatting issues in a file."""

    with open(filepath, 'r') as f:
        content = f.read()

    original_content = content
    changes = 0

    # Fix \item[Usage] - just remove it entirely
    # It appears in Description sections where it shouldn't be
    pattern = r'\\item\[Usage\][\s\t]*'
    matches = re.findall(pattern, content)
    if matches:
        content = re.sub(pattern, '', content)
        changes += len(matches)
        print(f"  Removed {len(matches)} \\item[Usage] commands")

    # Fix \item[Description] - just remove it
    pattern = r'\\item\[Description\][\s\t]*'
    matches = re.findall(pattern, content)
    if matches:
        content = re.sub(pattern, '', content)
        changes += len(matches)
        print(f"  Removed {len(matches)} \\item[Description] commands")

    # Fix \emph{text} -> *text*
    pattern = r'\\emph\{([^}]+)\}'
    matches = re.findall(pattern, content)
    if matches:
        content = re.sub(pattern, r'*\1*', content)
        changes += len(matches)
        print(f"  Converted {len(matches)} \\emph commands to *italic*")

    # Fix \url{url} -> `url <url>`_
    pattern = r'\\url\{([^}]+)\}'
    matches = re.findall(pattern, content)
    if matches:
        for url in matches:
            content = content.replace(f'\\url{{{url}}}', f'`{url} <{url}>`_')
        changes += len(matches)
        print(f"  Converted {len(matches)} \\url commands to RST links")

    # Fix \texttt{text} -> ``text``
    pattern = r'\\texttt\{([^}]+)\}'
    matches = re.findall(pattern, content)
    if matches:
        content = re.sub(pattern, r'``\1``', content)
        changes += len(matches)
        print(f"  Converted {len(matches)} \\texttt commands to ``code``")

    if changes > 0:
        with open(filepath, 'w') as f:
            f.write(content)

    return changes


def main():
    """Process all compiled reference files."""

    base_dir = Path('/home/smueller/Dropbox/Research/pebl/pebl/doc/sphinx-doc/source/reference')

    files_to_process = [
        'peblenvironment.rst',
        'peblstream.rst',
        'peblobjects.rst',
        'pebllist.rst',
        'peblstring.rst',
    ]

    total_changes = 0

    print("Fixing LaTeX formatting in PEBL documentation files...")
    print("=" * 80)

    for filename in files_to_process:
        filepath = base_dir / filename
        if not filepath.exists():
            print(f"Skipping {filename} (not found)")
            continue

        print(f"\nProcessing {filename}...")
        changes = fix_latex_formatting(filepath)
        total_changes += changes

        if changes == 0:
            print(f"  No changes needed")

    print("\n" + "=" * 80)
    print(f"Total formatting fixes: {total_changes}")
    print("\nDone! Please rebuild documentation with 'make html'")


if __name__ == '__main__':
    main()
