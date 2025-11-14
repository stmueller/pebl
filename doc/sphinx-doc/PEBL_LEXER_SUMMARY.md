# PEBL Syntax Highlighter for Sphinx

## Implementation

Created a custom Pygments lexer based on the official PEBL lexer and grammar:
- **Source**: `src/base/Pebl.l` (flex) and `src/base/grammar.y` (bison)
- **Location**: `source/_pygments/pebl_lexer.py`

## Syntax Rules Implemented

### Keywords (Case-Insensitive)
- Control flow: `if`, `elseif`, `else`, `loop`, `while`, `break`, `return`
- Function definition: `define`
- Logical operators: `and`, `or`, `not`

### Identifiers

#### Function Names
- Pattern: `:?[A-Z][a-zA-Z0-9_]*`
- Must start with uppercase letter
- Optional `:` prefix
- Example: `MakeWindow()`, `GetSubNum()`, `:CustomFunction()`

#### Global Variables
- Pattern: `g[A-Za-z0-9_]*(?:\.[A-Za-z0-9_]+)?`
- Must start with lowercase `g`
- Can have dot notation for properties
- Example: `gWin`, `gSubNum`, `gFileOut.name`

#### Local Variables
- Pattern: `[a-fh-z][A-Za-z0-9_]*(?:\.[A-Za-z0-9_]+)?`
- Must start with lowercase letter (except `g`)
- Can have dot notation for properties
- Example: `numTrials`, `accuracy`, `myList.length`

### Literals

#### Numbers
- **Float**: `[0-9]*\.[0-9]+` (e.g., `3.14`, `.5`, `100.0`)
- **Integer**: `[0-9]+` (e.g., `10`, `255`, `0`)

#### Strings
- Pattern: `"[^"]*"`
- Double quotes only (no single quotes in PEBL)
- Example: `"Hello World"`, `"Subject response"`

### Operators

#### Assignment
- `<-` (arrow)

#### Comparison
- `==` (equal)
- `!=`, `<>`, `~=` (not equal - three variants)
- `<`, `>`, `<=`, `>=` (ordering)

#### Arithmetic
- `+`, `-`, `*`, `/`, `^` (power)

#### Sequence/Range
- `:` (colon, used in `Sequence(1:10,1)` or for default parameters)

### Delimiters
- Parentheses: `(`, `)`
- Braces: `{`, `}`
- Brackets: `[`, `]`
- Comma: `,`
- Semicolon: `;` (optional statement terminator)
- Dot: `.` (property access)

### Comments
- `#` to end of line
- Example: `# This is a comment`

## Usage in Documentation

All PEBL code blocks in the Sphinx documentation automatically use this syntax highlighter:

```rst
.. code-block:: pebl

   define Start(p)
   {
       gWin <- MakeWindow()
       numTrials <- 10

       if numTrials > 0
       {
           Print("Starting experiment")
       }
   }
```

## Color Scheme

The highlighter uses standard Pygments token types:
- **Keywords**: Blue (default Sphinx style)
- **Function Names**: Teal/cyan
- **Global Variables**: Bold/special highlighting
- **Local Variables**: Standard variable color
- **Strings**: Green/red (depends on theme)
- **Numbers**: Purple/magenta
- **Comments**: Gray/italic
- **Operators**: Standard operator color

## Testing

View syntax highlighting examples at:
- Main documentation: `build/html/index.html`
- Test page: `build/html/syntax_test.html`
- PEBLMath reference: `build/html/reference/compiled/pebl_math.html`

## Differences from R

While PEBL is similar to R, key differences captured by this lexer:
1. Assignment uses `<-` exclusively (no `=` assignment)
2. Function names MUST start with uppercase
3. Variable names MUST start with lowercase
4. Global variables MUST start with `g`
5. Keywords are case-insensitive
6. No ternary operator
7. No `\n` for newlines (use `CR(1)` function)
8. Strings use double quotes only
