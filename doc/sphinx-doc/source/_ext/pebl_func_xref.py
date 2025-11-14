"""
Custom Sphinx extension to handle PEBL function cross-references.

This extension resolves :func: roles to create proper hyperlinks to function
documentation sections, even when those sections aren't defined using Python
domain directives.
"""

from docutils import nodes
from sphinx.util import logging

logger = logging.getLogger(__name__)


def collect_function_targets(app, doctree):
    """
    Collect all function section headers and store their locations.

    This runs for each document during the read phase.
    """
    env = app.env
    docname = env.docname

    if not hasattr(env, 'pebl_function_targets'):
        env.pebl_function_targets = {}

    print(f"[pebl_func_xref] Collecting from {docname}")

    # Find all sections with titles that look like function names
    count = 0
    section_count = 0
    for section in doctree.traverse(nodes.section):
        section_count += 1

        # Debug first few sections
        if section_count <= 3:
            title_nodes = section.traverse(nodes.title, descend=False)
            if title_nodes:
                print(f"  Section: ids={section['ids'][:1] if section['ids'] else 'NONE'}, title={title_nodes[0].astext()[:40]}")

        if not section['ids']:
            continue

        # Get the section title
        title_nodes = section.traverse(nodes.title, descend=False)
        if not title_nodes:
            continue

        title_text = title_nodes[0].astext()

        # Check if this looks like a function (ends with ())
        if title_text.endswith('()'):
            func_name = title_text
            anchor = section['ids'][0]

            # Store the mapping: function name -> (document, anchor)
            env.pebl_function_targets[func_name] = (docname, anchor)
            count += 1

    print(f"[pebl_func_xref] Found {count}/{section_count} functions in {docname}")


def resolve_func_xrefs(app, doctree, fromdocname):
    """
    Post-process the doctree to convert unresolved function xrefs to links.

    This runs after Sphinx has tried to resolve all cross-references.
    We look for literal nodes that were created from failed :func: references
    and convert them to proper hyperlinks.
    """
    env = app.env

    if not hasattr(env, 'pebl_function_targets'):
        return

    print(f"[pebl_func_xref] Resolving xrefs in {fromdocname}")

    # Show how many functions we have in our map
    func_count = len(env.pebl_function_targets) if hasattr(env, 'pebl_function_targets') else 0
    print(f"  Have {func_count} functions in target map")

    if func_count > 0 and func_count < 10:
        print(f"  Sample functions: {list(env.pebl_function_targets.keys())[:5]}")

    # Look for xref nodes with py:func that became literals
    link_count = 0
    literal_count = 0
    xref_count = 0

    for node in doctree.traverse(nodes.literal):
        literal_count += 1
        classes = node.get('classes', [])

        # Debug: print first few literal nodes
        if literal_count <= 5:
            print(f"  Literal #{literal_count}: classes={classes}, text={node.astext()[:30]}")

        # Check if this is from a cross-reference
        if 'xref' not in classes:
            continue
        xref_count += 1

        if 'py-func' not in classes:
            continue

        # Get the function name from the text content
        func_text = node.astext()
        print(f"  Found py-func xref: {func_text}")

        # Try to find this function in our map
        target = func_text
        if target not in env.pebl_function_targets:
            # Try without parentheses if they're missing
            if not target.endswith('()'):
                target = target + '()'
            if target not in env.pebl_function_targets:
                continue

        target_doc, anchor = env.pebl_function_targets[target]

        # Create a proper reference node
        refnode = nodes.reference('', '')

        # Set the reference URI
        if target_doc == fromdocname:
            # Same document - just use anchor
            refuri = f'#{anchor}'
        else:
            # Different document - need relative path
            relative_uri = app.builder.get_relative_uri(fromdocname, target_doc)
            refuri = f'{relative_uri}#{anchor}'

        refnode['refuri'] = refuri
        refnode['internal'] = True

        # Create a literal node for the display text
        literal_node = nodes.literal('', func_text, classes=['xref', 'py', 'py-func'])

        refnode.append(literal_node)

        # Replace the old literal node with our reference node
        node.replace_self(refnode)
        link_count += 1

    if link_count > 0:
        print(f"[pebl_func_xref] Created {link_count} links in {fromdocname}")


def merge_function_targets(app, env, docnames, other):
    """Merge function target maps when doing parallel builds."""
    if not hasattr(env, 'pebl_function_targets'):
        env.pebl_function_targets = {}
    if hasattr(other, 'pebl_function_targets'):
        env.pebl_function_targets.update(other.pebl_function_targets)


def setup(app):
    """Setup the Sphinx extension."""
    print("[pebl_func_xref] Extension loading...")

    # Connect to events
    app.connect('doctree-read', collect_function_targets)
    app.connect('doctree-resolved', resolve_func_xrefs)
    app.connect('env-merge-info', merge_function_targets)

    print("[pebl_func_xref] Extension loaded successfully")

    return {
        'version': '0.1',
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }
