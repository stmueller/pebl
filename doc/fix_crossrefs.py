#!/usr/bin/env python3
"""
Script to fix function indexing and cross-references in PEBL documentation.

This script:
1. Adds .. index:: directives for each function
2. Standardizes See Also sections to use :func: role consistently
3. Ensures all function references are properly formatted
"""

import re
import sys
from pathlib import Path

def add_index_entries(filepath):
    """Add index directives before each function heading."""

    with open(filepath, 'r') as f:
        lines = f.readlines()

    modified = []
    i = 0
    changes_made = 0

    while i < len(lines):
        line = lines[i]

        # Match function headings: FunctionName() followed by dashes
        if i + 1 < len(lines):
            next_line = lines[i + 1]
            func_match = re.match(r'^([A-Z][a-zA-Z0-9_]*)\(\)\s*$', line)
            dash_match = re.match(r'^-+\s*$', next_line)

            if func_match and dash_match:
                func_name = func_match.group(1)

                # Check if index directive already exists
                has_index = False
                if i > 0 and '.. index::' in lines[i-1]:
                    has_index = True
                if i > 1 and '.. index::' in lines[i-2]:
                    has_index = True

                if not has_index:
                    # Add blank line, index directive, and blank line before function
                    modified.append('\n')
                    modified.append(f'.. index:: {func_name}\n')
                    modified.append('\n')
                    changes_made += 1

                modified.append(line)
                i += 1
                continue

        modified.append(line)
        i += 1

    with open(filepath, 'w') as f:
        f.writelines(modified)

    return changes_made


def standardize_see_also(filepath):
    """Standardize See Also sections to use :func: role consistently."""

    with open(filepath, 'r') as f:
        content = f.read()

    # Pattern to match See Also sections
    # We need to find plain function names and wrap them in :func:`...`

    lines = content.split('\n')
    modified = []
    in_see_also = False
    changes_made = 0

    for i, line in enumerate(lines):
        if line.strip() == '**See Also:**':
            in_see_also = True
            modified.append(line)
            continue

        # Exit See Also section when we hit a new section or blank lines followed by content
        if in_see_also:
            if line.strip().startswith('**') or line.strip().startswith('..') or \
               (line.strip() == '' and i + 1 < len(lines) and lines[i + 1].strip() and \
                not lines[i + 1].strip().startswith(':')):
                in_see_also = False
            elif line.strip():  # Non-empty line in See Also
                # Replace plain function names with :func: role
                # Skip if line already contains only :func: references
                if ':func:`' not in line or not re.match(r'^[\s:func:`()A-Za-z0-9_,\s]+$', line):
                    new_line = line

                    # Find all plain function references (not already in :func:)
                    # Simple approach: find FunctionName() and check if not preceded by :func:`
                    pattern = r'([A-Z][a-zA-Z0-9_]*\(\))'
                    matches = list(re.finditer(pattern, line))

                    if matches:
                        # Replace from end to beginning to maintain positions
                        for match in reversed(matches):
                            func_name = match.group(1)
                            start, end = match.span(1)

                            # Check if already wrapped
                            prefix_start = max(0, start - 7)
                            prefix = line[prefix_start:start]
                            suffix = line[end:end+1]

                            # Skip if already in :func:`...`
                            if ':func:`' in prefix or suffix == '`':
                                continue

                            new_line = new_line[:start] + f':func:`{func_name}`' + new_line[end:]
                            changes_made += 1
                else:
                    new_line = line

                modified.append(new_line)
                continue

        modified.append(line)

    with open(filepath, 'w') as f:
        f.write('\n'.join(modified))

    return changes_made


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

    total_index_changes = 0
    total_ref_changes = 0

    print("Processing PEBL documentation files...")
    print("=" * 80)

    for filename in files_to_process:
        filepath = base_dir / filename
        if not filepath.exists():
            print(f"Skipping {filename} (not found)")
            continue

        print(f"\nProcessing {filename}...")

        # Add index entries
        index_changes = add_index_entries(filepath)
        print(f"  Added {index_changes} index entries")
        total_index_changes += index_changes

        # Standardize See Also sections
        ref_changes = standardize_see_also(filepath)
        print(f"  Standardized {ref_changes} function references")
        total_ref_changes += ref_changes

    print("\n" + "=" * 80)
    print(f"Total index entries added: {total_index_changes}")
    print(f"Total function references standardized: {total_ref_changes}")
    print("\nDone! Please rebuild documentation with 'make html'")


if __name__ == '__main__':
    main()
