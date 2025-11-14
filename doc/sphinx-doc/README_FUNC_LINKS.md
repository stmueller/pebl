# Function Cross-Reference Linking

## Overview

This Sphinx documentation uses `:func:` roles to reference PEBL functions throughout the documentation. By default, Sphinx renders these as styled code blocks but not as clickable hyperlinks (because PEBL functions aren't defined using Python domain directives).

We solve this with a **post-processing script** that automatically converts all `:func:` references into proper hyperlinks after Sphinx builds the HTML.

## How It Works

### 1. Write Documentation with `:func:` Roles

In the RST source files, reference functions using the `:func:` role:

```rst
**See Also:**

:func:`AddObject()`, :func:`RemoveObject()`, :func:`Draw()`
```

### 2. Sphinx Build

When you run `make html`, Sphinx:
- Processes the RST files
- Renders `:func:` roles as `<code class="xref py py-func">` elements (styled but not linked)
- Creates section anchors for each function (e.g., `id="addobject"`)

### 3. Post-Processing Script

The `make_func_links.py` script automatically runs after Sphinx and:

1. **Scans all HTML files** to build a function map:
   - Finds all `<section>` tags with function titles (ending in `()`)
   - Maps function names to their file and anchor (e.g., `AddObject()` → `peblobjects.html#addobject`)

2. **Updates all HTML files**:
   - Finds all `<code class="xref py py-func">` elements
   - Wraps them in `<a>` tags with correct relative URLs
   - Handles same-page and cross-page references

### Example Transformation

**Before:**
```html
<code class="xref py py-func docutils literal notranslate">
  <span class="pre">RemoveObject()</span>
</code>
```

**After:**
```html
<a class="reference internal" href="#removeobject">
  <code class="xref py py-func docutils literal notranslate">
    <span class="pre">RemoveObject()</span>
  </code>
</a>
```

## Building Documentation

### Standard Build

```bash
make html
```

This automatically:
1. Runs Sphinx to build HTML
2. Runs `make_func_links.py` to create function links
3. Reports how many links were created

### Expected Output

```
Building HTML documentation...
build succeeded, 17 warnings.

Converting function references to hyperlinks...
Scanning HTML files for function sections...
Found 415 functions
  ...

Processing HTML files...
  reference/compiled/peblobjects.html: 135 links
  reference/compiled/peblstream.html: 151 links
  ...

Total: 1377 function references converted to links

Build finished. The HTML pages are in build/html
```

## Files

- **`make_func_links.py`** - Post-processing script that creates function links
- **`Makefile`** - Updated to run the script after Sphinx builds HTML
- **`source/conf.py`** - Sphinx configuration (uses standard extensions only)

## Technical Details

### Function Section Detection

The script identifies functions by:
- Finding `<section>` elements with `id` attributes
- Looking for heading text that ends with `()`
- Removing Sphinx's pilcrow symbol (¶) from titles

### Relative URL Calculation

- **Same-page reference**: `href="#functionname"`
- **Cross-page reference**: `href="../path/to/file.html#functionname"`

### CSS Styling

The generated `<a>` tags use the class `reference internal`, which Sphinx's CSS already styles appropriately.

## Statistics

Based on the current documentation:

- **415 functions** documented across all files
- **1,377 function references** converted to clickable links
- **13 documentation files** processed

Per-file breakdown:
- peblenvironment.html: 337 links
- peblmath.html: 185 links
- pebl_math.html: 187 links
- peblstream.html: 151 links
- peblobjects.html: 135 links
- pebllist.html: 85 links
- design.html: 75 links
- utility.html: 72 links
- math.html: 56 links
- graphics.html: 30 links
- html.html: 32 links
- ui.html: 18 links
- peblstring.html: 14 links

## Maintenance

### Adding New Functions

When adding new function documentation:

1. Add the function section with standard RST format:
   ```rst
   .. index:: NewFunction

   NewFunction()
   -------------

   *Brief description*

   **Description:**
   ...

   **See Also:**

   :func:`RelatedFunction1()`, :func:`RelatedFunction2()`
   ```

2. The script will automatically:
   - Find the new function when scanning HTML
   - Create links to/from the new function

No manual updates to `make_func_links.py` are needed.

### Troubleshooting

If function links aren't working:

1. **Check the function title**: Must end with `()` and be in an `<h1>`, `<h2>`, or `<h3>` tag
2. **Check the section has an ID**: Sphinx should automatically create lowercase IDs from function names
3. **Run with verbose output**: Check the script's "Found N functions" count
4. **Inspect the HTML**: Look for the `<section id="functionname">` tag

### Script Limitations

- Only processes functions with `()` in their titles
- Relies on Sphinx's standard HTML structure
- Doesn't link to functions in external documentation (use `:doc:` or `:ref:` for that)
