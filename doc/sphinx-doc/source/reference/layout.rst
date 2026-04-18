================================================================================
Layout Library - Zone-Based Layouts and Response Handling
================================================================================

The Layout library (``Layout.pbl``) provides a unified system for creating experiment screens with automatic zone positioning, platform-aware response handling, and visual feedback animations.  It is the recommended way to build trial displays in PEBL 2.4.

A *layout* is a custom object that contains all screen regions (header, stimulus area, footer, response labels) and knows how to collect responses in a mode appropriate for the current platform (keyboard, mouse button, or touch target).

.. contents:: Function Index
   :local:
   :depth: 0


.. index:: CreateLayout

CreateLayout()
--------------

*Creates a standard zone-based trial layout.*

**Description:**

Creates a layout object for ``testName`` in window ``win``.  The layout divides the screen into a header zone, a stimulus region, a footer zone, and a response-label area.  All positions are calculated automatically and scale to the current screen resolution.

If ``params`` is provided, it configures the response mode:

- ``params.responsemode`` — response mode name (e.g., ``"spacebar"``, ``"auto"``, ``"userselect"``, ``"none"``)
- ``params.responsesemantics`` — list of semantic response names (e.g., ``["left", "right"]``)
- ``params.responselabels`` — display labels shown on-screen for each response
- ``params.customkeys`` — explicit key list override
- ``params.customlabels`` — explicit label list override

The layout object is also stored in the global ``gLayout`` for convenience.

Returns the layout object.  Access screen coordinates via ``layout.centerX``, ``layout.centerY`` for placing stimuli.

**Usage:**

.. code-block:: pebl

   define CreateLayout(testName, win, params:0)

**Example:**

.. code-block:: pebl

   gParams <- CreateParameters(parpairs, gParamFile)
   gWin    <- MakeWindow("black")
   layout  <- CreateLayout("simon", gWin, gParams)

   layout.header.text <- "Simon Task"
   stim <- EasyLabel("LEFT", layout.centerX, layout.centerY, gWin, 72)
   Draw()
   resp <- WaitForLayoutResponse(layout, 2000)

**See Also:**

:func:`WaitForLayoutResponse()`, :func:`WaitForLayoutResponseOrExit()`, :func:`ApplyDarkTheme()`


.. index:: WaitForLayoutResponse

WaitForLayoutResponse()
-----------------------

*Waits for a response according to the layout's response mode.*

**Description:**

