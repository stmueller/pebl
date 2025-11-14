================================================================================
PEBLEnvironment - System and Environment
================================================================================

This module contains functions for system interaction, timing, input/output, and environment management.

.. contents:: Function Index
   :local:
   :depth: 0



.. index:: ClearEventLoop

ClearEventLoop()
----------------

*Clears all trigger events from event loop*

**Description:**

Clears the event loop.  This function is currently experimental, and its usage may change in future versions of PEBL.

**Usage:**

.. code-block:: pebl
   ## shows a way to generate custom WaitForMouseButton
   RegisterEvent("<MOUSE_BUTTON_PRESS>",1,1,"<EQUAL>","", [])
   out <-   StartEventLoop()
   ClearEventLoop()


**See Also:**

:func:`RegisterEvent()`, :func:`StartEventLoop()`



.. index:: CopyFromClipboard

CopyFromClipboard()
-------------------

*Copies text from system clipboard.*

**Description:**

This copies text currently living in the system clipboard. Note that (depending on platform), text copied into the clipboard may not remain there after PEBL exits.

**Example:**

.. code-block:: pebl

   text <- CopyFromClipboard()
   	textbox.text <- text

**See Also:**

:func:`CopyToClipboard()`



.. index:: CallFunction

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



.. index:: DeleteFile

DeleteFile()
------------

*Deletes a file*

**Description:**

Deletes a file from the file system.

**Usage:**

.. code-block:: pebl

   DeleteFile( <filename>)

**Example:**

.. code-block:: pebl

   tmpfile <- FileOpenWrite("tmp.txt")
   FilePrint(tmpfile,Random())
   FileClose(tmpfile)
   text <- FileReadText("tmp.txt")
   DeleteFile("tmp.txt")

**See Also:**

:func:`GetDirectoryListing()`, :func:`FileExists()`,       :func:`IsDirectory()`,            :func:`MakeDirectory()`



.. index:: ExitQuietly

ExitQuietly()
-------------

**Description:**

Stops PEBL and prints ``<message>`` to stderr. Unlike SignalFatalError, it will NOT pop-up a window with the error message. Useful exiting a study or application without causing a popup error message.

**Usage:**

.. code-block:: pebl

   ExitQuietly(<message>)

**Example:**

.. code-block:: pebl

   If(response == "exit")
   	{
      	   ExitQuietly("Exiting study.")
   	}
   	##Prints out error message and 
   	##line/filename of function

**See Also:**

``MessageBox``, :func:`Print()`, ``SignalFatalError()``



.. index:: FileExists

FileExists()
------------

*Checks whether a file exists*

**Description:**

Checks whether a file exists.  Returns 1 if it exists, 0 otherwise.

**Usage:**

.. code-block:: pebl

   FileExists(<path>)

**Example:**

.. code-block:: pebl

   filename <- "data-"+gSubNum+".csv"
    exists <-  FileExists(filename)
     if(exists)
      {
       MessageBox("Subject file already exists. "+
       " Please try a new one.",gWin) 
       SignalFatalError("filename already used")
      }

**See Also:**

:func:`GetDirectoryListing()`, :func:`FileExists()`,       :func:`IsDirectory()`,            :func:`MakeDirectory()`



.. index:: GetCurrentScreenResolution

GetCurrentScreenResolution()
----------------------------

**Description:**

Returns an list of [width,height] specifying what the  current computer screen resolution is.  This is used within the pebl launcher in order to use the current resolution to run the experiment.

**Usage:**

.. code-block:: pebl

   res <- GetCurrentScreenResolution()

**Example:**

.. code-block:: pebl

   define Start(p)
   {
      ## For testing, let's make the screen resolution a bit smaller than the 
      ## current one so that it doesn't get hidden by the bottom task bar
      ##
      res <- GetCurrentScreenResolution()
      gVideoWidth <- First(res)-100
      gVideoHeight <- Second(res)-100
      gWin <- MakeWindow()
      MessageBox("Window slightly smaller than screen",gWin)
   }

**See Also:**

:func:`GetVideoModes()`



.. index:: GetDirectoryListing

GetDirectoryListing()
---------------------

*Returns a list of all the files/subdirectories in a path*

**Description:**

Returns a list of files and directories in a particular directory/folder.

**Usage:**

.. code-block:: pebl

   list <- GetDirectoryListing(<path>)

**Example:**

.. code-block:: pebl

   files <-  GetDirectoryListing("./")

**See Also:**

:func:`GetDirectoryListing()`, :func:`FileExists()`,       :func:`IsDirectory()`,            :func:`MakeDirectory()`



.. index:: GetDrivers

GetDrivers()
------------

*Gets a list of possible video drivers*

**Description:**

Gets a list of video drivers on the current platform. This is usually one of opengl, opengles, software, and directx, different ones of which are available on different platforms.  This is most useful for building launchers, although it could be used within a script *before* MakeWindow is called to choose the best available driver.

**Usage:**

.. code-block:: pebl

   drivers <- GetDrivers()

**See Also:**

:func:`GetCurrentScreenResolution`, :func:`gVideoWidth`, :func:`gVideoHeight`,   	:func:`GetVideoModes`



.. index:: GetExecutableName

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



.. index:: GetHomeDirectory

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



.. index:: GetJoystickAxisState

GetJoystickAxisState()
----------------------

*Gets the state of a joystick axis*

**Description:**

This gets the state of a particular joystick axis.  You need to specify a joystick object, which is created with OpenJoystick().  You also need to specify the axis.  You can determine how many axes a joystick has with the GetNumJoystickAxes() function.  The function returns  a value between 1 and 32768.

**See Also:**

GetNumJoysticks(), OpenJoystick(), GetNumJoystickAxes() GetNumJoystickBalls(), GetNumJoystickButtons(), GetNumJoystickHats() GetJoystickAxisState(), GetJoystickHatState(), GetJoystickButtonState()



