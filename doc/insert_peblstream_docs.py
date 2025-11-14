#!/usr/bin/env python3
"""
Script to insert PEBLStream function documentation at correct alphabetical positions.
"""

import re

# Define all function documentation blocks
# Some will replace stubs in "Functions Pending Documentation" section
# Others will be inserted in alphabetical order in main section
FUNCTIONS = {
    'AcceptNetworkConnection': {
        'insert_type': 'alphabetical',  # Insert in main section
        'after': 'AppendFile()',
        'before': 'CheckForNetworkConnection()',
        'content': '''
AcceptNetworkConnection()
-------------------------

*Accepts an incoming network connection on a listening port*

**Description:**

Accepts an incoming TCP/IP connection on a network listener that was opened using ``OpenNetworkListener()``. Returns a network connection object that can be used to send and receive data. This is typically used after ``CheckForNetworkConnection()`` confirms a connection is available.

**Usage:**

.. code-block:: pebl

   AcceptNetworkConnection(<listener>, <port>)

**Example:**

.. code-block:: pebl

   listener <- OpenNetworkListener(4444)
   if(CheckForNetworkConnection(listener))
   {
      connection <- AcceptNetworkConnection(listener, 4444)
      SendData(connection, "Hello client!")
      CloseNetworkConnection(connection)
   }

**See Also:**

:func:`OpenNetworkListener()`, :func:`CheckForNetworkConnection()`, :func:`WaitForNetworkConnection()`, :func:`CloseNetworkConnection()`

'''
    },

    'COMPortGetByte': {
        'insert_type': 'replace_stub',  # Replace existing stub
        'stub_title': 'COMPortGetByte()',
        'content': '''
COMPortGetByte()
----------------

*Gets a byte from the COM port*

**Description:**

Reads a single byte from an open COM/serial port. Returns the byte value as an integer (0-255). If no data is available, it returns -1. This is used for serial communication with external devices.

**Usage:**

.. code-block:: pebl

   COMPortGetByte(<port>)

**Example:**

.. code-block:: pebl

   port <- OpenCOMPort(1, 9600, "8N1")
   byte <- COMPortGetByte(port)
   if(byte >= 0)
   {
      Print("Received byte: " + byte)
   }

**See Also:**

:func:`COMPortSendByte()`, :func:`OpenCOMPort()`

'''
    },

    'COMPortSendByte': {
        'insert_type': 'replace_stub',
        'stub_title': 'COMPortSendByte()',
        'content': '''
COMPortSendByte()
-----------------

*Sends a byte to the COM port*

**Description:**

Sends a single byte to an open COM/serial port. The byte should be an integer value between 0 and 255. This is used for serial communication with external devices.

**Usage:**

.. code-block:: pebl

   COMPortSendByte(<port>, <byte>)

**Example:**

.. code-block:: pebl

   port <- OpenCOMPort(1, 9600, "8N1")
   ##Send ASCII 'A' (65)
   COMPortSendByte(port, 65)

**See Also:**

:func:`COMPortGetByte()`, :func:`OpenCOMPort()`

'''
    },

    'GetHTTPFile': {
        'insert_type': 'replace_stub',
        'stub_title': 'GetHTTPFile()',
        'content': '''
GetHTTPFile()
-------------

*Downloads a file from a web server via HTTP*

**Description:**

Fetches a file from a web server using HTTP and saves it to a local file. Supports HTTP and HTTPS protocols. Useful for downloading stimuli, configuration files, or data from web servers during experiments.

**Usage:**

.. code-block:: pebl

   GetHTTPFile(<url>, <output_filename>, <username>, <password>)

**Example:**

.. code-block:: pebl

   ##Download without authentication
   success <- GetHTTPFile("http://example.com/data.csv", "local_data.csv", "", "")

   ##Download with authentication
   success <- GetHTTPFile("https://secure.example.com/file.zip", "download.zip", "user", "pass")

**See Also:**

:func:`GetHTTPText()`, :func:`PostHTTP()`, :func:`PostHTTPFile()`

'''
    },

    'GetHTTPText': {
        'insert_type': 'replace_stub',
        'stub_title': 'GetHTTPText()',
        'content': '''
GetHTTPText()
-------------

*Retrieves text content from a web server via HTTP*

**Description:**

Fetches content from a web server using HTTP and returns it as a text string. Supports HTTP and HTTPS protocols. Useful for retrieving instructions, configurations, or data from web services during experiments.

**Usage:**

.. code-block:: pebl

   GetHTTPText(<url>, <username>, <password>)

**Example:**

.. code-block:: pebl

   ##Fetch text without authentication
   text <- GetHTTPText("http://example.com/instructions.txt", "", "")
   Print(text)

   ##Fetch JSON data with authentication
   jsonData <- GetHTTPText("https://api.example.com/config", "user", "pass")
   config <- ParseJSON(jsonData)

**See Also:**

:func:`GetHTTPFile()`, :func:`PostHTTP()`, :func:`ParseJSON()`

'''
    },

    'GetMyIPAddress': {
        'insert_type': 'alphabetical',
        'after': 'GetData()',
        'before': 'GetPPortState()',
        'content': '''
GetMyIPAddress()
----------------

*Returns the local IP address of this computer*

**Description:**

Returns the IP address of the current computer as a formatted string. The returned address is suitable for use in networking functions. If multiple network interfaces are present, it typically returns the primary interface address.

**Usage:**

.. code-block:: pebl

   GetMyIPAddress(<interface_number>)

**Example:**

.. code-block:: pebl

   myIP <- GetMyIPAddress(0)
   Print("My IP address is: " + myIP)

**See Also:**

:func:`ConnectToHost()`, :func:`ConnectToIP()`, :func:`OpenNetworkListener()`

'''
    },

    'ParseJSON': {
        'insert_type': 'alphabetical',
        'after': 'OpenPPort()',
        'before': 'Print()',
        'content': '''
ParseJSON()
-----------

*Parses a JSON string into PEBL data structures*

**Description:**

Parses a JSON-formatted string and converts it into PEBL data structures (lists and custom objects). JSON objects become PEBL custom objects with properties, and JSON arrays become PEBL lists. This is useful for working with web APIs and configuration files.

**Usage:**

.. code-block:: pebl

   ParseJSON(<json_string>)

**Example:**

.. code-block:: pebl

   jsonString <- GetHTTPText("https://api.example.com/data", "", "")
   data <- ParseJSON(jsonString)

   ##Access parsed data
   Print(data.name)
   Print(data.values)

**See Also:**

:func:`GetHTTPText()`, :func:`PostHTTP()`, :func:`MakeCustomObject()`

'''
    },

    'PostHTTP': {
        'insert_type': 'replace_stub',
        'stub_title': 'PostHTTP()',
        'content': '''
PostHTTP()
----------

*Sends data to a web server via HTTP POST*

**Description:**

Sends data to a web server using the HTTP POST method. Returns the server's response as a string. This is useful for submitting experimental data to web servers, interacting with web APIs, or logging data remotely.

**Usage:**

.. code-block:: pebl

   PostHTTP(<url>, <post_data>, <username>, <password>, <content_type>)

**Example:**

.. code-block:: pebl

   ##Post form data
   postData <- "subjectID=123&condition=A&score=85"
   response <- PostHTTP("http://example.com/submit", postData, "", "", "application/x-www-form-urlencoded")

   ##Post JSON data
   jsonData <- "{\"subject\":123,\"rt\":450}"
   response <- PostHTTP("https://api.example.com/data", jsonData, "user", "pass", "application/json")

**See Also:**

:func:`PostHTTPFile()`, :func:`GetHTTPText()`, :func:`GetHTTPFile()`

'''
    },

    'PostHTTPFile': {
        'insert_type': 'new_stub',  # Add to stub section if not exists
        'content': '''
PostHTTPFile()
--------------

*Uploads a file to a web server via HTTP POST*

**Description:**

Uploads a file to a web server using HTTP POST multipart/form-data encoding. This is useful for uploading experimental data files, log files, or other content to a web server for storage or processing.

**Usage:**

.. code-block:: pebl

   PostHTTPFile(<url>, <filename>, <fieldname>, <username>, <password>, <additional_fields>)

**Example:**

.. code-block:: pebl

   ##Upload data file
   response <- PostHTTPFile("http://example.com/upload", "data.csv", "datafile", "", "", "")

   ##Upload with additional form fields
   additionalData <- "subjectID=123&session=1"
   response <- PostHTTPFile("https://secure.example.com/upload", "results.txt", "file", "user", "pass", additionalData)

**See Also:**

:func:`PostHTTP()`, :func:`GetHTTPFile()`, :func:`FileOpenRead()`

'''
    },

    'SetNetworkPort': {
        'insert_type': 'alphabetical',
        'after': 'SendData()',
        'before': 'SetPPortMode()',
        'content': '''
SetNetworkPort()
----------------

*Configures the default network port for connections*

**Description:**

Sets the default network port number to be used for subsequent network operations. This allows you to configure the port once rather than specifying it for each connection. The port number should be between 1024 and 65535 for user applications.

**Usage:**

.. code-block:: pebl

   SetNetworkPort(<port>)

**Example:**

.. code-block:: pebl

   SetNetworkPort(8080)
   listener <- OpenNetworkListener(8080)

**See Also:**

:func:`OpenNetworkListener()`, :func:`ConnectToHost()`, :func:`ConnectToIP()`

'''
    }
}


