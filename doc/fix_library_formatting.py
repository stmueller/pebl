#!/usr/bin/env python3
"""
Script to add index directives and fix See Also formatting in library RST files.
"""

import re
from pathlib import Path

def fix_library_file(filepath):
    """Fix index directives and See Also sections in a library file."""
    with open(filepath, 'r') as f:
        content = f.read()

    lines = content.split('\n')
    new_lines = []
    i = 0

    while i < len(lines):
        line = lines[i]

        # Check if this is a function header (line ending with () followed by dashes)
        if i < len(lines) - 1:
            next_line = lines[i + 1] if i + 1 < len(lines) else ""

            # Function header pattern: FunctionName() followed by a line of dashes
            if line.strip().endswith('()') and next_line.strip() and all(c == '-' for c in next_line.strip()):
                func_name = line.strip()[:-2]  # Remove ()

                # Check if previous lines already have an index directive
                has_index = False
                if len(new_lines) >= 3:
                    if '.. index::' in new_lines[-1] or '.. index::' in new_lines[-2]:
                        has_index = True

                if not has_index:
                    # Add blank lines and index directive before function
                    if new_lines and new_lines[-1].strip():
                        new_lines.append('')
                    new_lines.append('')
                    new_lines.append(f'.. index:: {func_name}')
                    new_lines.append('')

        # Fix See Also sections - convert plain function names to :func: role
        if line.strip() == '**See Also:**':
            new_lines.append(line)
            i += 1
            # Process the next few lines (the function list)
            if i < len(lines):
                # Skip blank line if present
                if not lines[i].strip():
                    new_lines.append(lines[i])
                    i += 1

                # Process the See Also content line
                if i < len(lines):
                    see_also_line = lines[i]
                    # Convert function names to :func: role
                    # Match patterns like "FunctionName()" with or without comma/space
                    fixed_line = re.sub(
                        r'(\b[A-Z][a-zA-Z0-9_]*\(\))',
                        r':func:`\1`',
                        see_also_line
                    )
                    new_lines.append(fixed_line)
                    i += 1
                    continue

        new_lines.append(line)
        i += 1

    # Write back
    with open(filepath, 'w') as f:
        f.write('\n'.join(new_lines))

    print(f"Fixed {filepath.name}")

def main():
    library_dir = Path('/home/smueller/Dropbox/Research/pebl/pebl/doc/sphinx-doc/source/reference')

    for rst_file in library_dir.glob('*.rst'):
        print(f"Processing {rst_file.name}...")
        fix_library_file(rst_file)

    print("Done!")

if __name__ == '__main__':
    main()
