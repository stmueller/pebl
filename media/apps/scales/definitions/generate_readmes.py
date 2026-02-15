#!/usr/bin/env python3
"""Generate README.md files for each scale definition directory.

Scans each subdirectory for a <code>.json definition file and creates
a README.md summarizing the scale for community/git repository use.
"""

import json
import os
import sys
import glob
import subprocess
import shutil


def generate_readme(scale_dir):
    """Generate a README.md for a single scale directory."""
    dirname = os.path.basename(scale_dir)
    json_path = os.path.join(scale_dir, f"{dirname}.json")

    if not os.path.exists(json_path):
        print(f"  Skipping {dirname}: no {dirname}.json found")
        return False

    with open(json_path, "r") as f:
        data = json.load(f)

    info = data.get("scale_info", {})
    name = info.get("name", dirname)
    code = info.get("code", dirname)
    abbrev = info.get("abbreviation", "")
    description = info.get("description", "")
    citation = info.get("citation", "")
    license_info = info.get("license", "")
    version = info.get("version", "")
    url = info.get("url", "")

    # Count questions (exclude instruction items)
    questions = data.get("questions", [])
    likert_count = sum(1 for q in questions if q.get("type") == "likert")
    other_count = sum(1 for q in questions if q.get("type") not in ("likert", "inst"))
    total_items = likert_count + other_count

    # Get Likert scale info
    likert_opts = data.get("likert_options", {})
    likert_points = likert_opts.get("points", 0)

    # Get dimensions
    dimensions = data.get("dimensions", [])

    # Get scoring info
    scoring = data.get("scoring", {})

    # Find available translations
    trans_files = glob.glob(os.path.join(scale_dir, f"{code}.pbl-*.json"))
    languages = []
    for tf in sorted(trans_files):
        basename = os.path.basename(tf)
        # Extract language code from <code>.pbl-<lang>.json
        lang = basename.replace(f"{code}.pbl-", "").replace(".json", "")
        languages.append(lang)

    # Build README
    lines = []
    lines.append(f"# {name}")
    lines.append("")

    if abbrev:
        lines.append(f"**Abbreviation:** {abbrev}  ")
    lines.append(f"**Code:** `{code}`  ")
    if version:
        lines.append(f"**Version:** {version}  ")
    if license_info:
        lines.append(f"**License:** {license_info}  ")
    lines.append("")

    if description:
        lines.append(f"{description}")
        lines.append("")

    # Items summary
    lines.append("## Scale Summary")
    lines.append("")
    lines.append(f"- **Items:** {total_items}")
    if likert_count > 0 and likert_points > 0:
        lines.append(f"- **Likert items:** {likert_count} ({likert_points}-point scale)")
    if other_count > 0:
        lines.append(f"- **Other items:** {other_count}")

    if dimensions:
        lines.append(f"- **Dimensions:** {len(dimensions)}")
        for dim in dimensions:
            dim_name = dim.get("name", dim.get("id", ""))
            dim_abbrev = dim.get("abbreviation", "")
            dim_desc = dim.get("description", "")
            label = dim_name
            if dim_abbrev and dim_abbrev != dim_name:
                label += f" ({dim_abbrev})"
            if dim_desc:
                lines.append(f"  - **{label}:** {dim_desc}")
            else:
                lines.append(f"  - **{label}**")

    if languages:
        lines.append(f"- **Languages:** {', '.join(languages)}")
    lines.append("")

    # Scoring
    if scoring:
        lines.append("## Scoring")
        lines.append("")
        for score_id, score_info in scoring.items():
            method = score_info.get("method", "")
            items = score_info.get("items", [])
            desc = score_info.get("description", "")
            coding = score_info.get("item_coding", {})
            reverse_coded = [k for k, v in coding.items() if v == -1]

            lines.append(f"- **{score_id}**: {method} ({len(items)} items)")
            if desc:
                lines.append(f"  - {desc}")
            if reverse_coded:
                lines.append(f"  - Reverse-coded: {', '.join(reverse_coded)}")
        lines.append("")

    # Citation
    if citation:
        lines.append("## Citation")
        lines.append("")
        # Split multiple citations (separated by newlines)
        for cit in citation.strip().split("\n"):
            cit = cit.strip()
            if cit:
                lines.append(f"> {cit}")
                lines.append(">")
        # Remove trailing >
        if lines[-1] == ">":
            lines.pop()
        lines.append("")

    if url:
        lines.append(f"## Links")
        lines.append("")
        lines.append(f"- [{url}]({url})")
        lines.append("")

    # Files section
    lines.append("## Files")
    lines.append("")
    lines.append(f"- `{code}.json` - Scale definition")
    for lang in languages:
        lines.append(f"- `{code}.pbl-{lang}.json` - {lang.upper()} translation")
    lines.append("")

    # Usage
    lines.append("## Usage")
    lines.append("")
    lines.append("This scale is designed to be run using the PEBL ScaleRunner system.")
    lines.append("See the [PEBL documentation](https://pebl.sf.net) for details.")
    lines.append("")

    readme_path = os.path.join(scale_dir, "README.md")
    with open(readme_path, "w") as f:
        f.write("\n".join(lines))

    print(f"  Created README.md for {name} ({code})")
    return True


def generate_screenshot(scale_dir):
    """Generate a screenshot for a single scale directory."""
    dirname = os.path.basename(scale_dir)
    screenshot_dest = os.path.join(scale_dir, f"{dirname}.pbl.png")

    base = os.path.dirname(os.path.abspath(__file__))       # definitions/
    scales_dir = os.path.normpath(os.path.join(base, "..")) # media/apps/scales/
    pebl_bin = os.path.normpath(os.path.join(scales_dir, "..", "..", "..", "bin", "pebl2"))
    script = os.path.normpath(os.path.join(scales_dir, "..", "scale-screenshot.pbl"))

    if not os.path.exists(pebl_bin) or not os.path.exists(script):
        print(f"  Skipping screenshot for {dirname}: pebl2 or script not found")
        return False

    # Run from definitions/ dir so script path 1 (<code>/<code>.json) resolves correctly
    try:
        result = subprocess.run([pebl_bin, script, "-v", dirname],
                               cwd=base, capture_output=True, timeout=60)
    except subprocess.TimeoutExpired:
        print(f"  Screenshot timed out for {dirname}")
        return False

    # Output lands in definitions/<code>.pbl.png, move into definitions/<code>/
    output_file = os.path.join(base, f"{dirname}.pbl.png")
    if os.path.exists(output_file):
        shutil.move(output_file, screenshot_dest)
        print(f"  Generated screenshot for {dirname}")
        return True

    print(f"  Screenshot failed for {dirname} (exit {result.returncode})")
    return False


def main():
    # Default to the directory containing this script
    base_dir = os.path.dirname(os.path.abspath(__file__))
    if len(sys.argv) > 1:
        base_dir = sys.argv[1]

    print(f"Scanning {base_dir} for scale definitions...")
    readme_count = 0
    screenshot_count = 0

    for entry in sorted(os.listdir(base_dir)):
        entry_path = os.path.join(base_dir, entry)
        if os.path.isdir(entry_path):
            if generate_readme(entry_path):
                readme_count += 1
            if generate_screenshot(entry_path):
                screenshot_count += 1

    print(f"\nGenerated {readme_count} README.md files and {screenshot_count} screenshots.")


if __name__ == "__main__":
    main()
