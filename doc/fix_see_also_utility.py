#!/usr/bin/env python3
"""
Fix inconsistent See Also formatting in utility.rst
Convert all references to use :func: role consistently
"""

import re

def fix_see_also_line(line):
    """Fix a See Also content line to use :func: consistently."""

    # Pattern 1: Convert ``FunctionName`` to :func:`FunctionName()`
    # But avoid already converted ones
    if ':func:' not in line:
        # Add () if not present, wrap in :func:
        line = re.sub(r'``([A-Z][a-zA-Z0-9_]+)``', r':func:`\1()`', line)

    # Pattern 2: Convert plain backticks to :func:
    # Match function names in backticks without :func:
    line = re.sub(r'(?<!:func:)``([A-Z][a-zA-Z0-9_]+)(\(\))?``',
                  lambda m: f':func:`{m.group(1)}()`', line)

    # Pattern 3: Fix malformed :func: references (extra backticks)
    line = re.sub(r'``(:func:`[^`]+`)``', r'\1', line)

    return line

def main():
    filepath = '/home/smueller/Dropbox/Research/pebl/pebl/doc/sphinx-doc/source/reference/utility.rst'

    with open(filepath, 'r') as f:
        lines = f.readlines()

    new_lines = []
    in_see_also = False
    fixed_count = 0

    for i, line in enumerate(lines):
        if '**See Also:**' in line:
            in_see_also = True
            new_lines.append(line)
            continue

        if in_see_also:
            # Check if we're still in See Also section (not a new section header)
            if line.strip() and not line.startswith(' ') and not line.strip().startswith('``'):
                # End of See Also section
                in_see_also = False
                new_lines.append(line)
                continue

            # If blank line or next section marker, end See Also
            if line.strip() == '' or line.strip().startswith('..'):
                in_see_also = False
                new_lines.append(line)
                continue

            # Fix this See Also line
            original = line
            fixed = fix_see_also_line(line)
            if original != fixed:
                fixed_count += 1
                print(f"Line {i+1}:")
                print(f"  Before: {original.rstrip()}")
                print(f"  After:  {fixed.rstrip()}")
            new_lines.append(fixed)
        else:
            new_lines.append(line)

    # Write back
    with open(filepath, 'w') as f:
        f.writelines(new_lines)

    print(f"\nFixed {fixed_count} lines")

if __name__ == '__main__':
    main()
