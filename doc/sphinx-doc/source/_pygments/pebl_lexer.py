"""
Pygments lexer for PEBL (Psychology Experiment Building Language)

Based on the official PEBL lexer (Pebl.l) and grammar (grammar.y)
"""

from pygments.lexer import RegexLexer, include, words, bygroups
from pygments.token import (
    Text, Comment, Operator, Keyword, Name, String,
    Number, Punctuation, Whitespace
)

__all__ = ['PEBLLexer']


class PEBLLexer(RegexLexer):
    """
    Lexer for PEBL (Psychology Experiment Building Language)

    Based on src/base/Pebl.l and src/base/grammar.y from PEBL source
    """

    name = 'PEBL'
    aliases = ['pebl']
    filenames = ['*.pbl']
    mimetypes = ['text/x-pebl']

    tokens = {
        'root': [
            # BOM (byte order mark) - ignored
            (r'\xef\xbb\xbf', Text),

            # Whitespace
            (r'[ \t]+', Whitespace),

            # Comments (# to end of line)
            (r'#.*$', Comment.Single),

            # Keywords (case-insensitive in PEBL)
            (r'(?i)\b(if|elseif|else|loop|while|return|define|and|or|not|break)\b', Keyword),

            # Function definitions: define FunctionName(params)
            (r'(?i)(define)(\s+)([A-Z][a-zA-Z0-9_]*)',
             bygroups(Keyword, Whitespace, Name.Function)),

            # Function calls: FunctionName(...) or :FunctionName(...)
            # Function names must start with uppercase letter, optional : prefix
            (r':?[A-Z][a-zA-Z0-9_]*(?=\s*\()', Name.Function),

            # Numbers - float must come before integer
            # Float: [0-9]*\.[0-9]+
            (r'[0-9]*\.[0-9]+', Number.Float),
            # Integer: [0-9]+
            (r'[0-9]+', Number.Integer),

            # Strings (double quotes only in PEBL)
            # Pattern: \"[^\"]*[\"\n]
            (r'"[^"]*"', String.Double),

            # Assignment operator (must come before < comparison)
            (r'<-', Operator),

            # Comparison operators
            (r'==', Operator),
            (r'!=', Operator),
            (r'<>', Operator),  # Alternative NE
            (r'~=', Operator),  # Alternative NE
            (r'>=', Operator),
            (r'<=', Operator),
            (r'>', Operator),
            (r'<', Operator),

            # Arithmetic operators
            (r'\+', Operator),
            (r'-', Operator),
            (r'\*', Operator),
            (r'/', Operator),
            (r'\^', Operator),  # Power operator

            # Colon (for default parameters and sequence ranges)
            (r':', Operator),

            # Dot (for property access)
            (r'\.', Punctuation),

            # Brackets and parentheses
            (r'\(', Punctuation),
            (r'\)', Punctuation),
            (r'\{', Punctuation),
            (r'\}', Punctuation),
            (r'\[', Punctuation),
            (r'\]', Punctuation),

            # Comma and semicolon
            (r',', Punctuation),
            (r';', Punctuation),

            # Global variables: g[A-Za-z0-9_]* (can have dots for properties)
            # Pattern: g[A-Za-z0-9_]*(\.[A-Za-z0-9_]+)?
            (r'g[A-Za-z0-9_]*(?:\.[A-Za-z0-9_]+)?', Name.Variable.Global),

            # Local variables: [a-fh-z][A-Za-z0-9_]* (NOT starting with g, can have dots)
            # Pattern: [a-fh-z][A-Za-z0-9_]*(\.[A-Za-z0-9_]+)?
            (r'[a-fh-z][A-Za-z0-9_]*(?:\.[A-Za-z0-9_]+)?', Name.Variable),

            # Newlines (significant in PEBL grammar)
            (r'\r?\n', Text),
            (r'\r', Text),

            # Any other character
            (r'.', Text),
        ],
    }


def setup(app):
    """
    Setup function for Sphinx integration.
    This is called automatically when the extension is loaded.
    """
    from sphinx.highlighting import lexers
    lexers['pebl'] = PEBLLexer()