.. index:: GetJoystickBallState

GetJoystickBallState()
----------------------

*Gets the state of a joystick ball*

**Description:**

Not implemented.

**See Also:**

GetNumJoysticks(), OpenJoystick(), GetNumJoystickAxes() GetNumJoystickBalls(), GetNumJoystickButtons(), GetNumJoystickHats() GetJoystickAxisState(), GetJoystickHatState(), GetJoystickButtonState()



.. index:: GetJoystickButtonState

GetJoystickButtonState()
------------------------

**Description:**

This gets the state of a particular joystick button.  You need to specify a joystick object, which is created with OpenJoystick().  You also need to specify the button.  You can determine how many buttons a joystick has with the GetNumJoystickButtons() function.  The function returns either 0 (for unpressed) or 1 (for pressed).

**See Also:**

GetNumJoysticks(), OpenJoystick(), GetNumJoystickAxes() GetNumJoystickBalls(), GetNumJoystickButtons(), GetNumJoystickHats() GetJoystickAxisState(), GetJoystickHatState(), GetJoystickButtonState()



.. index:: GetJoystickHatState

GetJoystickHatState()
---------------------

*Gets the state of a joystick hat*

**Description:**

``GetJoystickHatState(js,1)``    This gets the state of a particular joystick hat.  You need to specify a joystick object, which is created with OpenJoystick().  You also need to specify the hat id.  You can determine how many hats a joystick has with the GetNumJoystickHats() function.  The function returns a value between 0 and 15, which is the sum of values specifying whether each primary NSEW direction is pressed.  The coding is: 0=no buttons; 1=N, 2=E, 4=S, 8=W.  Thus, if 1 is returned, the north hat button is pressed.  If 3 is returned, NorthEast.  If 12 is returned, SW, and so on.

**See Also:**

GetNumJoysticks(), OpenJoystick(), GetNumJoystickAxes() GetNumJoystickBalls(), GetNumJoystickButtons(), GetNumJoystickHats() GetJoystickAxisState(), GetJoystickHatState(), GetJoystickButtonState()



.. index:: GetMouseCursorPosition

GetMouseCursorPosition()
------------------------

**Description:**

Gets the current x,y coordinates of the mouse   pointer.

**Usage:**

.. code-block:: pebl

   GetMouseCursorPosition()

**Example:**

.. code-block:: pebl

   pos <- GetMouseCursorPosition()

**See Also:**

:func:`ShowCursor`, :func:`WaitForMouseButton`,   :func:`SetMouseCursorPosition`, :func:`GetMouseCursorPosition`



.. index:: GetMouseState

GetMouseState()
---------------

*Gets [x,y,b1,b2,b3] list of mouse state, including button states*

**Description:**

Gets the current x,y coordinates of the mouse   pointer, plus the current state of the buttons.  Returns a 5-element list, with the first two indicating x,y position, the third is either 0 or 1 depending on if the left mouse is clicked, the fourth 0 or 2 depending on whether the middle mouse is clicked, and the fifth either 0 or 4 depending on whether the right mouse is clicked.

**Example:**

