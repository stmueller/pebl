================================================================================
PEBLString - String Manipulation
================================================================================

This module contains functions for string manipulation and formatting.

.. contents:: Function Index
   :local:
   :depth: 0



.. index:: CopyToClipboard

CopyToClipboard()
-----------------

*Puts argument in system clipboard.*

**Description:**

Puts text into the the system clipboard, so that it can be accessed either by another program or by the ``Copyfromclipboard`` function. Note that, possibly depending on platform, text copied into the clipboard by PEBL may not stay there after PEBL exits.

**Example:**

.. code-block:: pebl

   text <- GetInput(textbox,"<enter>")
   CopyToClipboard(text)
   MessageBox("Text : " + text + " copied to clipboard",gWin)

**See Also:**

:func:`CopyFromClipboard()`



.. index:: FindInString

FindInString()
--------------

**Description:**

Finds a token in a string, returning the position (starting at a particular position).

**Usage:**

.. code-block:: pebl

   FindInString(<basestring>,<searchstring>,<startingpos>)

**Example:**

.. code-block:: pebl

   FindInString("about","bo",1) 	# == 2
   FindInString("banana","na",1) 	# == 3
   FindInString("banana","na",4) 	# == 5

**See Also:**

:func:`SplitString()`



.. index:: Lowercase

Lowercase()
-----------

*Returns lowercased string*

**Description:**

Changes a string to lowercase.  Useful for testing user 		input against a stored value, to ensure case differences 		are not detected.

**Usage:**

.. code-block:: pebl

   Lowercase(<string>)

**Example:**

.. code-block:: pebl

   Lowercase("POtaTo")	# == "potato"

**See Also:**

:func:`Uppercase()`



.. index:: SplitString

SplitString()
-------------

**Description:**

Splits a string into tokens. ``<split>`` must be a string. If  		``<split>`` is not found in ``<string>``, a list containing the entire  		string is returned; if split is equal to ``""``, the each letter  		in the string is placed into a different item in the list.  Only the first character of ``<split>`` is used.  IF you need a multicharacter split, you can use ``<SplitStringSlow>``, which can handle multi-character splits but is relatively slower. This should not matter for short strings, but if you are using splitstring on long files, it could make a difference.

**Usage:**

.. code-block:: pebl

   SplitString(<string>, <split>)

**Example:**

.. code-block:: pebl

   SplitString("Everybody Loves a Clown", " ") 
   # Produces ["Everybody", "Loves", "a", "Clown"]

**See Also:**

:func:`FindInString()`, :func:`ReplaceChar`, :func:`SplitStringSlow`



.. index:: StringLength

StringLength()
--------------

*Returns the length of a string*

**Description:**

Determines the length of a string, in characters.

**Usage:**

.. code-block:: pebl

   StringLength(<string>)

**Example:**

.. code-block:: pebl

   StringLength("absolute")     # == 8
   StringLength("   spaces   ") # == 12
   StringLength("")             # == 0

**See Also:**

:func:`Length()`, :func:`SubString()`



.. index:: SubString

SubString()
-----------

*Returns a substring*

**Description:**

Extracts a substring from a longer string.

**Usage:**

.. code-block:: pebl

   SubString(<string>,<position>,<length>)

**Example:**

.. code-block:: pebl

   SubString("abcdefghijklmnop",3,5)	# == "cdefg"

**See Also:**

:func:`StringLength()`, :func:`FindInString()`



.. index:: ToASCII

ToASCII()
---------

*Converts an ASCII code to a character*

**Description:**

Converts an integer ASCII code to its corresponding single-character string. This is useful for creating special characters or control characters from their numeric codes.

**Usage:**

.. code-block:: pebl

   ToASCII(<ascii_code>)

**Example:**

.. code-block:: pebl

   ToASCII(65)    # == "A"
   ToASCII(97)    # == "a"
   ToASCII(32)    # == " " (space)
   ToASCII(10)    # == newline character

**See Also:**

:func:`StringLength()`, :func:`Uppercase()`, :func:`Lowercase()`



.. index:: Uppercase

Uppercase()
-----------

*Returns uppercased string*

**Description:**

Changes a string to uppercase.  Useful for testing user 	      	input against a stored value, to ensure case differences 	      	are not detected.

**Usage:**

.. code-block:: pebl

   Uppercase(<string>)

**Example:**

.. code-block:: pebl

   Uppercase("POtaTo")  # == "POTATO"

**See Also:**

:func:`Lowercase()`
