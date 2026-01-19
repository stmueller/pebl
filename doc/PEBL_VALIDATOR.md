# PEBL Validator

A command-line tool for validating PEBL (.pbl) script syntax without execution.

## Building

```bash
make validator
```

This creates `bin/pebl-validator` using the same build system as the main PEBL binary.

## Usage

```bash
bin/pebl-validator <file.pbl> [options]
```

### Options

- `--json` - Output results as JSON (for programmatic use)
- `--help` - Show help message

### Exit Codes

- `0` - Valid syntax
- `1` - Invalid syntax or file not found
- `2` - Usage error (missing arguments, etc.)

## Examples

### Text Output (Human-Readable)

```bash
$ bin/pebl-validator battery/corsi/corsi.pbl
PEBL Validator Results
======================
File: battery/corsi/corsi.pbl

✓ Syntax: VALID

Note: Function validation not yet implemented
      (will detect undefined functions in future version)
```

### JSON Output (For Integration)

```bash
$ bin/pebl-validator battery/corsi/corsi.pbl --json
{
  "file": "battery/corsi/corsi.pbl",
  "syntax_valid": true,
  "errors": [
  ],
  "warnings": [
    "Function validation not yet implemented"
  ]
}
```

### Syntax Error Detection

```bash
$ bin/pebl-validator bad-file.pbl
File [bad-file.pbl] opened successfully.
line 7 of bad-file.pbl: syntax error

$ echo $?
1
```

## Current Features (Phase 1)

- ✅ Syntax validation using PEBL's parser (grammar.y)
- ✅ File existence checking
- ✅ JSON and text output modes
- ✅ Proper exit codes for scripting

## Planned Features (Phase 2)

- ⏳ Function validation (detect undefined function calls)
- ⏳ Start(p) function requirement check
- ⏳ AST-based linting:
  - Detect potentially unsafe patterns (SystemCall, excessive UploadFile)
  - Detect common mistakes
  - Variable usage analysis

## Integration with Custom Test Upload

The validator is designed to be used in the custom test upload workflow:

```php
// PHP integration example
exec("bin/pebl-validator $filepath --json 2>&1", $output, $exitCode);
$result = json_decode(implode("\n", $output), true);

if ($exitCode !== 0 || !$result['syntax_valid']) {
    // Reject upload
    return ["error" => "Syntax validation failed", "details" => $result['errors']];
}
```

## Architecture

The validator uses the full PEBL parser and base system but does not execute code:

- **Parser**: Uses existing `grammar.y` and `Pebl.l` (Bison/Flex)
- **Dependencies**: Includes full PEBL core (Evaluator, VariableMap, FunctionMap, etc.) and SDL platform layer
- **Build**: Separate from main PEBL binary, uses `obj-validator/` directory
- **Execution**: Parse-only, no script evaluation

## Notes

- The validator requires SDL libraries to build (due to PEBL's architecture), but does not open windows or execute graphics code
- Syntax errors from the parser are written to stderr and cause immediate exit(1)
- The tool is useful for LLMs and automated testing to validate PEBL syntax without full execution
