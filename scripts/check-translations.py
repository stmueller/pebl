#!/usr/bin/env python3
"""
PEBL Translation Completeness Checker

Scans all tests in upload-battery (or battery/) and checks translation files
against the English reference to identify missing keys.

Usage:
    # Check entire battery
    python3 check-translations.py [battery_path]

    # Check single test (by path)
    python3 check-translations.py /path/to/battery/testname

    # Check single test (run from test directory)
    cd battery/testname
    python3 ../../scripts/check-translations.py .

If no path is provided, defaults to ../upload-battery
"""

import os
import json
import sys
from collections import defaultdict
from pathlib import Path


def find_translation_files(test_dir):
    """Find all translation JSON files in a test directory."""
    translations_dir = os.path.join(test_dir, 'translations')

    if not os.path.isdir(translations_dir):
        return {}

    translations = {}

    # Look for .pbl-XX.json files
    for file in os.listdir(translations_dir):
        if file.endswith('.json') and '.pbl-' in file:
            lang_code = file.split('.pbl-')[1].replace('.json', '')
            filepath = os.path.join(translations_dir, file)
            translations[lang_code] = filepath

    return translations


def load_json_safe(filepath):
    """Load JSON file with multiple encoding attempts."""
    encodings = ['utf-8', 'iso-8859-1', 'windows-1252', 'cp1252']

    for encoding in encodings:
        try:
            with open(filepath, 'r', encoding=encoding) as f:
                return json.load(f)
        except (UnicodeDecodeError, json.JSONDecodeError):
            continue

    return None


def get_all_keys(data, parent_key='', sep='.'):
    """Recursively get all keys from nested JSON structure."""
    keys = set()

    if isinstance(data, dict):
        for k, v in data.items():
            new_key = f"{parent_key}{sep}{k}" if parent_key else k
            keys.add(new_key)
            if isinstance(v, dict):
                keys.update(get_all_keys(v, new_key, sep))

    return keys


def compare_translations(english_data, other_data):
    """
    Compare translation files and return missing/extra keys.
    """
    english_keys = get_all_keys(english_data)
    other_keys = get_all_keys(other_data)

    missing = english_keys - other_keys
    extra = other_keys - english_keys

    return missing, extra


def check_test_translations(test_path, test_name):
    """Check all translations for a single test."""
    translations = find_translation_files(test_path)

    if not translations:
        return None

    if 'en' not in translations:
        return {
            'test': test_name,
            'status': 'error',
            'message': 'No English translation file found',
            'languages': list(translations.keys())
        }

    # Load English as reference
    english_data = load_json_safe(translations['en'])
    if not english_data:
        return {
            'test': test_name,
            'status': 'error',
            'message': 'Failed to load English translation',
            'languages': list(translations.keys())
        }

    english_key_count = len(get_all_keys(english_data))

    results = {
        'test': test_name,
        'status': 'ok',
        'english_keys': english_key_count,
        'languages': {}
    }

    # Check each non-English translation
    for lang, filepath in translations.items():
        if lang == 'en':
            continue

        lang_data = load_json_safe(filepath)
        if not lang_data:
            results['languages'][lang] = {
                'status': 'error',
                'message': 'Failed to load translation file'
            }
            continue

        missing, extra = compare_translations(english_data, lang_data)

        lang_key_count = len(get_all_keys(lang_data))
        completeness = (lang_key_count / english_key_count * 100) if english_key_count > 0 else 0

        results['languages'][lang] = {
            'keys': lang_key_count,
            'completeness': round(completeness, 1),
            'missing': sorted(list(missing)),
            'extra': sorted(list(extra))
        }

        if missing or extra:
            results['status'] = 'incomplete'

    return results


def is_test_directory(path):
    """Check if a directory appears to be a test directory (has translations/)."""
    translations_dir = os.path.join(path, 'translations')
    return os.path.isdir(translations_dir)


def scan_battery(battery_path):
    """Scan all tests in the battery directory or a single test."""
    if not os.path.isdir(battery_path):
        print(f"Error: Directory not found: {battery_path}")
        return [], [], []

    all_results = []
    en_only_tests = []
    no_translations_tests = []

    # Check if this is a single test directory (has translations/)
    if is_test_directory(battery_path):
        # This is a single test directory
        test_name = os.path.basename(battery_path)
        result = check_test_translations(battery_path, test_name)
        if result:
            all_results.append(result)
            # Check if it's English-only
            if result['status'] != 'error' and len(result.get('languages', {})) == 0:
                en_only_tests.append(test_name)
        return all_results, en_only_tests, no_translations_tests

    # This is a battery directory - scan all subdirectories
    test_dirs = [d for d in os.listdir(battery_path)
                 if os.path.isdir(os.path.join(battery_path, d))]

    test_dirs.sort()

    for test_name in test_dirs:
        test_path = os.path.join(battery_path, test_name)

        # Check if it has a translations directory
        if not is_test_directory(test_path):
            no_translations_tests.append(test_name)
            continue

        result = check_test_translations(test_path, test_name)

        if result:
            all_results.append(result)
            # Check if it's English-only
            if result['status'] != 'error' and len(result.get('languages', {})) == 0:
                en_only_tests.append(test_name)

    return all_results, en_only_tests, no_translations_tests


