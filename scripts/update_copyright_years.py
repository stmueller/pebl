#!/usr/bin/env python3
"""
Update Copyright Years in PEBL Source Files

This script updates copyright year ranges in PEBL source code files to extend
to the current year. It only updates copyrights attributed to Shane T. Mueller.

Usage:
    python3 update_copyright_years.py [target_year]

    If target_year is not provided, it defaults to the current year.

Example:
    python3 update_copyright_years.py 2026

Author: PEBL Development Team
License: GPL 2
"""

import os
import re
import sys
from datetime import datetime
from pathlib import Path


def update_copyright_in_file(filepath, target_year):
    """
    Update copyright year in a single file.

    Args:
        filepath: Path to the file to update
        target_year: Target year to update copyright to

    Returns:
        True if file was modified, False otherwise
    """
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return False

    # Pattern to match copyright lines with date ranges for Shane T. Mueller
    # Matches formats like: Copyright: (c) 2003-2024 Shane T. Mueller
    pattern = r'(//\s*Copyright:\s*\(c\)\s*)(\d{4})-(20[0-2][0-9])(\s+Shane\s+T\.\s+Mueller)'

    # Check if file contains a copyright statement that needs updating
    if not re.search(pattern, content):
        return False

    # Replace the end year with target year
    new_content = re.sub(
        pattern,
        r'\g<1>\g<2>-' + str(target_year) + r'\g<4>',
        content
    )

    # Only write if content changed
    if new_content != content:
        try:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(new_content)
            return True
        except Exception as e:
            print(f"Error writing {filepath}: {e}")
            return False

    return False


def should_skip_file(filepath):
    """
    Determine if a file should be skipped.

    Args:
        filepath: Path to check

    Returns:
        True if file should be skipped, False otherwise
    """
    filename = os.path.basename(filepath)

    # Skip backup files
    if filename.endswith('.bak') or filename.endswith('~') or \
       filename.startswith('#') or filename.endswith('#') or \
       filename.endswith('.edited'):
        return True

    # Skip generated parser files (these have FSF copyrights)
    if 'grammar.tab' in filename or 'lex.yy' in filename:
        return True

    # Skip old/backup versions
    if 'grammar-old' in filename or '-old.' in filename:
        return True

    return False


def update_copyright_years(src_dir, target_year):
    """
    Update copyright years in all eligible files in src directory.

    Args:
        src_dir: Path to the src directory
        target_year: Target year to update copyrights to

    Returns:
        Tuple of (files_updated, files_skipped)
    """
    files_updated = []
    files_skipped = []

    # Walk through src directory
    for root, dirs, files in os.walk(src_dir):
        # Skip third-party library directories if they exist
        dirs[:] = [d for d in dirs if d not in ['emsdk', 'external', 'third_party']]

        for filename in files:
            # Only process C++ source and header files
            if not (filename.endswith('.cpp') or filename.endswith('.h') or
                   filename.endswith('.c') or filename.endswith('.hpp')):
                continue

            filepath = os.path.join(root, filename)

            if should_skip_file(filepath):
                files_skipped.append(filepath)
                continue

            if update_copyright_in_file(filepath, target_year):
                files_updated.append(filepath)
                print(f"Updated: {filepath}")

    return files_updated, files_skipped


def main():
    """Main function."""
    # Get target year from command line or use current year
    if len(sys.argv) > 1:
        try:
            target_year = int(sys.argv[1])
            if target_year < 2003 or target_year > 2100:
                print(f"Error: Year {target_year} seems invalid")
                sys.exit(1)
        except ValueError:
            print(f"Error: Invalid year '{sys.argv[1]}'")
            sys.exit(1)
    else:
        target_year = datetime.now().year

    print(f"Updating copyright years to {target_year}")
    print("=" * 60)

    # Find the src directory relative to this script
    script_dir = Path(__file__).parent
    src_dir = script_dir / 'src'

    if not src_dir.exists():
        print(f"Error: src directory not found at {src_dir}")
        sys.exit(1)

    # Update copyrights
    files_updated, files_skipped = update_copyright_years(str(src_dir), target_year)

    # Print summary
    print("=" * 60)
    print(f"\nSummary:")
    print(f"  Files updated: {len(files_updated)}")
    print(f"  Files skipped: {len(files_skipped)}")

    if files_updated:
        print(f"\n{len(files_updated)} files were successfully updated to copyright year {target_year}")
    else:
        print(f"\nNo files needed updating (all copyrights may already be at {target_year})")

    return 0


if __name__ == '__main__':
    sys.exit(main())