Blocks until the participant makes a response (key press, mouse click, or touch, depending on the layout's response mode) or until ``timeout`` milliseconds have elapsed.  Returns the *semantic* response string (e.g., ``"left"``, ``"right"``, ``"respond"``) rather than the raw key.  Returns ``"<timeout>"`` if the timeout expires.

Pass ``timeout`` of 0 (the default) to wait indefinitely.

**Usage:**

.. code-block:: pebl

   define WaitForLayoutResponse(layout, timeout:0)

**Example:**

.. code-block:: pebl

   Draw()
   resp <- WaitForLayoutResponse(layout, 3000)
   if(resp == "<timeout>")
   {
       ## no response
   } elseif(resp == "left") {
       ## handle left
   } elseif(resp == "right") {
       ## handle right
   }

**See Also:**

:func:`CreateLayout()`, :func:`WaitForLayoutResponseOrExit()`


.. index:: WaitForLayoutResponseOrExit

WaitForLayoutResponseOrExit()
------------------------------

*Waits for a trial response or an exit/continue signal.*

**Description:**

Like :func:`WaitForLayoutResponse()`, but also watches for an exit signal (spacebar in keyboard mode, middle mouse button in mouse-button mode, or a click on ``exitTargets`` in mouse-target mode).  Returns the semantic response string on a normal response, or ``"exit"`` when the exit signal is received.  Useful for interactive practice loops where the experimenter needs to be able to move on.

**Usage:**

.. code-block:: pebl

   define WaitForLayoutResponseOrExit(layout, exitTargets:0)

**Example:**

.. code-block:: pebl

   practiceActive <- 1
   while(practiceActive)
   {
       Draw()
       resp <- WaitForLayoutResponseOrExit(layout)
       if(resp == "exit") { practiceActive <- 0 }
       elseif(resp == "left")  { ## handle left  }
       elseif(resp == "right") { ## handle right }
   }

**See Also:**

:func:`WaitForLayoutResponse()`, :func:`CreateLayout()`


.. index:: FadeIn

FadeIn()
--------

*Fades a label in by animating its alpha from 0 to 255.*

**Description:**

Animates ``label`` from fully transparent to fully opaque over ``durationMs`` milliseconds (default 1000 ms) by stepping through alpha values.

**Usage:**

.. code-block:: pebl

   define FadeIn(label, durationMs:1000)

**Example:**

.. code-block:: pebl

   stim <- EasyLabel("Ready?", gVideoWidth/2, gVideoHeight/2, gWin, 48)
   FadeIn(stim, 500)

**See Also:**

:func:`FadeOut()`, :func:`PulseLabel()`


.. index:: FadeOut

FadeOut()
---------

*Fades a label out by animating its alpha from 255 to 0.*

**Description:**

Animates ``label`` from fully opaque to fully transparent over ``durationMs`` milliseconds (default 1000 ms).

**Usage:**

.. code-block:: pebl

   define FadeOut(label, durationMs:1000)

**Example:**

.. code-block:: pebl

   FadeOut(stim, 300)

**See Also:**

:func:`FadeIn()`, :func:`PulseLabel()`


.. index:: FlashCorrect

FlashCorrect()
--------------

*Flashes a label green to indicate a correct response.*

**Description:**

Briefly changes ``label``'s background to green for ``durationMs`` milliseconds (default 500 ms), then restores the original color.

**Usage:**

.. code-block:: pebl

   define FlashCorrect(label, durationMs:500)

**Example:**

.. code-block:: pebl

   if(correct)
   {
       FlashCorrect(feedbackLabel, 600)
   }

**See Also:**

:func:`FlashIncorrect()`, :func:`HighlightResponse()`


.. index:: FlashIncorrect

FlashIncorrect()
----------------

*Flashes a label red to indicate an incorrect response.*

**Description:**

Briefly changes ``label``'s background to red for ``durationMs`` milliseconds (default 500 ms), then restores the original color.

**Usage:**

.. code-block:: pebl

   define FlashIncorrect(label, durationMs:500)

**Example:**

.. code-block:: pebl

   if(not correct)
   {
       FlashIncorrect(feedbackLabel, 600)
   }

**See Also:**

:func:`FlashCorrect()`, :func:`HighlightResponse()`


.. index:: PulseLabel

PulseLabel()
------------

*Pulses a label by briefly increasing its font size.*

**Description:**

Increases ``label``'s font size by ``pulseSize`` points (default 10), then returns it to its original size.  Repeats ``count`` times (default 3).  Useful for drawing attention to a target or to confirm a response.

**Usage:**

.. code-block:: pebl

   define PulseLabel(label, count:3, pulseSize:10)

**Example:**

.. code-block:: pebl

   PulseLabel(targetLabel)
   PulseLabel(targetLabel, 2, 20)

**See Also:**

:func:`FadeIn()`, :func:`HighlightResponse()`


.. index:: HighlightResponse

HighlightResponse()
-------------------

*Highlights a label with a yellow background and black text.*

**Description:**

Changes ``label``'s background to yellow and foreground to black for ``durationMs`` milliseconds (default 1000 ms), then restores the original colors.  Useful for echoing back the participant's selected response option.

**Usage:**

.. code-block:: pebl

   define HighlightResponse(label, durationMs:1000)

**Example:**

.. code-block:: pebl

   HighlightResponse(Nth(layout.responseLabels, 1), 800)

**See Also:**

:func:`FlashCorrect()`, :func:`FlashIncorrect()`


.. index:: SetLabelColor

SetLabelColor()
---------------

*Sets a label's foreground color by name.*

**Description:**

Sets ``label``'s foreground (text) color to the named color and calls :func:`Draw()`.

**Usage:**

.. code-block:: pebl

   define SetLabelColor(label, colorName)

**Example:**

.. code-block:: pebl

   SetLabelColor(stimLabel, "red")
   SetLabelColor(stimLabel, "blue")

**See Also:**

:func:`SetLabelBackground()`, :func:`MakeColor()`


.. index:: SetLabelBackground

SetLabelBackground()
--------------------

*Sets a label's background color with RGBA values.*

**Description:**

Sets ``label``'s background color using explicit red, green, and blue components (0–255) and an optional alpha (0–255, default 255).  Calls :func:`Draw()`.

**Usage:**

.. code-block:: pebl

   define SetLabelBackground(label, red, green, blue, alpha:255)

**Example:**

.. code-block:: pebl

   SetLabelBackground(stimLabel, 50, 50, 200, 255)   ## opaque dark blue
   SetLabelBackground(stimLabel, 0, 200, 0,   128)   ## semi-transparent green

**See Also:**

:func:`SetLabelColor()`, :func:`MakeColor()`


.. index:: ScaleFont

ScaleFont()
-----------

*Scales a label's font size by a multiplier.*

**Description:**

Multiplies ``label``'s current font size by ``scaleFactor`` (rounding to the nearest integer) and calls :func:`Draw()`.

**Usage:**

.. code-block:: pebl

   define ScaleFont(label, scaleFactor)

**Example:**

.. code-block:: pebl

   ScaleFont(headerLabel, 1.5)   ## make 50% larger
   ScaleFont(headerLabel, 0.8)   ## make 20% smaller

**See Also:**

:func:`SetLabelColor()`, :func:`ApplyAccessibilitySettings()`


.. index:: StyleForDifficulty

StyleForDifficulty()
--------------------

*Adjusts layout appearance to reflect task difficulty.*

**Description:**

Changes the color scheme of ``layout`` based on ``difficulty``:

- ``"easy"`` — green tones
- ``"medium"`` — yellow/amber tones
- ``"hard"`` — red tones

This provides a passive visual cue about trial difficulty without changing the task.

**Usage:**

.. code-block:: pebl

   define StyleForDifficulty(layout, difficulty)

**Example:**

.. code-block:: pebl

   StyleForDifficulty(layout, "hard")
   Draw()
   resp <- WaitForLayoutResponse(layout, 3000)

**See Also:**

:func:`ApplyDarkTheme()`, :func:`ApplyHighContrastTheme()`


.. index:: ApplyDarkTheme

ApplyDarkTheme()
----------------

*Applies a dark color theme to the layout.*

**Description:**

Sets the window background to a dark gray (RGB 40, 40, 40) and changes all layout text labels (header, subheader, footer, response labels) to white.  Calls :func:`Draw()`.

**Usage:**

.. code-block:: pebl

   define ApplyDarkTheme(layout)

**Example:**

.. code-block:: pebl

   layout <- CreateLayout("task", gWin, gParams)
   ApplyDarkTheme(layout)

**See Also:**

:func:`ApplyHighContrastTheme()`, :func:`ApplyAccessibilitySettings()`


.. index:: ApplyHighContrastTheme

ApplyHighContrastTheme()
------------------------

*Applies a high-contrast yellow-on-black theme to the layout.*

**Description:**

Sets the window background to yellow and all text labels to black, and increases header font size by 50%.  Designed for participants with low vision.  Calls :func:`Draw()`.

**Usage:**

.. code-block:: pebl

   define ApplyHighContrastTheme(layout)

**Example:**

.. code-block:: pebl

   ApplyHighContrastTheme(layout)

**See Also:**

:func:`ApplyDarkTheme()`, :func:`ApplyAccessibilitySettings()`


.. index:: ApplyAccessibilitySettings

ApplyAccessibilitySettings()
-----------------------------

*Applies accessibility display settings from a parameter object.*

**Description:**

Reads accessibility flags from ``params`` and applies the corresponding theme modifications:

- ``params.highContrast`` — if true, applies :func:`ApplyHighContrastTheme()`
- ``params.largeText`` — if true, increases all font sizes by 50%
- ``params.colorBlind`` — if true, switches to black-on-white with no color coding

Any combination of flags may be active simultaneously.

**Usage:**

.. code-block:: pebl

   define ApplyAccessibilitySettings(layout, params)

**Example:**

.. code-block:: pebl

   gParams.highContrast <- 1
   layout <- CreateLayout("task", gWin, gParams)
   ApplyAccessibilitySettings(layout, gParams)

**See Also:**

:func:`ApplyDarkTheme()`, :func:`ApplyHighContrastTheme()`