def find_section_end(lines, start_idx):
    """Find the end of a function documentation section."""
    i = start_idx + 1
    while i < len(lines):
        # Check for next function heading (line ending with ())
        if re.match(r'^[A-Z][a-zA-Z0-9_]+\(\)\s*$', lines[i]):
            return i
        # Check for section dividers
        if lines[i].strip().startswith('===='):
            return i
        i += 1
    return i


def replace_stub(lines, stub_title, new_content):
    """Replace a stub in the 'Functions Pending Documentation' section."""
    for i, line in enumerate(lines):
        if line.strip() == stub_title:
            # Found the stub title, now find where it ends
            # Stubs end at the next function or end of section
            end_idx = find_section_end(lines, i)
            # Remove the old stub and insert new content
            del lines[i:end_idx]
            lines.insert(i, new_content)
            return True, i
    return False, -1


def insert_alphabetically(lines, after_pattern, before_pattern, content):
    """Insert content between two functions alphabetically."""
    after_idx = None
    before_idx = None

    for i, line in enumerate(lines):
        if line.strip() == after_pattern:
            after_idx = i
        if line.strip() == before_pattern:
            before_idx = i

    if after_idx is None:
        return False, f"Could not find '{after_pattern}'"
    if before_idx is None:
        return False, f"Could not find '{before_pattern}'"

    # Find end of 'after' section
    insert_at = find_section_end(lines, after_idx)

    if insert_at >= before_idx:
        return False, f"Insertion point is after target section"

    lines.insert(insert_at, content)
    return True, f"Inserted at line {insert_at}"


