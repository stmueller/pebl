# Copyright Year Update Tool

This directory contains a Python script to automatically update copyright year ranges in PEBL source code files.

## Overview

The script `update_copyright_years.py` updates all copyright statements attributed to Shane T. Mueller in the `src/` directory to extend to a specified year (or the current year by default).

## Usage

### Basic Usage (Updates to Current Year)
```bash
python3 update_copyright_years.py
```

### Specify Target Year
```bash
python3 update_copyright_years.py 2026
```

## What It Does

The script:
- Scans all `.cpp`, `.h`, `.c`, and `.hpp` files in the `src/` directory
- Finds copyright statements matching the pattern: `Copyright: (c) YYYY-YYYY Shane T. Mueller`
- Updates the end year to the target year (preserving the start year)
- Skips backup files (`*.bak`, `*~`, `#*#`)
- Skips generated parser files (grammar.tab.*, lex.yy.*)
- Only updates copyrights attributed to Shane T. Mueller (not third-party code)

## Examples

### Before:
```cpp
//    Copyright:  (c) 2003-2020 Shane T. Mueller <smueller@obereed.net>
```

### After (running in 2025):
```cpp
//    Copyright:  (c) 2003-2025 Shane T. Mueller <smueller@obereed.net>
```

## Output

The script prints:
- Each file that was updated
- A summary showing total files updated and skipped

Example output:
```
Updating copyright years to 2025
============================================================
Updated: /path/to/src/apps/PEBL.cpp
Updated: /path/to/src/libs/PEBLStream.cpp
...
============================================================

Summary:
  Files updated: 108
  Files skipped: 15

108 files were successfully updated to copyright year 2025
```

## When to Run

Run this script:
- At the beginning of each year
- Before major releases
- After significant development work in a new year

## Requirements

- Python 3.6 or later
- No external dependencies (uses only Python standard library)

## Safety

The script:
- Only modifies files in the `src/` directory
- Preserves the original start year of copyright ranges
- Only updates Shane T. Mueller's copyrights
- Does not modify third-party code or generated files
- Creates no backup files (use version control to revert if needed)

## Notes

- Always commit your changes to version control before running this script
- Review the changes with `git diff` after running
- The script is idempotent - running it multiple times with the same year won't cause issues

## Troubleshooting

If the script doesn't find files to update:
- Ensure you're running it from the PEBL root directory
- Check that the `src/` directory exists
- Verify that files haven't already been updated to the target year

If the script reports encoding errors:
- The script uses UTF-8 encoding with error ignoring
- These can usually be safely ignored for copyright updates

## License

This script is part of the PEBL project and is licensed under GPL 2.
