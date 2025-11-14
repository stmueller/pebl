================================================================================
HTML Library - HTML Generation
================================================================================

This library contains functions for generating HTML markup and web content.

.. contents:: Function Index
   :local:
   :depth: 0



.. index:: B

B()
---

**Description:**

Implements the HTML ``<b>`` tag. Wraps the provided text in bold tags to display it in bold font weight when rendered in HTML.

**Usage:**

.. code-block:: pebl

   define B(text)

**Example:**

.. code-block:: pebl

   report <- B("Important Result")
   FilePrint(file, report)
   ## Produces: <b>Important Result</b>

**See Also:**

:func:`P()`, :func:`H()`, :func:`Entag()`



.. index:: BR

BR()
----

**Description:**

Implements the HTML ``<br>`` tag. Returns a line break tag to create a new line in HTML output without starting a new paragraph.

**Usage:**

.. code-block:: pebl

   define BR()

**Example:**

.. code-block:: pebl

   text <- "Line 1" + BR() + "Line 2"
   FilePrint(file, text)
   ## Produces: Line 1<br>Line 2

**See Also:**

:func:`P()`, :func:`HL()`



.. index:: CT

CT()
----

**Description:**

Closes an HTML tag by generating the closing tag syntax. This is a helper function used by other HTML functions to create properly formatted closing tags. Takes a tag name and returns ``</tagname>``.

**Usage:**

.. code-block:: pebl

   define CT(tag)

**Example:**

.. code-block:: pebl

   closing <- CT("div")
   ## Produces: </div>

**See Also:**

:func:`OT()`, :func:`Entag()`



.. index:: Entag

Entag()
-------

**Description:**

Generic function to wrap body content in any HTML tag. Takes a tag name and body text, then returns the body wrapped in opening and closing tags. Useful for creating custom HTML elements not covered by specific functions.

**Usage:**

.. code-block:: pebl

   define Entag(tag, body)

**Example:**

.. code-block:: pebl

   emphasized <- Entag("em", "This is important")
   ## Produces: <em>This is important</em>
   
   div <- Entag("div", "Content in a div")
   ## Produces: <div>Content in a div</div>

**See Also:**

:func:`OT()`, :func:`CT()`, :func:`P()`, :func:`B()`



.. index:: H

H()
---

**Description:**

Implements HTML header tags ``<h1>`` through ``<h6>``. Takes text and a level (1-6) and wraps the text in the appropriate header tag. Level 1 is the largest heading, level 6 is the smallest.

**Usage:**

.. code-block:: pebl

   define H(text, level)

**Example:**

.. code-block:: pebl

   title <- H("Test Results", 1)
   ## Produces: <h1>Test Results</h1>
   
   subhead <- H("Section A", 2)
   ## Produces: <h2>Section A</h2>

**See Also:**

:func:`P()`, :func:`B()`



.. index:: HL

HL()
----

**Description:**

Creates a horizontal line element in HTML. Returns ``<hl>`` tag. Note: This appears to be a non-standard tag; standard HTML uses ``<hr>`` for horizontal rules.

**Usage:**

.. code-block:: pebl

   define HL()

**Example:**

.. code-block:: pebl

   separator <- HL()
   FilePrint(file, separator)

**See Also:**

:func:`BR()`, :func:`P()`



.. index:: Img

Img()
-----

**Description:**

Implements the HTML ``<img>`` tag. Creates an image element with specified filename and width. The filename should be a path to the image file, and width is specified in pixels.

**Usage:**

.. code-block:: pebl

   define Img(filename, width)

**Example:**

.. code-block:: pebl

   image <- Img("results_chart.png", 600)
   ## Produces: <img src='results_chart.png' width=600/>

**See Also:**

:func:`Page()`, :func:`Table()`



.. index:: MakeDivPage

MakeDivPage()
-------------

**Description:**

Creates a page-formatted div container with automatic page numbering. Uses CSS classes 'page' and 'subpage' for styling (defined in ``Page()`` stylesheet). Automatically increments a global page counter (gPage) for multi-page reports. Useful for creating printable reports with consistent page formatting.

**Usage:**

.. code-block:: pebl

   define MakeDivPage(text)

**Example:**

.. code-block:: pebl

   page1 <- MakeDivPage(H("Report", 1) + P("Page content here"))
   page2 <- MakeDivPage(H("Continued", 2) + P("More content"))
   report <- Page(page1 + page2)
   FilePrint(file, report)

**See Also:**

:func:`Page()`, :func:`H()`, :func:`P()`



.. index:: OT

OT()
----

**Description:**

Opens an HTML tag by generating the opening tag syntax. This is a helper function used by other HTML functions to create properly formatted opening tags. Takes a tag name and returns ``<tagname>``.

**Usage:**

.. code-block:: pebl

   define OT(tag)

**Example:**

.. code-block:: pebl

   opening <- OT("div")
   ## Produces: <div>

**See Also:**

:func:`CT()`, :func:`Entag()`



.. index:: P

P()
---

**Description:**

Implements the HTML ``<p>`` tag. Wraps the provided text in paragraph tags to create a standard HTML paragraph element.

**Usage:**

.. code-block:: pebl

   define P(text)

**Example:**

.. code-block:: pebl

   paragraph <- P("This is the first paragraph of the report.")
   FilePrint(file, paragraph)
   ## Produces: <p>This is the first paragraph of the report.</p>

**See Also:**

:func:`H()`, :func:`B()`, :func:`BR()`



.. index:: Page

Page()
------

**Description:**

Creates a complete HTML document with CSS styling suitable for printable reports. Wraps content in full HTML structure including head, style, and body tags. Provides default CSS for letter-size pages with print-friendly styling, or accepts custom CSS. The default stylesheet includes responsive table styling and page formatting optimized for 8.5x11 inch paper.

**Usage:**

.. code-block:: pebl

   define Page(text, style:0)

**Example:**

.. code-block:: pebl

   content <- H("Test Report", 1) + P("Results below:") + Table(data, ["Name", "Score"])
   html <- Page(content)
   FilePrint(file, html)
   
   ## With custom CSS:
   customCSS <- "body { font-family: Arial; }"
   html <- Page(content, customCSS)

**See Also:**

:func:`MakeDivPage()`, :func:`Table()`, :func:`H()`, :func:`P()`



.. index:: Table

Table()
-------

**Description:**

Implements HTML ``<table>`` markup. Converts a nested list (list of rows, where each row is a list of cells) into an HTML table. Optionally accepts a header list to create table column headers using ``<thead>`` and ``<th>`` tags. Data rows are automatically wrapped in ``<tr>`` and ``<td>`` tags. Works with the CSS styling provided by ``Page()`` for formatted, printable tables.

**Usage:**

.. code-block:: pebl

   define Table(tab, header:"")

**Example:**

.. code-block:: pebl

   ## Simple table without headers:
   data <- [["John", 85], ["Mary", 92], ["Bob", 78]]
   table <- Table(data)
   
   ## Table with headers:
   data <- [["John", 85], ["Mary", 92], ["Bob", 78]]
   headers <- ["Name", "Score"]
   table <- Table(data, headers)
   
   ## In a full report:
   report <- Page(H("Results", 1) + Table(data, headers))
   FilePrint(file, report)

**See Also:**

:func:`Page()`, :func:`MakeDivPage()`, :func:`Entag()`