.. code-block:: pebl

   define Start(p)
   {
    
     win <- MakeWindow()
     i <- 1
     while(i < 100)
     {
       Draw()
       Print(GetMouseState())
   
       Wait(100)
       i <- i + 1
   
     }	
   ##Returns look like:
   [417, 276, 0, 0, 0]
   [495, 286, 0, 0, 0]
   [460, 299, 0, 0, 0]
   [428, 217, 0, 0, 0]
   [446, 202, 0, 0, 4]
   [446, 202, 1, 0, 0]
   [446, 202, 1, 0, 0]
   [446, 202, 0, 2, 0]

**See Also:**

:func:`ShowCursor` :func:`WaitForMouseButton`,   :func:`SetMouseCursorPosition`, :func:`GetMouseCursorPosition`



.. index:: GetNumJoystickAxes

GetNumJoystickAxes()
--------------------

*Counts how many axes on a joystick*

**Description:**

This gets the number of axes on a joystick.  You need to specify a joystick object, which is created with OpenJoystick().

**See Also:**

GetNumJoysticks(), OpenJoystick(), GetNumJoystickAxes() GetNumJoystickBalls(), GetNumJoystickButtons(), GetNumJoystickHats() GetJoystickAxisState(), GetJoystickHatState(), GetJoystickButtonState()



.. index:: GetNumJoystickBalls

GetNumJoystickBalls()
---------------------

*Counts how many balls on a joystick*

**Description:**

This gets the number of joystick balls available on a particular joystick.  You need to specify a joystick object, which is created with OpenJoystick().

**See Also:**

GetNumJoysticks(), OpenJoystick(), GetNumJoystickAxes() GetNumJoystickBalls(), GetNumJoystickButtons(), GetNumJoystickHats() GetJoystickAxisState(), GetJoystickHatState(), GetJoystickButtonState()



.. index:: GetNumJoystickButtons

GetNumJoystickButtons()
-----------------------

**Description:**

This gets the number of joystick buttons available on a particular joystick.  You need to specify a joystick object, which is created with OpenJoystick().

**See Also:**

GetNumJoysticks(), OpenJoystick(), GetNumJoystickAxes() GetNumJoystickBalls(), GetNumJoystickButtons(), GetNumJoystickHats() GetJoystickAxisState(), GetJoystickHatState(), GetJoystickButtonState()



.. index:: GetNumJoystickHats

GetNumJoystickHats()
--------------------

*Counts how many hats on a joystick*

**Description:**

This gets the number of hats available on a particular joystick.  You need to specify a joystick object, which is created with OpenJoystick().

**See Also:**

GetNumJoysticks(), OpenJoystick(), GetNumJoystickAxes() GetNumJoystickBalls(), GetNumJoystickButtons(), GetNumJoystickHats() GetJoystickAxisState(), GetJoystickHatState(), GetJoystickButtonState()



.. index:: GetNumJoysticks

GetNumJoysticks()
-----------------

*Determines how many joysticks are available*

**Description:**

This gets the number of joysticks available on a system. It returns an integer, which if greater than   you can open a joystick using the OpenJoystick() function..

**See Also:**

GetNumJoysticks(), OpenJoystick(), GetNumJoystickAxes() GetNumJoystickBalls(), GetNumJoystickButtons(), GetNumJoystickHats() GetJoystickAxisState(), GetJoystickHatState(), GetJoystickButtonState()



.. index:: GetPEBLVersion

GetPEBLVersion()
----------------

*Returns a string indicating which version of PEBL you are using*

**Description:**

Returns a string describing which version of PEBL you are running.

**Usage:**

.. code-block:: pebl

   GetPEBLVersion()

**Example:**

.. code-block:: pebl

   Print(GetPEBLVersion())

**See Also:**

:func:`TimeStamp()`



.. index:: GetSystemType

GetSystemType()
---------------

*Identifies the type of operating system being used.*

**Description:**

Returns a string identify what type of computer system you are using. It will return either: OSX, LINUX, or WINDOWS.

**Usage:**

.. code-block:: pebl

   GetSystemType()

**Example:**

.. code-block:: pebl

   ## Put this at the beginning of an experiment, 
   ## after a window gWin has been defined.
      if(GetSystemType() == "WINDOWS")
       {
         SignalFatalError("Experiment untested on windows")
       }

**See Also:**

:func:`SystemCall()`



.. index:: GetTextBoxCursorFromClick

GetTextBoxCursorFromClick()
---------------------------

**Description:**

Returns the position (in characters) corresponding to a x,y click on a text box.  The X,Y position must be relative to the x,y position of the box, not absolute.  Once obtained, the cursor position can be set with SetCursorPosition().

**Usage:**

.. code-block:: pebl

   GetTextBoxCursorFromClick(<widget>,<x>,<y>)

**See Also:**

:func:`SetCursorPosition()`, :func:`GetCursorPosition()`, :func:`SetEditable()`, :func:`MakeTextBox()`



.. index:: GetTime

GetTime()
---------

*Gets a number, in milliseconds, representing the time since the PEBL program began running.*

**Description:**

Gets time, in milliseconds, from when PEBL was   initialized.  Do not use as a seed for the RNG, because it will tend   to be about the same on each run. Instead, use ``RandomizeTimer()``.

**Usage:**

.. code-block:: pebl

   GetTime()

**Example:**

.. code-block:: pebl

   a <- GetTime()
   WaitForKeyDown("A")
   b <- GetTime()
   Print("Response time is: " + (b - a))

**See Also:**

:func:`TimeStamp()`



.. index:: GetTimeOfDay

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



.. index:: GetVideoModes

GetVideoModes()
---------------

*Gets list of available screen resolutions*

**Description:**

Gets a list of useable video modes (in width/height pixel pairs), as supplied by the video driver, for a specified screen. Screen is specified as an integer, with 0 being the default screen. If no screen is specified, screen 0 is used.

**Usage:**

.. code-block:: pebl

   modes <- GetVideoModes()

**Example:**

.. code-block:: pebl

   Print(GetVideoModes)
   ##Might return:
   [[1440, 900]
   , [1360, 768]
   , [1152, 864]
   , [1024, 768]
   , [960, 600]
   , [960, 540]
   , [840, 525]
   , [832, 624]
   , [800, 600]
   , [800, 512]
   , [720, 450]
   , [720, 400]
   , [700, 525]
   ]

**See Also:**

:func:`GetCurrentScreenResolution`, :func:`gVideoWidth`, :func:`gVideoHeight`,     :func:`GetDrivers`




.. index:: GetWorkingDirectory

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


.. index:: IsAnyKeyDown

IsAnyKeyDown()
--------------

*Determines whether any key is down.*

**Description:**

.. code-block:: text

   IsAnyKeyDown()


**Usage:**

.. code-block:: pebl

   IsAnyKeyDown()



.. index:: IsAudioOut

IsAudioOut()
------------

**Description:**

Tests whether ``<variant>`` is a AudioOut stream.

**Usage:**

.. code-block:: pebl

   IsAudioOut(<variant>)

**Example:**

.. code-block:: pebl

   if(IsAudioOut(x))
   {
    Play(x)
   }

**See Also:**

:func:`IsColor()`, :func:`IsImage()`,   :func:`IsInteger()`, :func:`IsFileStream()`, :func:`IsFloat()`,   :func:`IsFont()`, :func:`IsLabel()`, :func:`IsList()`,   :func:`IsNumber()`, :func:`IsString()`, :func:`IsTextBox()`,   :func:`IsWidget()`



.. index:: IsCanvas

IsCanvas()
----------

**Description:**

Tests whether ``<variant>`` is a Canvas widget.

**Usage:**

.. code-block:: pebl

   IsCanvas(<variant>)

**Example:**

.. code-block:: pebl

   if(IsCanvas(x)
   {
      SetPixel(x,10,10,MakeColor("red"))
   }

**See Also:**

:func:`IsAudioOut()`, :func:`IsImage()`,   :func:`IsInteger()`, :func:`IsFileStream()`, :func:`IsFloat()`,   :func:`IsFont()`, :func:`IsLabel()`, :func:`IsList()`,   :func:`IsNumber()`, :func:`IsString()`, :func:`IsTextBox()`, :func:`IsText()`   :func:`IsWidget()`, :func:`IsWindow()`



.. index:: IsColor

IsColor()
---------

**Description:**

Tests whether ``<variant>`` is a Color.

**Usage:**

.. code-block:: pebl

   IsColor(<variant>)

**Example:**

.. code-block:: pebl

   if(IsColor(x)
   {
    gWin <- MakeWindow(x)
   }

**See Also:**

:func:`IsAudioOut()`, :func:`IsImage()`,   :func:`IsInteger()`, :func:`IsFileStream()`, :func:`IsFloat()`,   :func:`IsFont()`, :func:`IsLabel()`, :func:`IsList()`,   :func:`IsNumber()`, :func:`IsString()`, :func:`IsTextBox()`,   :func:`IsWidget()`, :func:`IsWindow()`



.. index:: IsCustomObject

IsCustomObject()
----------------

*Tests whether object is a custom object.*

**Description:**

Tests whether ``<variant>`` is a Custom object (created with ``MakeCustomObject``.) Return 1 if so, 0 if not.

**Usage:**

.. code-block:: pebl

   IsCustomObject(<obj>)

**Example:**

.. code-block:: pebl

   if(IsCustomObject(obj)
   {
      MoveObject(obj,x,y)
   } else {
      Move(obj,x,y)
   }

**See Also:**

:func:`IsAudioOut()`, :func:`IsImage()`,   :func:`IsInteger()`, :func:`IsFileStream()`, :func:`IsFloat()`,   :func:`IsFont()`, :func:`IsLabel()`, :func:`IsList()`,   :func:`IsNumber()`, :func:`IsString()`, :func:`IsTextBox()`, :func:`IsText()`   :func:`IsWidget()`, :func:`IsWindow()`



.. index:: IsDirectory

IsDirectory()
-------------

*Checks whether a file is a directory*

**Description:**

Determines whether a named path is a directory.  Returns 1 if it exists and is a directory, and 0 otherwise.

**Usage:**

.. code-block:: pebl

   IsDirectory(<path>)

**Example:**

.. code-block:: pebl

   filename <- "data-"+gSubNum+".csv"
    exists <-  FileExists(filename)
     if(exists)
      {
       out <-    IsDirectory(filename)
       Print(out)
      }

**See Also:**

:func:`GetDirectoryListing()`, :func:`FileExists()`,       :func:`IsDirectory()`,            :func:`MakeDirectory()`



.. index:: IsFileStream

IsFileStream()
--------------

**Description:**

Tests whether ``<variant>`` is a FileStream object.

**Usage:**

.. code-block:: pebl

   IsFileStream(<variant>)

**Example:**

.. code-block:: pebl

   if(IsFileStream(x))
   {
    Print(FileReadWord(x)
   }

**See Also:**

:func:`IsAudioOut()`, :func:`IsColor()`,   :func:`IsImage()`, :func:`IsInteger()`, :func:`IsFloat()`,   :func:`IsFont()`, :func:`IsLabel()`, :func:`IsList()`,   :func:`IsNumber()`, :func:`IsString()`, :func:`IsTextBox()`,   :func:`IsWidget()`



.. index:: IsFloat

IsFloat()
---------

**Description:**

Tests whether ``<variant>`` is a floating-point   value. Note that floating-point can represent integers with great   precision, so that a number appearing as an integer can still be a   float.

**Usage:**

.. code-block:: pebl

   IsFloat(<variant>)

**Example:**

.. code-block:: pebl

   x <- 44
   y <- 23.5
   z <- 6.5
   test <- x + y + z 
   
   IsFloat(x)     	# false
   IsFloat(y)     	# true
   IsFloat(z)     	# true
   IsFloat(test)  	# true

**See Also:**

:func:`IsAudioOut()`, :func:`IsColor()`,   :func:`IsImage()`, :func:`IsInteger()`, :func:`IsFileStream()`,   :func:`IsFont()`, :func:`IsLabel()`, :func:`IsList()`,   :func:`IsNumber()`, :func:`IsString()`, :func:`IsTextBox()`,   :func:`IsWidget()`



.. index:: IsFont

IsFont()
--------

**Description:**

Tests whether ``<variant>`` is a Font object.

**Usage:**

.. code-block:: pebl

   IsFont(<variant>)

**Example:**

.. code-block:: pebl

   if(IsFont(x))
   {
    y <- MakeLabel("stimulus", x)
   }

**See Also:**

:func:`IsAudioOut()`, :func:`IsColor()`,   :func:`IsImage()`, :func:`IsInteger()`, :func:`IsFileStream()`,   :func:`IsFloat()`, :func:`IsLabel()`, :func:`IsList()`,   :func:`IsNumber()`, :func:`IsString()`, :func:`IsTextBox()`,   :func:`IsWidget()`



.. index:: IsImage

IsImage()
---------

**Description:**

Tests whether ``<variant>`` is an Image.

**Usage:**

.. code-block:: pebl

   IsImage(<variant>)

**Example:**

.. code-block:: pebl

   if(IsImage(x))
   {
    AddObject(gWin, x)
   }

**See Also:**

:func:`IsAudioOut()`, :func:`IsColor()`,   :func:`IsInteger()`, :func:`IsFileStream()`, :func:`IsFloat()`,   :func:`IsFont()`, :func:`IsLabel()`, :func:`IsList()`,   :func:`IsNumber()`, :func:`IsString()`, :func:`IsTextBox()`,   :func:`IsWidget()`



.. index:: IsInteger

IsInteger()
-----------

**Description:**

Tests whether ``<variant>`` is an integer type.   Note: a number represented internally as a floating-point type whose   is an integer will return false.  Floating-point numbers can be   converted to internally- represented integers with the   ``ToInteger()`` or ``Round()`` commands.

**Usage:**

.. code-block:: pebl

   IsInteger(<variant>)

**Example:**

.. code-block:: pebl

   x <- 44
   y <- 23.5
   z <- 6.5
   test <- x + y + z 
   	
   IsInteger(x)		# true
   IsInteger(y)		# false
   IsInteger(z)		# false
   IsInteger(test)		# false

**See Also:**

:func:`IsAudioOut()`, :func:`IsColor()`,   :func:`IsImage()`, :func:`IsFileStream()`, :func:`IsFloat()`,   :func:`IsFont()`, :func:`IsLabel()`, :func:`IsList()`,   :func:`IsNumber()`, :func:`IsString()`, :func:`IsTextBox()`,   :func:`IsWidget()`



.. index:: IsKeyDown

IsKeyDown()
-----------

**Description:**

**See Also:**

:func:`IsKeyUp()`



.. index:: IsKeyUp

IsKeyUp()
---------

**Description:**

**See Also:**

:func:`IsKeyDown()`



.. index:: IsLabel

IsLabel()
---------

**Description:**

Tests whether ``<variant>`` is a text Label object.

**Usage:**

.. code-block:: pebl

   IsLabel(<variant>)

**Example:**

.. code-block:: pebl

   if(IsLabel(x)
   {
    text <- GetText(x)
   }

**See Also:**

:func:`IsAudioOut()`, :func:`IsColor()`,   :func:`IsImage()`, :func:`IsInteger()`, :func:`IsFileStream()`,   :func:`IsFloat()`, :func:`IsFont()`, :func:`IsList()`,   :func:`IsNumber()`, :func:`IsString()`, :func:`IsTextBox()`,   :func:`IsWidget()`



.. index:: IsList

IsList()
--------

**Description:**

Tests whether ``<variant>`` is a PEBL list.

**Usage:**

.. code-block:: pebl

   IsList(<variant>)

**Example:**

.. code-block:: pebl

   if(IsList(x))
   {
    loop(item, x)
    {
     Print(item)
    }
   }

**See Also:**

:func:`IsAudioOut()`, :func:`IsColor()`,   :func:`IsImage()`, :func:`IsInteger()`, :func:`IsFileStream()`,   :func:`IsFloat()`, :func:`IsFont()`, :func:`IsLabel()`,   :func:`IsNumber()`, :func:`IsString()`, :func:`IsTextBox()`,   :func:`IsWidget()`



.. index:: IsNumber

IsNumber()
----------

**Description:**

Tests whether ``<variant>`` is a number, either a 		floating-point or an integer.

**Usage:**

.. code-block:: pebl

   IsNumber(<variant>)

**Example:**

.. code-block:: pebl

   if(IsNumber(x))
   {
    Print(Sequence(x, x+10, 1))
   }

**See Also:**

:func:`IsAudioOut()`, :func:`IsColor()`,   :func:`IsImage()`, :func:`IsInteger()`, :func:`IsFileStream()`,   :func:`IsFloat()`, :func:`IsFont()`, :func:`IsLabel()`,   :func:`IsList()`, :func:`IsString()`, :func:`IsTextBox()`,   :func:`IsWidget()`



.. index:: IsShape

IsShape()
---------

**Description:**

Tests whether ``<variant>`` is a drawable   shape, such as a circle, square rectangle, line, bezier curve, or   polygon.

**Usage:**

.. code-block:: pebl

   IsShape(<variant>)

**Example:**

.. code-block:: pebl

   if(IsShape(x))
   {
     Move(x,300,300)
   }

**See Also:**

:func:`Square()`, :func:`Circle()`,   :func:`Rectangle()`, :func:`Line()`, :func:`Bezier()`, :func:`Polygon()`   :func:`IsAudioOut()`, :func:`IsColor()`,   :func:`IsImage()`, :func:`IsInteger()`, :func:`IsFileStream()`,   :func:`IsFloat()`, :func:`IsFont()`, :func:`IsLabel()`,   :func:`IsList()`, :func:`IsNumber()`, :func:`IsString()`,   :func:`IsTextBox()`, :func:`IsWindow()`



.. index:: IsString

IsString()
----------

**Description:**

Tests whether ``<variant>`` is a text string.

**Usage:**

.. code-block:: pebl

   IsString(<variant>)

**Example:**

.. code-block:: pebl

   if(IsString(x))
   {
    tb <- MakeTextBox(x, 100, 100)
   }

**See Also:**

:func:`IsText()`	:func:`IsAudioOut()`, :func:`IsColor()`, :func:`IsImage()`, :func:`IsInteger()`,  		:func:`IsFileStream()`, :func:`IsFloat()`, :func:`IsFont()`, :func:`IsLabel()`, 		:func:`IsList()`, :func:`IsNumber()`, :func:`IsTextBox()`, :func:`IsWidget()`



.. index:: IsText

IsText()
--------

**Description:**

Tests whether ``<variant>`` is a text string.   Same as IsString().

**Usage:**

.. code-block:: pebl

   IsString(<variant>)

**Example:**

.. code-block:: pebl

   if(IsText(x))
   {
    tb <- MakeTextBox(x, 100, 100)
   }

**See Also:**

:func:`IsString()`	:func:`IsAudioOut()`, :func:`IsColor()`, :func:`IsImage()`, :func:`IsInteger()`,  		:func:`IsFileStream()`, :func:`IsFloat()`, :func:`IsFont()`, :func:`IsLabel()`, 		:func:`IsList()`, :func:`IsNumber()`, :func:`IsTextBox()`, :func:`IsWidget()`



.. index:: IsTextBox

IsTextBox()
-----------

**Description:**

Tests whether ``<variant>`` is a TextBox Object

**Usage:**

.. code-block:: pebl

   IsTextBox(<variant>)

**Example:**

.. code-block:: pebl

   if(IsTextBox(x))
   {
    Print(GetText(x))
   }

**See Also:**

:func:`IsAudioOut()`, :func:`IsColor()`,   :func:`IsImage()`, :func:`IsInteger()`, :func:`IsFileStream()`,   :func:`IsFloat()`, :func:`IsFont()`, :func:`IsLabel()`,   :func:`IsList()`, :func:`IsNumber()`, :func:`IsString()`,   :func:`IsWidget()`



.. index:: IsWidget

IsWidget()
----------

**Description:**

Tests whether ``<variant>`` is any kind of a widget object 		(image, label, or textbox).

**Usage:**

.. code-block:: pebl

   IsWidget(<variant>)

**Example:**

.. code-block:: pebl

   if(IsWidget(x))
   {
    Move(x, 200,300)
   }

**See Also:**

:func:`IsAudioOut()`, :func:`IsColor()`,   :func:`IsImage()`, :func:`IsInteger()`, :func:`IsFileStream()`,   :func:`IsFloat()`, :func:`IsFont()`, :func:`IsLabel()`,   :func:`IsList()`, :func:`IsNumber()`, :func:`IsString()`,   :func:`IsTextBox()`



.. index:: IsWindow

IsWindow()
----------

**Description:**

Tests whether ``<variant>`` is a window.

**Usage:**

.. code-block:: pebl

   IsWindow(<variant>)

**Example:**

.. code-block:: pebl

   if(IsWindow(x))
   {
     AddObject(y,x)
   }

**See Also:**

:func:`IsAudioOut()`, :func:`IsColor()`,   :func:`IsImage()`, :func:`IsInteger()`, :func:`IsFileStream()`,   :func:`IsFloat()`, :func:`IsFont()`, :func:`IsLabel()`,   :func:`IsList()`, :func:`IsNumber()`, :func:`IsString()`,   :func:`IsTextBox()`



.. index:: LaunchFile

LaunchFile()
------------

*Launches a file using platform-specific handlers*

**Description:**

Launch a specified file or URI with a platform-specific handler.

**Usage:**

.. code-block:: pebl

   LaunchFile("filename")

**See Also:**

:func:`SystemCall()`



.. index:: MakeDirectory

MakeDirectory()
---------------

*Creates a directory in path*

**Description:**

Creates a directory with a particular name. It will have no effect of the directory already exists.

**Usage:**

.. code-block:: pebl

   FileExists(<path>)

**Example:**

.. code-block:: pebl

   #create data subdirectory + subject-specific directory
    MakeDirectory("data")
    MakeDirectory("data/"+gsubnum)
    filename <- "data/"+gsubnum+"/output.csv"

**See Also:**

:func:`GetDirectoryListing()`, :func:`FileExists()`,       :func:`IsDirectory()`,            :func:`MakeDirectory()`



.. index:: OpenJoystick

OpenJoystick()
--------------

*Gets a joystick object*

**Description:**

This opens an available joystick, as specified by its index.  The returned object can then be used in to access the state of the joystick.  It takes an integer argument, and for the most part, if you have a single joystick attached to your system, you will use OpenJoystick(1).  If you want to use a second joystick, use OpenJoystick(2), and so on.

**See Also:**

GetNumJoysticks(), OpenJoystick(), GetNumJoystickAxes() GetNumJoystickBalls(), GetNumJoystickButtons(), GetNumJoystickHats() GetJoystickAxisState(), GetJoystickHatState(), GetJoystickButtonState()



.. index:: PlayMovie

PlayMovie() 
-----------
***(CURRENTLY NOT WORKING)***
*Plays a movie until its end*

**Description:**


Plays the movie (or other multimedia file) loaded via either the LoadMovie or LoadAudioFile function.  Note that this functionality uses a  different underlying system than the sound playing functions PlayBackground and PlayForeground, and they are not interchangeable.

**Usage:**

.. code-block:: pebl

   PlayMovie(movie)

**Example:**

.. code-block:: pebl

   movie <- LoadMovie("movie.avi",gWin,640,480)
      PrintProperties(movie)
      Move(movie,20,20)
      movie.volume <- .1
      status <- EasyLabel("Demo Movie Player",300,25,gWin,22)    
      Draw()
      PlayMovie(movie)

**See Also:**

:func:`LoadAudioFile()`, :func:`LoadMovie()`, :func:`StartPlayback()`, :func:`PausePlayback()`



.. index:: RegisterEvent

RegisterEvent()
---------------

*Registers events to trigger based on particular conditions*

**Description:**

Adds an event to the event loop.  This function is currently experimental, and its usage may change in future versions of PEBL.

**Usage:**

.. code-block:: pebl

   ## shows a way to generate custom WaitForMouseButton
   RegisterEvent("<MOUSE_BUTTON_PRESS>",1,1,"<EQUAL>","", [])
   out <-   StartEventLoop()
   ClearEventLoop()



**See Also:**

:func:`ClearEventLoop()`, :func:`StartEventLoop()`



.. index:: SetMouseCursorPosition

SetMouseCursorPosition()
------------------------

**Description:**

Sets the current x,y coordinates of the mouse   pointer, 'warping' the mouse to that location immediately

**Usage:**

.. code-block:: pebl

   SetMouseCursorPosition(<x>,<y>)

**Example:**

.. code-block:: pebl

   ##Set mouse to center of screen:
     SetMouseCursorPosition(gVideoWidth/2,
                            gVideoHeight/2)

**See Also:**

:func:`ShowCursor`, :func:`WaitForMouseButton`,   :func:`SetMouseCursorPosition`, :func:`GetMouseCursorPosition`



.. index:: SetWorkingDirectory

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



.. index:: ShowCursor

ShowCursor()
------------

*Hides or show mouse cursor.*

**Description:**

Hides or shows the mouse cursor.  Currently, the   mouse is not used, but on some systems in some configurations, the   mouse cursor shows up.  Calling ``ShowCursor(0)`` will turn off the   cursor, and ``ShowCursor(1)`` will turn it back on.  Be sure to turn it   on at the end of the experiment, or you may actually lose the cursor   for good.

**Usage:**

.. code-block:: pebl

   ShowCursor(<value>)

**Example:**

.. code-block:: pebl

   window <- MakeWindow()
   ShowCursor(0)
   ## Do experiment here
   ##
   
   ## Turn mouse back on.
   ShowCursor(1)



.. index:: SignalFatalError

SignalFatalError()
------------------

*Halts execution, printing out message*

**Description:**

Stops PEBL and prints ``<message>`` to stderr. In addition, when possible, it will pop-up a window with the error message. Useful for type-checking in user-defined functions.  If you want to end an experiment directly, use ``ExitQuietly`` instead.

**Usage:**

.. code-block:: pebl

   SignalFatalError(<message>)

**Example:**

.. code-block:: pebl

   If(not IsList(x))
   {
    SignalFatalError("Tried to frobnicate a List.")
   }
   ##Prints out error message and 
   ##line/filename of function

**See Also:**

:func:`Print()`, ``ExitQuietly()``



.. index:: StartEventLoop

StartEventLoop()
----------------

*Starts the event loop*

**Description:**

Starts the event loop with currently-registered events.  This function is currently experimental, and its usage may change in future versions of PEBL.

**Usage:**

.. code-block:: pebl

   ## shows a way to generate custom WaitForMouseButton
   RegisterEvent("<MOUSE_BUTTON_PRESS>",1,1,"<EQUAL>","", [])
   out <-   StartEventLoop()
   ClearEventLoop()


**See Also:**

:func:`RegisterEvent()`, :func:`ClearEventLoop()`



.. index:: SystemCall

SystemCall()
------------

*Executes command in operating system*

**Description:**

Calls/runs another operating system command.  Can also be used to  launch another PEBL program.  Useful to check GetSystemType() before running.   Note that the output of a    command-line argument is generally not passed back into PEBL; just    the function's return code, which is usually 0 on success or some    other number on failure (depending upon the type of failure).  Some    uses might include:

**Usage:**

.. code-block:: pebl

   SystemCall("text-of-command")
   SystemCall("text-of-command","command-line-options")

**Example:**

.. code-block:: pebl

   if(GetSystemType() == "WINDOWS")
        {
          x <- SystemCall("dir input.txt") 
        } else {
          x <- SystemCall("ls input.txt") 
        }
         if(x <> 0)
         {
            SignalFatalError("Expected file ["+
                  "input.txt] does not exist")
         }

**See Also:**

:func:`GetSystemType()`




.. index:: SystemCallUpdate

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


.. index:: TimeStamp

TimeStamp()
-----------

*Returns a string containing the current date and time*

**Description:**

Returns a string containing the date-and-time,   formatted according to local conventions. Should be used for   documenting the time-of-day and date an experiment was run, but not   for keeping track of timing accuracy.  For that, use   ``GetTime()``.

**Usage:**

.. code-block:: pebl

   TimeStamp()

**Example:**

.. code-block:: pebl

   a <- TimeStamp()
   Print(a)

**See Also:**

:func:`GetTime()`



.. index:: TranslateKeyCode

TranslateKeyCode()
------------------

*Converts a keycode to a key name*

**Description:**

Translates a code corresponding to a keyboard key   into a keyboard value.  This code is returned by some event/device   polling functions.



.. index:: TranslateString

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



.. index:: VariableExists

VariableExists()
----------------

**Description:**

Tests whether a variable exists.

**Usage:**

.. code-block:: pebl

   Uppercase("variablename")

**Example:**

.. code-block:: pebl

   if(not VariableExists("underwear"))
        {
          underwear <- "Under there"
        }

**See Also:**

:func:`PropertyExists()`



.. index:: Wait

Wait()
------

**Description:**

Waits the specified number of milliseconds, then returns.

**Usage:**

.. code-block:: pebl

   Wait(<time>)

**Example:**

.. code-block:: pebl

   Wait(100)
   Wait(15)



.. index:: WaitForAllKeysUp

WaitForAllKeysUp()
------------------

*Waits until all keys are in up state*

**Description:**

Wait until all keyboard keys are in the up                position. This includes numlock, capslock, etc.



.. index:: WaitForKeyDown

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



.. index:: WaitForAnyKeyDown

WaitForAnyKeyDown()
-------------------

*Waits until any key is detected in down state*

**Description:**

Waits for any key to be detected in the down position.              This includes numlock, capslock, etc, which can be locked              in the down position even if they are not being held              down.  Will return immediately if a key is being held              down before the function is called.

**See Also:**

:func:`WaitForAnyKeyPress()`



.. index:: WaitForAnyKeyDownWithTimeout

WaitForAnyKeyDownWithTimeout()
------------------------------

**Description:**

Waits until any key is detected in the down position, but will return   after a specified number of milliseconds.  This tests for the key position on each cycle; users should prefer using WaitForAnyKeyPressWithTimout() which waits for the keypress event.

**Usage:**

.. code-block:: pebl

   WaitForAnyKeyDownWithTimeout(<time>)

**See Also:**

:func:`WaitForAnyKeyPressWithTimeout()`, :func:`WaitListKeyPressWithTimeout()`,  :func:`WaitForAnyKeyPress()`, :func:`WaitListKeyPress()`



.. index:: WaitForAnyKeyPress

WaitForAnyKeyPress()
--------------------

*Waits until any key is pressed*

**Description:**

Waits until any key is pressed, and returns the key pressed. This waits for the keyboard event, which is typically more reliable and less computationally taxing than waiting for the keyboard state (which updates based on those events anyway).

**Usage:**

.. code-block:: pebl

   WaitForKeyPress(<time>)

**Example:**

.. code-block:: pebl

   cont <- 1
   	   while(cont)
   	    {
            key <- WaitForAnyKEyPress()
            if(key == "x")
             {
             	 cont <- 0
             }
   	    }

**See Also:**

:func:`WaitForAnyKeyPressWithTimeout()`, :func:`WaitListKeyPressWithTimeout()`,   :func:`WaitListKeyPress()`



.. index:: WaitForAnyKeyPressWithTimeout

WaitForAnyKeyPressWithTimeout()
-------------------------------

**Description:**

Waits until any key is detected in the down position, but will return 	after a specified number of milliseconds.  This tests for the key position on each cycle; users should prefer using WaitForAnyKeyPressWithTimout() which waits for the keypress event.

**Usage:**

.. code-block:: pebl

   WaitForAnyKeyDownWithTimeout(<time>)

**See Also:**

:func:`WaitForAnyKeyPressWithTimeout()`, :func:`WaitListKeyPressWithTimeout()`



.. index:: WaitForKeyListDown

WaitForKeyListDown()
--------------------

*Waits until one of the keys is in down state*

**Description:**

Returns when any one of the keys specified in the   argument is down. If a key is down when called, it will return immediately.

**Usage:**

.. code-block:: pebl

   WaitForKeyListDown(<list-of-keys>)

**Example:**

.. code-block:: pebl

   WaitForKeyListDown(["a","z"])



.. index:: WaitForKeyPress

WaitForKeyPress()
-----------------

**Description:**

Waits for a keypress event that matches the   specified key.  Usage of this function is preferred over   ``WaitForKeyDown()``, which tests the state of the key. Returns the   value of the key pressed.

**Usage:**

.. code-block:: pebl

   WaitForKeyPress(<key>)

**See Also:**

:func:`WaitForAnyKeyPress()`, :func:`WaitForKeyRelease()`, :func:`WaitForListKeyPress()`



.. index:: WaitForKeyUp

WaitForKeyUp()
--------------

**Description:**

.. index:: WaitForKeyRelease

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



.. index:: WaitForListKeyPress

WaitForListKeyPress()
---------------------

**Description:**

Returns when any one of the keys specified in the   argument is pressed. Will only return on a new keyboard event, and   so a previously pressed key will not trip this function, unlike   ``WaitForKeyListDown()``  Returns a string indicating the value   of the keypress.

**Usage:**

.. code-block:: pebl

   WaitForListKeyPress(<list-of-keys>)

**Example:**

.. code-block:: pebl

   WaitForListKeyPress(["a","z"])

**See Also:**

:func:`WaitForKeyListDown`, :func:`WaitForListKeyPressWithTimeout`



.. index:: WaitForListKeyPressWithTimeout

WaitForListKeyPressWithTimeout()
--------------------------------

**Description:**

Returns when any one of the keys specified in the   argument is pressed, or when the timeout has elapsed; whichever   comes first. Will only return on a new keyboard/timeout events, and   so a previously pressed key will not trip this function, unlike   ``WaitForKeyListDown()``.  The optional ``<style>`` parameter is currently   unused, but may be deployed in the future for differences in how   or when things should be returned.  Returns the value of the pressed   key.  If the function terminates by exceeding the ``<timeout>``,   it will return the string ``"<timeout>"``.  Note: previous to 2.0, returned a list ["<timeout>"], which may mean updating logic for tests designed in the 0.x series.

**Usage:**

.. code-block:: pebl

   WaitForListKeyPressWithTimeout(<list-of-keys>,
                                   <timeout>,opt:<style>)

**Example:**

.. code-block:: pebl

   x <- WaitForListKeyPressWithTimeout(["a","z"],
                                          2000)
     if(IsList(x))
     {
        Print("Did Not Respond.")
     }

**See Also:**

:func:`WaitForKeyListDown`, :func:`WaitForListKeyPress`, :func:`WaitForKeyPressWithTimeout`



.. index:: WaitForMouseButton

WaitForMouseButton()
--------------------

*Waits until any of the mouse buttons is pressed or released, and returns message indicating what happened*

**Description:**

Waits for a mouse click event to occur.   This takes no arguments, and returns a 4-tuple list, indicating:  

.. code-block:: text

   [xpos,   ypos,   button id [1-3],   "<pressed>" or "<released>"]


**Usage:**

.. code-block:: pebl

   WaitForMouseButton()

**Example:**

.. code-block:: pebl

   ## Here is how to wait for a mouse down-click
   
    continue <- 1
    while(continue)
    {
        x <- WaitForMouseButton()
        if(Nth(x,4)=="<pressed>")
         {
             continue <- 0
         }
    }
    Print("Clicked")

**See Also:**

:func:`ShowCursor`, :func:`WaitForMouseButtonWithTimeout`   :func:`SetMouseCursorPosition`, :func:`GetMouseCursorPosition`



.. index:: WaitForMouseButtonWithTimeout

WaitForMouseButtonWithTimeout()
-------------------------------

**Description:**

Waits for a mouse click event to occur, or a   timeout to be reached.   This takes a single argument: timeout delay in ms. When clicked, it returns a   4-tuple list, indicating: 

.. code-block:: text

   [xpos,  ypos,   button id [1-3],   "<pressed>" or "<released>"]

   when not click and timeout is reached, it returns a list:    ``[timeout]``

**Usage:**

.. code-block:: pebl

   WaitForMouseButtonWithTimeOut(10)

**Example:**

.. code-block:: pebl

   ## Here is how to wait for a mouse down-click
   
    continue <- 1
    while(continue)
    {
        x <- WaitForMouseButtonWithTimeout(500)
        if(First(x)=="<timeout>")
         {
            Print("time is "+GetTime())
             continue <- 1
         } else {
             continue <- 0
         }
    }
    Print("Clicked")

**See Also:**

:func:`ShowCursor`,   :func:`SetMouseCursorPosition`, :func:`GetMouseCursorPosition`