def print_summary_report(results):
    """Print a summary report of translation status."""
    print("\n" + "="*80)
    print("PEBL TRANSLATION COMPLETENESS REPORT")
    print("="*80)

    tests_with_translations = len(results)
    tests_complete = len([r for r in results if r['status'] == 'ok'])
    tests_incomplete = len([r for r in results if r['status'] == 'incomplete'])
    tests_errors = len([r for r in results if r['status'] == 'error'])

    print(f"\nTotal tests with translations: {tests_with_translations}")
    print(f"  Complete: {tests_complete}")
    print(f"  Incomplete: {tests_incomplete}")
    print(f"  Errors: {tests_errors}")

    # Language statistics
    lang_stats = defaultdict(lambda: {'total': 0, 'complete': 0, 'incomplete': 0})

    for result in results:
        if result['status'] == 'error':
            continue

        for lang, lang_data in result.get('languages', {}).items():
            lang_stats[lang]['total'] += 1

            if lang_data.get('status') == 'error':
                continue

            if not lang_data.get('missing') and not lang_data.get('extra'):
                lang_stats[lang]['complete'] += 1
            else:
                lang_stats[lang]['incomplete'] += 1

    if lang_stats:
        print("\n" + "-"*80)
        print("LANGUAGE COVERAGE")
        print("-"*80)
        print(f"{'Language':<15} {'Tests':<10} {'Complete':<12} {'Incomplete':<12}")
        print("-"*80)

        for lang in sorted(lang_stats.keys()):
            stats = lang_stats[lang]
            print(f"{lang:<15} {stats['total']:<10} {stats['complete']:<12} {stats['incomplete']:<12}")


def print_detailed_report(results, show_complete=False, show_missing_keys=True):
    """Print detailed report for each test."""
    print("\n" + "="*80)
    print("DETAILED TEST REPORTS")
    print("="*80)

    for result in results:
        test_name = result['test']
        status = result['status']

        # Skip complete tests if show_complete is False
        if status == 'ok' and not show_complete:
            continue

        print(f"\n{'='*80}")
        print(f"Test: {test_name}")
        print(f"Status: {status.upper()}")

        if status == 'error':
            print(f"Error: {result.get('message', 'Unknown error')}")
            print(f"Available languages: {', '.join(result.get('languages', []))}")
            continue

        print(f"English keys: {result['english_keys']}")

        if not result.get('languages'):
            print("No non-English translations found")
            continue

        print(f"\nTranslations: {len(result['languages'])} languages")
        print("-"*80)

        for lang in sorted(result['languages'].keys()):
            lang_data = result['languages'][lang]

            if lang_data.get('status') == 'error':
                print(f"  [{lang}] ERROR: {lang_data.get('message')}")
                continue

            completeness = lang_data['completeness']
            status_icon = "✓" if completeness == 100.0 else "⚠"

            print(f"  [{lang}] {status_icon} {completeness}% complete ({lang_data['keys']} keys)")

            if show_missing_keys:
                if lang_data['missing']:
                    print(f"       Missing keys ({len(lang_data['missing'])}): {', '.join(lang_data['missing'][:10])}")
                    if len(lang_data['missing']) > 10:
                        print(f"       ... and {len(lang_data['missing']) - 10} more")

                if lang_data['extra']:
                    print(f"       Extra keys ({len(lang_data['extra'])}): {', '.join(lang_data['extra'][:10])}")
                    if len(lang_data['extra']) > 10:
                        print(f"       ... and {len(lang_data['extra']) - 10} more")


def print_translation_coverage_report(en_only_tests, no_translations_tests):
    """Print report of tests with limited translation coverage."""
    if not en_only_tests and not no_translations_tests:
        return

    print("\n" + "="*80)
    print("TRANSLATION COVERAGE SUMMARY")
    print("="*80)

    if en_only_tests:
        print(f"\nTests with ONLY English translations ({len(en_only_tests)}):")
        print("-"*80)
        for test in en_only_tests:
            print(f"  - {test}")

    if no_translations_tests:
        print(f"\nTests with NO translation files ({len(no_translations_tests)}):")
        print("-"*80)
        for test in no_translations_tests:
            print(f"  - {test}")


def export_json_report(results, output_file):
    """Export results to JSON file."""
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(results, f, indent=2, ensure_ascii=False)
    print(f"\nJSON report exported to: {output_file}")


def main():
    # Determine battery path
    if len(sys.argv) > 1:
        battery_path = sys.argv[1]
    else:
        # Default to current directory
        battery_path = os.getcwd()

    battery_path = os.path.abspath(battery_path)

    # Determine if scanning single test or battery
    is_single_test = is_test_directory(battery_path)

    if is_single_test:
        test_name = os.path.basename(battery_path)
        print(f"Checking translations for test: {test_name}")
        print(f"Path: {battery_path}\n")
    else:
        print(f"Scanning battery directory: {battery_path}")
        print("This may take a moment...\n")

    results, en_only_tests, no_translations_tests = scan_battery(battery_path)

    if not results and not en_only_tests and not no_translations_tests:
        print("No tests found.")
        return

    # Print reports
    if results:
        print_summary_report(results)
        print_detailed_report(results, show_complete=is_single_test, show_missing_keys=True)

    # Print coverage report (only for battery scans, not single tests)
    if not is_single_test:
        print_translation_coverage_report(en_only_tests, no_translations_tests)

    # Export JSON report
    if results:
        if is_single_test:
            output_file = os.path.join(battery_path, 'translation-report.json')
        else:
            output_file = os.path.join(os.path.dirname(battery_path), 'translation-report.json')

        export_json_report(results, output_file)

    print("\n" + "="*80)
    print("Report complete!")
    print("="*80)


if __name__ == '__main__':
    main()
