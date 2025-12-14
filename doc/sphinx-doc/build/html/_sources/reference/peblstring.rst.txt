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



.. index:: Format

Format()
--------

**Description:**

Formats the printing of values to ensure proper spacing. It will either truncate or pad ``<value>`` with spaces so that it ends up exactly ``<length>`` characters long. Character padding is at the end.

**Usage:**

.. code-block:: pebl

   Format(<value>, <length>)

**Example:**

.. code-block:: pebl

   x <- 33.23425225
   y <- 23.3
   Print("["+Format(x,5)+"]")
   Print("["+Format(y,5)+"]")
   ## Output:
   ## [33.23 ]
   ## [23.3  ]

**See Also:**

:func:`CR()`, :func:`Tab()`



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


.. index:: DetectTextScript

DetectTextScript()
------------------

*Detects the Unicode script of text*

**Description:**

Analyzes text and returns the ISO 15924 four-letter script code identifying the writing system used. Returns codes such as "Arab" (Arabic), "Hebr" (Hebrew), "Hani" (Han/Chinese), "Thai" (Thai), "Cyrl" (Cyrillic), etc. Returns an empty string for Latin script or when the script cannot be determined. This is useful for automatic font selection, determining text directionality, and providing appropriate text rendering for international content.

**Usage:**

.. code-block:: pebl

   DetectTextScript(<text>)

**Example:**

.. code-block:: pebl

   ## Detect scripts in different languages
   Print(DetectTextScript("Hello"))           ## "" (Latin)
   ## Arabic text "marhaba" (hello):
   Print(DetectTextScript(arabicText))        ## "Arab" (Arabic)
   ## Hebrew text "shalom" (hello):
   Print(DetectTextScript(hebrewText))        ## "Hebr" (Hebrew)
   ## Chinese text "nihao" (hello):
   Print(DetectTextScript(chineseText))       ## "Hani" (Chinese)
   ## Thai text "sawasdee" (hello):
   Print(DetectTextScript(thaiText))          ## "Thai" (Thai)
   ## Cyrillic text "privet" (hello):
   Print(DetectTextScript(cyrillicText))      ## "Cyrl" (Cyrillic)

**See Also:**

:func:`IsRTL()`, :func:`GetFontForText()`, :func:`GetSystemLocale()`


.. index:: IsRTL

IsRTL()
-------

*Determines if text or script is right-to-left*

**Description:**

Tests whether the input is right-to-left (RTL) text. The function accepts either actual text (which will be analyzed to detect its script) or a four-letter ISO 15924 script code. Returns 1 for RTL scripts (Arabic, Hebrew, etc.) and 0 for LTR scripts. This is essential for proper text layout, UI mirroring, and ensuring correct text directionality in international experiments.

**Usage:**

.. code-block:: pebl

   IsRTL(<text_or_script_code>)

**Example:**

.. code-block:: pebl

   ## Test with actual text
   Print(IsRTL("Hello"))                ## 0 (LTR)
   ## Arabic text "marhaba":
   Print(IsRTL(arabicText))             ## 1 (RTL - Arabic)
   ## Hebrew text "shalom":
   Print(IsRTL(hebrewText))             ## 1 (RTL - Hebrew)

   ## Test with script codes
   Print(IsRTL("Arab"))                 ## 1 (Arabic script is RTL)
   Print(IsRTL("Hebr"))                 ## 1 (Hebrew script is RTL)
   Print(IsRTL("Latn"))                 ## 0 (Latin script is LTR)

   ## Use for text justification
   text <- GetInput(textbox, "<return>")
   if(IsRTL(text))
   {
       textbox.hjustify <- "right"
   }

**See Also:**

:func:`DetectTextScript()`, :func:`IsSystemLocaleRTL()`, :func:`GetFontForText()`


.. index:: GetFontForText

GetFontForText()
----------------

*Returns appropriate font filename for text based on detected script*

**Description:**

Automatically selects an appropriate font for the given text by detecting its Unicode script. This ensures proper rendering of international text by choosing fonts that support the necessary character ranges. The optional font_type parameter specifies the font style: 0 for sans-serif (default), 1 for monospace, or 2 for serif. Returns a font filename suitable for use with :func:`MakeFont()`. The function uses the DejaVu and Noto font families which provide extensive Unicode coverage.

**Usage:**

.. code-block:: pebl

   GetFontForText(<text>)
   GetFontForText(<text>, <font_type>)

**Example:**

.. code-block:: pebl

   ## Automatic font selection for different languages
   ## arabicText would contain Arabic text "marhaba bik"
   fontFile <- GetFontForText(arabicText)
   font <- MakeFont(fontFile, 0, 24, MakeColor("black"), MakeColor("white"), 1)
   label <- MakeLabel(arabicText, font)

   ## Select monospace font for code display
   ## hebrewCode would contain Hebrew text
   monoFont <- GetFontForText(hebrewCode, 1)  ## 1 = monospace

   ## Select serif font for Thai text
   ## thaiText would contain Thai text "sawasdee"
   serifFont <- GetFontForText(thaiText, 2)   ## 2 = serif

**See Also:**

:func:`DetectTextScript()`, :func:`MakeFont()`, :func:`IsRTL()`


.. index:: GetSystemLocale

GetSystemLocale()
-----------------

*Retrieves the operating system's locale setting*

**Description:**

Returns the current system locale as configured in the operating system. The locale string typically follows the format "language_COUNTRY" (e.g., "en_US", "zh_CN", "ar_SA") or may be just a language code (e.g., "ar", "he"). Returns an empty string if locale detection fails. This is useful for automatically adapting experiment interfaces to the user's language and regional settings.

**Usage:**

.. code-block:: pebl

   GetSystemLocale()

**Example:**

.. code-block:: pebl

   ## Detect system locale and adapt interface
   locale <- GetSystemLocale()
   Print("System locale: " + locale)

   if(SubString(locale, 1, 2) == "ar")
   {
       ## Arabic locale detected
       gLanguage <- "ar"
   } elseif(SubString(locale, 1, 2) == "he")
   {
       ## Hebrew locale detected
       gLanguage <- "he"
   } else {
       ## Default to English
       gLanguage <- "en"
   }

   ## Possible outputs: "en_US", "ar_SA", "he_IL", "zh_CN", "es_MX", etc.

**See Also:**

:func:`IsSystemLocaleRTL()`, :func:`DetectTextScript()`, :func:`GetFontForText()`


.. index:: IsSystemLocaleRTL

IsSystemLocaleRTL()
-------------------

*Checks if the system locale uses right-to-left text*

**Description:**

Determines whether the operating system's current locale setting is for a right-to-left (RTL) language (Arabic or Hebrew). Returns 1 if the system locale is RTL, 0 if it is LTR. This is particularly useful for setting default text justification and UI layout before any text input occurs, ensuring that the interface matches the user's language expectations.

**Usage:**

.. code-block:: pebl

   IsSystemLocaleRTL()

**Example:**

.. code-block:: pebl

   ## Set default text justification based on system locale
   if(IsSystemLocaleRTL())
   {
       ## System is configured for RTL language (Arabic/Hebrew)
       defaultJustify <- "right"
       Print("RTL locale detected")
   } else {
       ## System is configured for LTR language
       defaultJustify <- "left"
       Print("LTR locale detected")
   }

   ## Create textbox with appropriate default alignment
   textbox <- MakeTextBox("", font, 400, 100)
   textbox.hjustify <- defaultJustify

**See Also:**

:func:`GetSystemLocale()`, :func:`IsRTL()`, :func:`DetectTextScript()`
