================================================================================
LSL - Lab Streaming Layer Built-in Functions
================================================================================

These are C++ built-in functions that interface directly with the `Lab Streaming Layer <https://github.com/sccn/labstreaminglayer>`_ (LSL) library.  LSL enables synchronized multi-modal data acquisition for neuroscience research — for example, aligning EEG recordings with stimulus event markers from a PEBL experiment.

LSL must be enabled at run time by passing ``--lsl [streamname]`` on the command line.  When LSL is not enabled these functions will fail; use the higher-level PEBL library wrappers :func:`InitializeLSL()`, :func:`LSLMarker()`, and :func:`FinalizeLSL()` (defined in ``Utility.pbl``) which automatically guard against calling these functions when LSL is disabled.

.. contents:: Function Index
   :local:
   :depth: 0


.. index:: CreateLSLOutlet

CreateLSLOutlet()
-----------------

*Creates an LSL outlet (stream) that recording software can subscribe to.*

**Description:**

Creates a named LSL outlet of type ``type`` and registers it on the local network so that LSL-compatible recording software (e.g., LabRecorder) can discover and record from it.  Returns 1 on success, 0 on failure.

After creating an outlet, use :func:`SendLSLMarker()` to push samples and :func:`CloseLSLOutlet()` when finished.

For most experiments, use the higher-level :func:`InitializeLSL()` wrapper instead of calling this directly.

**Usage:**

.. code-block:: pebl

   CreateLSLOutlet(streamName, type, sourceID)

**Parameters:**

- ``streamName`` — display name for the stream (e.g., ``"PEBL_stroop"``)
- ``type`` — stream type (e.g., ``"Markers"``); use ``"Markers"`` for event-marker streams
- ``sourceID`` — unique source identifier string; pass ``""`` to auto-generate

**Example:**

.. code-block:: pebl

   success <- CreateLSLOutlet("MyExperiment", "Markers", "")
   if(success)
   {
       Print("LSL outlet created")
   }

**See Also:**

:func:`SendLSLMarker()`, :func:`CloseLSLOutlet()`, :func:`InitializeLSL()`


.. index:: SendLSLMarker

SendLSLMarker()
---------------

*Pushes a single event marker to the LSL outlet.*

**Description:**

Sends ``marker`` (a string) to the LSL outlet created by :func:`CreateLSLOutlet()`.  LSL automatically timestamps the sample using the LSL clock.  For most experiments, use the higher-level :func:`LSLMarker()` wrapper, which is a no-op when LSL is disabled.

**Usage:**

.. code-block:: pebl

   SendLSLMarker(marker)

**Example:**

.. code-block:: pebl

   SendLSLMarker("stimulus_onset")
   Draw()
   response <- WaitForListKeyPress(["z", "/"])
   SendLSLMarker("response")

**See Also:**

:func:`CreateLSLOutlet()`, :func:`LSLMarker()`


.. index:: CloseLSLOutlet

CloseLSLOutlet()
----------------

*Closes the LSL outlet and releases resources.*

**Description:**

Destroys the current LSL outlet and frees the associated resources.  Call once at the end of the experiment.  For most experiments, use the higher-level :func:`FinalizeLSL()` wrapper.

**Usage:**

.. code-block:: pebl

   CloseLSLOutlet()

**Example:**

.. code-block:: pebl

   MessageBox("Experiment complete.", gWin)
   CloseLSLOutlet()

**See Also:**

:func:`CreateLSLOutlet()`, :func:`FinalizeLSL()`


.. index:: LSLHasConsumers

LSLHasConsumers()
-----------------

*Tests whether any recording application is subscribed to the LSL outlet.*

**Description:**

Returns 1 if at least one LSL inlet (consumer, e.g., LabRecorder) is currently subscribed to the outlet, or 0 if no consumers are connected.  Useful for displaying a warning before starting the experiment if recording software is not running.

**Usage:**

.. code-block:: pebl

   LSLHasConsumers()

**Example:**

.. code-block:: pebl

   CreateLSLOutlet("PEBL_task", "Markers", "")
   if(not LSLHasConsumers())
   {
       MessageBox("WARNING: No LSL consumer detected.  Start LabRecorder first.", gWin)
   }

**See Also:**

:func:`CreateLSLOutlet()`, :func:`InitializeLSL()`


.. index:: LSLLocalClock

LSLLocalClock()
---------------

*Returns the current LSL clock time in seconds.*

**Description:**

Returns the current time on the LSL clock as a floating-point number of seconds.  The LSL clock is synchronized across devices on the same network and is the same clock used to timestamp markers sent with :func:`SendLSLMarker()`.  Use this to record precise stimulus-onset times that can be aligned with EEG or other LSL streams in post-processing.

**Usage:**

.. code-block:: pebl

   LSLLocalClock()

**Example:**

.. code-block:: pebl

   Draw()
   lslOnset <- LSLLocalClock()
   SendLSLMarker("stimulus_onset")
   ## ... wait for response ...
   FilePrint(gDataFile, gSubNum + "," + trial + "," + lslOnset)

**See Also:**

:func:`SendLSLMarker()`, :func:`GetTime()`
