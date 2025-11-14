#!/usr/bin/env python3
"""
Fix inconsistent See Also formatting in all library .rst files
Convert all references to use :func: role consistently
"""

import re
import os

def fix_see_also_line(line):
    """Fix a See Also content line to use :func: consistently."""
    original = line

    # Pattern 1: Fix malformed :func: with extra backticks
    # ``:func:`Name()``` → :func:`Name()`
    line = re.sub(r'``(:func:`[^`]+`)``', r'\1', line)

    # Pattern 2: Fix :func: with extra parentheses
    # :func:`Name()`() → :func:`Name()`
    line = re.sub(r':func:`([^`]+\(\))`\(\)', r':func:`\1`', line)

    # Pattern 3: Convert plain backticks with function names (capital letter start)
    # ``FunctionName()`` → :func:`FunctionName()`
    # But avoid ones that are already part of :func:
    def replace_plain_backtick(match):
        name = match.group(1)
        # Skip lowercase names (like gQuote, properties)
        if name[0].isupper():
            # Already has parentheses
            return f':func:`{name}`'
        else:
            # Keep lowercase as-is (properties, variables)
            return f'``{name}``'

    # Only replace plain backticks not preceded by :func:
    line = re.sub(r'(?<!:func:)``([A-Za-z][A-Za-z0-9_]+\(\))``', replace_plain_backtick, line)

    return line

def fix_file(filepath):
    """Fix See Also sections in a single file."""
    with open(filepath, 'r') as f:
        lines = f.readlines()

    new_lines = []
    in_see_also = False
    see_also_started = False
    fixed_count = 0

    for i, line in enumerate(lines):
        if '**See Also:**' in line:
            in_see_also = True
            see_also_started = False  # Haven't seen content yet
            new_lines.append(line)
            continue

        if in_see_also:
            # First blank line after **See Also:** is OK
            if line.strip() == '':
                if not see_also_started:
                    # This is the expected blank line after the header
                    new_lines.append(line)
                    continue
                else:
                    # Content has started, blank line ends the section
                    in_see_also = False
                    new_lines.append(line)
                    continue

            # Check if we hit a new section marker (index directive, etc.)
            if line.strip().startswith('..'):
                in_see_also = False
                new_lines.append(line)
                continue

            # We're in See Also content now
            see_also_started = True

            # Fix this See Also line
            original = line
            fixed = fix_see_also_line(line)
            if original != fixed:
                fixed_count += 1
            new_lines.append(fixed)
        else:
            new_lines.append(line)

    # Write back
    with open(filepath, 'w') as f:
        f.writelines(new_lines)

    return fixed_count

def main():
    base_path = '/home/smueller/Dropbox/Research/pebl/pebl/doc/sphinx-doc/source/reference'

    # Files to fix (design already done, but include it to be safe)
    files = ['graphics.rst', 'math.rst', 'ui.rst']

    total_fixed = 0
    for filename in files:
        filepath = os.path.join(base_path, filename)
        if os.path.exists(filepath):
            count = fix_file(filepath)
            print(f"{filename}: Fixed {count} lines")
            total_fixed += count
        else:
            print(f"{filename}: File not found")

    print(f"\nTotal: Fixed {total_fixed} lines across all files")

if __name__ == '__main__':
    main()