def insert_new_stub(lines, content):
    """Add new stub to the end of the file."""
    lines.append(content)
    return True, "Added to end of file"


def insert_functions(filepath):
    """Insert all function documentation."""

    with open(filepath, 'r') as f:
        lines = f.readlines()

    results = []

    for func_name, func_data in FUNCTIONS.items():
        insert_type = func_data['insert_type']
        content = func_data['content']

        if insert_type == 'replace_stub':
            stub_title = func_data['stub_title']
            success, msg = replace_stub(lines, stub_title, content)
            results.append((func_name, 'replace_stub', success, msg))

        elif insert_type == 'alphabetical':
            after = func_data['after']
            before = func_data['before']
            success, msg = insert_alphabetically(lines, after, before, content)
            results.append((func_name, 'alphabetical', success, msg))

        elif insert_type == 'new_stub':
            success, msg = insert_new_stub(lines, content)
            results.append((func_name, 'new_stub', success, msg))

    # Write back
    with open(filepath, 'w') as f:
        f.writelines(lines)

    # Print results
    print("\nInsertion Results:")
    print("=" * 80)
    for func_name, insert_type, success, msg in results:
        status = "✓" if success else "✗"
        print(f"{status} {func_name} ({insert_type}): {msg}")

    successful = sum(1 for _, _, success, _ in results if success)
    print(f"\nSuccessfully processed {successful}/{len(results)} functions")


if __name__ == '__main__':
    filepath = '/home/smueller/Dropbox/Research/pebl/pebl/doc/sphinx-doc/source/reference/peblstream.rst'
    insert_functions(filepath)
