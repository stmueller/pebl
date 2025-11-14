#!/usr/bin/env python3
"""
Fix inconsistent See Also formatting in design.rst
Convert all references to use :func: role consistently
"""

import re

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
    # ``FunctionName`` → :func:`FunctionName()`
    # But avoid ones that are already part of :func:
    def replace_plain_backtick(match):
        name = match.group(1)
        # Skip lowercase names (like gQuote, properties)
        if name[0].isupper():
            # Add () if not present
            if not name.endswith('()'):
                return f':func:`{name}()`'
            else:
                return f':func:`{name}`'
        else:
            # Keep lowercase as-is (properties, variables)
            return f'``{name}``'

    # Only replace plain backticks not preceded by :func:
    line = re.sub(r'(?<!:func:)``([A-Za-z][A-Za-z0-9_]+\(\))``', replace_plain_backtick, line)

    return line

def main():
    filepath = '/home/smueller/Dropbox/Research/pebl/pebl/doc/sphinx-doc/source/reference/design.rst'

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
                print(f"Line {i+1}:")
                print(f"  Before: {original.rstrip()}")
                print(f"  After:  {fixed.rstrip()}")
            new_lines.append(fixed)
        else:
            new_lines.append(line)

    # Write back
    with open(filepath, 'w') as f:
        f.writelines(new_lines)

    print(f"\nFixed {fixed_count} lines in design.rst")

if __name__ == '__main__':
    main()
