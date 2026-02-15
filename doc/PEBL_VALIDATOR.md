# PEBL Validator

A command-line tool for validating PEBL (.pbl) script syntax and function calls without executing the script. The validator checks that your code parses correctly and that all function calls reference defined functions, without opening any windows or running any experiment logic.

## Where to Find It

The validator is included with PEBL 2.3 and later:

- **Windows installer:** `C:\Program Files (x86)\PEBL\bin\pebl-validator.exe`
- **Windows portable:** `pebl-validator.exe` in the `bin\` folder of the extracted zip
- **Linux AppImage:** Bundled inside the AppImage (run via `./PEBL-2.3-x86_64.AppImage pebl-validator`)
- **Linux installed:** `/usr/local/bin/pebl-validator` (after `make install`)
- **Built from source:** `bin/pebl-validator` (after `make validator`)

## Usage

### Windows

Open **Command Prompt** or **PowerShell** and run:

```
"C:\Program Files (x86)\PEBL\bin\pebl-validator.exe" myscript.pbl
```

Or if using the portable version, navigate to the PEBL folder:

```
cd C:\Users\YourName\Desktop\PEBL2.3_Portable
bin\pebl-validator.exe myscript.pbl
```

**Tip:** You can drag and drop a .pbl file onto the Command Prompt window to paste its full path, then add the validator command before it.

To validate a battery test:

```
"C:\Program Files (x86)\PEBL\bin\pebl-validator.exe" "C:\Users\YourName\Documents\pebl-exp.2.3\battery\stroop\stroop.pbl"
```

### Linux

```bash
# If using the AppImage directly:
./PEBL-2.3-x86_64.AppImage pebl-validator battery/stroop/stroop.pbl

# If pebl-validator is on your PATH (after make install or symlink):
pebl-validator battery/stroop/stroop.pbl
```

### Options

| Option   | Description                              |
|----------|------------------------------------------|
| `--json` | Output results as JSON (for scripts/tools) |
| `--help` | Show help message                        |

### Exit Codes

| Code | Meaning                              |
|------|--------------------------------------|
| `0`  | Valid syntax, all functions defined   |
| `1`  | Syntax error or undefined functions   |
| `2`  | Usage error (missing arguments, etc.) |

## Examples

### Validating a Correct Script

```
> pebl-validator battery/corsi/corsi.pbl
PEBL Validator Results
======================
File: battery/corsi/corsi.pbl

Syntax: VALID
```

### Detecting a Syntax Error

```
> pebl-validator broken-script.pbl
line 7 of broken-script.pbl: syntax error

PEBL Validator Results
======================
File: broken-script.pbl

Syntax: INVALID

Errors:
  Parse failed - syntax error in file
```

### Detecting Undefined Functions

If your script calls a function that doesn't exist in PEBL's built-in functions or standard library:

```
> pebl-validator myscript.pbl
PEBL Validator Results
======================
File: myscript.pbl

Syntax: INVALID

Errors:
  Undefined function: MyMadeUpFunction
```

### JSON Output

Use `--json` for machine-readable output, useful for editors or CI pipelines:

```
> pebl-validator myscript.pbl --json
{
  "file": "myscript.pbl",
  "syntax_valid": true,
  "errors": [
  ],
  "warnings": [
  ]
}
```

## What It Checks

1. **Syntax validation** - Parses your script using the same parser as PEBL itself (grammar.y/Pebl.l). Any syntax error that would prevent PEBL from running will be caught.

2. **Standard library loading** - Loads all PEBL standard libraries (Design.pbl, Utility.pbl, Math.pbl, Graphics.pbl, UI.pbl, HTML.pbl, Layout.pbl, Transfer.pbl) so their functions are known.

3. **Function validation** - Checks that every function call in your script refers to a function that is either:
   - A built-in C++ function
   - Defined in the PEBL standard library
   - Defined in your own script

4. **Start() function check** - Verifies that your script defines a `Start(p)` function with exactly one parameter, which is required for PEBL scripts.

## What It Does NOT Check

- **Runtime errors** - Type mismatches, out-of-bounds list access, division by zero, etc. are not caught because the script is not executed.
- **File references** - Missing image, sound, or data files are not detected.
- **Variable usage** - Undefined variables or typos in variable names are not caught (PEBL creates variables on first assignment).

## Batch Validation

You can validate all scripts in a directory using a shell loop:

**Windows (PowerShell):**
```powershell
Get-ChildItem -Path "C:\Users\YourName\Documents\pebl-exp.2.3\battery" -Recurse -Filter "*.pbl" |
  ForEach-Object { & "C:\Program Files (x86)\PEBL\bin\pebl-validator.exe" $_.FullName }
```

**Linux/macOS:**
```bash
find battery/ -name "*.pbl" -exec pebl-validator {} \;
```

## Building from Source

```bash
make validator
```

This creates `bin/pebl-validator`. The validator uses a stub platform layer (no SDL or graphics dependencies) so it can be built on systems without SDL installed.
