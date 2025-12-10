#!/usr/bin/env python3
"""
Find tests that are missing translations for target languages.

This script scans the battery directory and identifies which tests need
translations added for specific target languages.

Usage:
    python3 find-missing-translations.py [battery_path] [lang1,lang2,...]

Example:
    python3 find-missing-translations.py battery es,fr,de,pt,it,nl
"""

import os
import sys
import json
from pathlib import Path


def find_translation_files(test_dir):
    """Find all translation JSON files in a test directory."""
    translations_dir = os.path.join(test_dir, 'translations')

    if not os.path.isdir(translations_dir):
        return set()

    languages = set()

    # Look for .pbl-XX.json files
    for file in os.listdir(translations_dir):
        if file.endswith('.json') and '.pbl-' in file:
            lang_code = file.split('.pbl-')[1].replace('.json', '')
            languages.add(lang_code)

    return languages


def scan_battery_for_missing(battery_path, target_languages):
    """
    Scan battery and find which tests are missing which target language translations.

    Returns dict: {test_name: [missing_lang1, missing_lang2, ...]}
    """
    if not os.path.isdir(battery_path):
        print(f"Error: Directory not found: {battery_path}")
        return {}

    missing_translations = {}

    test_dirs = [d for d in os.listdir(battery_path)
                 if os.path.isdir(os.path.join(battery_path, d))]

    test_dirs.sort()

    for test_name in test_dirs:
        test_path = os.path.join(battery_path, test_name)

        # Get existing translations
        existing_languages = find_translation_files(test_path)

        # Skip if no English translation exists
        if 'en' not in existing_languages:
            continue

        # Find missing target languages
        missing = []
        for lang in target_languages:
            if lang not in existing_languages:
                missing.append(lang)

        if missing:
            missing_translations[test_name] = missing

    return missing_translations


def main():
    # Default target languages (Latin alphabet only)
    default_targets = ['es', 'fr', 'de', 'pt', 'it', 'nl']

    # Parse arguments
    if len(sys.argv) > 1:
        battery_path = sys.argv[1]
    else:
        battery_path = os.getcwd()

    if len(sys.argv) > 2:
        target_languages = [lang.strip() for lang in sys.argv[2].split(',')]
    else:
        target_languages = default_targets

    battery_path = os.path.abspath(battery_path)

    print(f"Scanning: {battery_path}")
    print(f"Target languages: {', '.join(target_languages)}")
    print("="*80)

    missing = scan_battery_for_missing(battery_path, target_languages)

    if not missing:
        print("\nAll tests have translations for all target languages!")
        return

    # Print summary
    print(f"\nFound {len(missing)} tests with missing translations:\n")

    # Count missing translations per language
    lang_counts = {lang: 0 for lang in target_languages}
    for test, langs in missing.items():
        for lang in langs:
            lang_counts[lang] += 1

    print("Missing translations by language:")
    for lang in target_languages:
        print(f"  {lang}: {lang_counts[lang]} tests")

    # Print detailed list
    print("\n" + "="*80)
    print("DETAILED LIST")
    print("="*80 + "\n")

    for test in sorted(missing.keys()):
        langs = missing[test]
        print(f"{test}: missing {', '.join(langs)}")

    # Export to JSON for automation
    output_file = os.path.join(os.path.dirname(battery_path), 'missing-translations.json')
    with open(output_file, 'w') as f:
        json.dump(missing, f, indent=2)

    print(f"\n{'='*80}")
    print(f"Missing translations exported to: {output_file}")
    print("="*80)


if __name__ == '__main__':
    main()
