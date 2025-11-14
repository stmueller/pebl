#!/usr/bin/env python3
"""
Script to insert PEBLEnvironment function documentation at correct alphabetical positions.
"""

import re

# Define all function documentation blocks
FUNCTIONS = {
    'CallFunction': {
        'after': 'CopyFromClipboard()',
        'before': 'DeleteFile()',
        'content': '''
CallFunction()
--------------

*Calls a PEBL function by name with a list of arguments*

**Description:**

Calls a PEBL function dynamically using its name as a string and a list of arguments. This is useful for implementing callbacks, event handlers, or calling functions whose names are determined at runtime.

**Usage:**

.. code-block:: pebl

   CallFunction(<function_name>, <argument_list>)

**Example:**

.. code-block:: pebl

   ## Call a function by name
   result <- CallFunction("Max", [1, 5, 3, 2])
   Print(result)  # == 5

   ## Use for callbacks
   myCallback <- "ProcessResponse"
   CallFunction(myCallback, [response, rt])

**See Also:**

:func:`PropertyExists()`, :func:`MakeCustomObject()`

'''
    },

    'GetExecutableName': {
        'after': 'GetDrivers()',
        'before': 'GetJoystickAxisState()',
        'content': '''
GetExecutableName()
-------------------

*Returns the name/path of the PEBL executable*

**Description:**

This function signals a fatal error directing users to use the global variable ``gExecutableName`` instead. The executable name is set at program startup and stored in this global variable.

**Usage:**

.. code-block:: pebl

   name <- gExecutableName  ##Use this global variable instead

**See Also:**

:func:`GetSystemType()`, :func:`GetWorkingDirectory()`

'''
    },

    'GetHomeDirectory': {
        'after': 'GetExecutableName()',
        'before': 'GetJoystickAxisState()',
        'content': '''
GetHomeDirectory()
------------------

*Returns the user's home directory path*

**Description:**

Returns the path to the current user's home directory. This is platform-specific and will return different values on Windows, Linux, and Mac OS.

**Usage:**

.. code-block:: pebl

   GetHomeDirectory()

**Example:**

.. code-block:: pebl

   homedir <- GetHomeDirectory()
   Print("User home directory: " + homedir)

**See Also:**

:func:`GetWorkingDirectory()`, :func:`SetWorkingDirectory()`, :func:`GetDirectoryListing()`

'''
    },

    'GetTimeOfDay': {
        'after': 'GetTime()',
        'before': 'GetVideoModes()',
        'content': '''
GetTimeOfDay()
--------------

*Returns the current time in seconds since Unix epoch*

**Description:**

Returns the current time of day in seconds since the Unix epoch (January 1, 1970). This provides an absolute timestamp useful for logging when events occurred in real-world time.

**Usage:**

.. code-block:: pebl

   GetTimeOfDay()

**Example:**

.. code-block:: pebl

   timestamp <- GetTimeOfDay()
   Print("Current Unix timestamp: " + timestamp)

**See Also:**

:func:`GetTime()`, :func:`TimeStamp()`

'''
    },

    'GetWorkingDirectory': {
        'after': 'GetVideoModes()',
        'before': 'IsAnyKeyDown()',
        'content': '''
GetWorkingDirectory()
---------------------

*Returns the current working directory*

**Description:**

Returns the current working directory path. This is the directory from which PEBL is currently executing and where relative file paths are resolved.

**Usage:**

.. code-block:: pebl

   GetWorkingDirectory()

**Example:**

.. code-block:: pebl

   cwd <- GetWorkingDirectory()
   Print("Current directory: " + cwd)

**See Also:**

:func:`SetWorkingDirectory()`, :func:`GetHomeDirectory()`, :func:`GetDirectoryListing()`

'''
    },

    'SetWorkingDirectory': {
        'after': 'SetMouseCursorPosition()',
        'before': 'ShowCursor()',
        'content': '''
SetWorkingDirectory()
---------------------

*Changes the current working directory*

**Description:**

Changes the current working directory to the specified path. This affects how relative file paths are resolved in subsequent file operations.

**Usage:**

.. code-block:: pebl

   SetWorkingDirectory(<path>)

**Example:**

.. code-block:: pebl

   SetWorkingDirectory("./data")
   Print(GetWorkingDirectory())  ##Shows new directory

   ##Now relative paths work from ./data
   file <- FileOpenRead("output.csv")

**See Also:**

:func:`GetWorkingDirectory()`, :func:`GetHomeDirectory()`, :func:`FileExists()`

'''
    },

    'SystemCallUpdate': {
        'after': 'SystemCall()',
        'before': 'TimeStamp()',
        'content': '''
SystemCallUpdate()
------------------

*Executes an OS command with real-time output updates*

**Description:**

Calls an operating system command similar to SystemCall(), but with support for receiving output updates during execution. This is useful for long-running commands where you want to see progress.

**Usage:**

.. code-block:: pebl

   SystemCallUpdate(<command>)
   SystemCallUpdate(<command>, <arguments>)

**Example:**

.. code-block:: pebl

   ##Run a command with arguments
   result <- SystemCallUpdate("ls", "-la")

**See Also:**

:func:`SystemCall()`, :func:`GetSystemType()`

'''
    },

    'TranslateString': {
        'after': 'TranslateKeyCode()',
        'before': 'VariableExists()',
        'content': '''
TranslateString()
-----------------

*Converts a key name string to its keycode*

**Description:**

Translates a string representation of a key (like "a", "space", "return") into its corresponding internal keycode value. This is useful for programmatically working with keyboard input.

**Usage:**

.. code-block:: pebl

   TranslateString(<key_string>)

**Example:**

.. code-block:: pebl

   keycode <- TranslateString("a")
   spaceCode <- TranslateString("space")
   enterCode <- TranslateString("return")

**See Also:**

:func:`TranslateKeyCode()`, :func:`WaitForKeyPress()`

'''
    },

    'WaitForKeyDown': {
        'after': 'WaitForAllKeysUp()',
        'before': 'WaitForAnyKeyDown()',
        'content': '''
WaitForKeyDown()
----------------

*Waits until a specific key is in the down state*

**Description:**

Waits for a specific key to be detected in the down position. Unlike WaitForKeyPress(), this tests the state of the key rather than waiting for a keypress event. Will return immediately if the key is already down when called.

**Usage:**

.. code-block:: pebl

   WaitForKeyDown(<key>)

**Example:**

.. code-block:: pebl

   WaitForKeyDown("a")
   Print("The 'a' key is now down")

**See Also:**

:func:`WaitForKeyPress()`, :func:`WaitForKeyRelease()`, :func:`WaitForAnyKeyDown()`

'''
    },

    'WaitForKeyRelease': {
        'after': 'WaitForKeyUp()',
        'before': 'WaitForListKeyPress()',
        'content': '''
WaitForKeyRelease()
-------------------

*Waits until a specific key is released*

**Description:**

Waits for a specific key to be released (transition from down to up state). This is useful for ensuring a key has been released before continuing, preventing accidental repeated inputs.

**Usage:**

.. code-block:: pebl

   WaitForKeyRelease(<key>)

**Example:**

.. code-block:: pebl

   WaitForKeyPress("space")
   Print("Space pressed")
   WaitForKeyRelease("space")
   Print("Space released")

**See Also:**

:func:`WaitForKeyDown()`, :func:`WaitForKeyPress()`, :func:`WaitForAnyKeyPress()`

'''
    }
}

