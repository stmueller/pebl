#!/usr/bin/env python3
"""
Extract function names from PEBL documentation files.
"""

import re
import sys

def extract_from_reference(filename):
    """Extract functions from reference.tex"""
    functions = []
    with open(filename, 'r', encoding='latin-1') as f:
        content = f.read()

    # Find all Name/Symbol entries
    pattern = r'\\item\[Name/Symbol\]\s+(.*?)(?=\\item\[Description\]|$)'
    matches = re.findall(pattern, content, re.DOTALL)

    for match in matches:
        # Clean up the match
        match = match.strip()
        # Remove \verb and its delimiters
        match = re.sub(r'\\verb[!+|](.+?)[!+|]', r'\1', match)
        # Remove \texttt
        match = re.sub(r'\\texttt\{([^}]+)\}', r'\1', match)
        # Remove other LaTeX commands
        match = re.sub(r'\\[a-zA-Z]+\{([^}]*)\}', r'\1', match)
        match = re.sub(r'\\[a-zA-Z]+', '', match)
        match = match.strip()

        # Skip operators and empty lines
        if match and not re.match(r'^[+\-*/^;#<>=!&|,()[\]{}]+$', match):
            # Extract function name (before any parentheses or special chars)
            func_name = re.split(r'[\s(,]', match)[0]
            if func_name and len(func_name) > 1:
                functions.append(func_name)

    return sorted(set(functions))

def extract_from_chap5(filename):
    """Extract functions mentioned in chap5.tex"""
    functions = []
    with open(filename, 'r', encoding='latin-1') as f:
        content = f.read()

    # Look for \texttt{FunctionName}
    pattern = r'\\texttt\{([A-Z][a-zA-Z0-9_]+)(?:\([^)]*\))?\}'
    matches = re.findall(pattern, content)
    functions.extend(matches)

    # Look for function definitions or examples
    pattern = r'\b([A-Z][a-zA-Z0-9_]+)\s*\('
    matches = re.findall(pattern, content)
    functions.extend(matches)

    return sorted(set(functions))

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: extract_functions.py <reference.tex|chap5.tex>")
        sys.exit(1)

    filename = sys.argv[1]

    if 'reference' in filename:
        functions = extract_from_reference(filename)
    elif 'chap5' in filename:
        functions = extract_from_chap5(filename)
    else:
        print(f"Unknown file type: {filename}")
        sys.exit(1)

    for func in functions:
        print(func)