def find_section_end(lines, start_idx):
    """Find the end of a function documentation section."""
    # Look for the next function heading (line ending with ())
    i = start_idx + 1
    while i < len(lines):
        if re.match(r'^[A-Z][a-zA-Z0-9_]+\(\)\s*$', lines[i]):
            return i
        # Also check for section dividers
        if lines[i].strip().startswith('===='):
            return i
        i += 1
    return i

def insert_functions(filepath):
    """Insert all function documentation at correct positions."""

    with open(filepath, 'r') as f:
        lines = f.readlines()

    # Track insertions (in reverse order so indices don't shift)
    insertions = []

    for func_name, func_data in FUNCTIONS.items():
        after_pattern = func_data['after']
        before_pattern = func_data['before']
        content = func_data['content']

        # Find the insertion point
        after_idx = None
        before_idx = None

        for i, line in enumerate(lines):
            if line.strip() == after_pattern:
                after_idx = i
            if line.strip() == before_pattern:
                before_idx = i

        if after_idx is None:
            print(f"Warning: Could not find '{after_pattern}' for {func_name}")
            continue
        if before_idx is None:
            print(f"Warning: Could not find '{before_pattern}' for {func_name}")
            continue

        # Find end of 'after' section
        insert_at = find_section_end(lines, after_idx)

        if insert_at >= before_idx:
            print(f"Warning: Insertion point for {func_name} is after target section")
            continue

        insertions.append((insert_at, content, func_name))
        print(f"Will insert {func_name} at line {insert_at}")

    # Sort by line number in reverse so we can insert without shifting indices
    insertions.sort(reverse=True, key=lambda x: x[0])

    # Insert all content
    for insert_at, content, func_name in insertions:
        lines.insert(insert_at, content)
        print(f"Inserted {func_name}")

    # Write back
    with open(filepath, 'w') as f:
        f.writelines(lines)

    print(f"\nSuccessfully inserted {len(insertions)} functions")

if __name__ == '__main__':
    filepath = '/home/smueller/Dropbox/Research/pebl/pebl/doc/sphinx-doc/source/reference/peblenvironment.rst'
    insert_functions(filepath)
