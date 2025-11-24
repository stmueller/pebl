
================================================================================
PEBLObjects - Graphics and Objects
================================================================================

This module contains functions for creating and manipulating graphical objects, windows, and visual elements.

.. contents:: Function Index
   :local:
   :depth: 0


.. index:: AddObject

AddObject()
-----------

*Adds an object to a parent object (window)*

**Description:**

Adds a widget to a parent window, at the top of the object stack.  Once added, the object will be drawn onto the parent last, meaning it will be on top of anything previously added.   In general, objects can be added to other objects as well as windows.  For example, you can add drawing objects (circles, etc.) to an image to annotate the image and maintain its proper x,y coordinates.  Also, if you 're-add' an object that is already on a widget, it will get automatically removed from the window first.  This is an easy way to reorder elements on a screen.  

.. code-block:: text

   AddObject(<obj>, <window>) AddObject(<obj>, <canvas>) AddObject(<obj>, <widget>)


**Example:**

.. code-block:: pebl

   define Start(p)
   {
    win <- MakeWindow()
    img <- MakeImage("pebl.png")
    circ <- Circle(20,20,10,MakeColor("red"),1)
    AddObject(circ,img)
    AddObject(img,win)
    Move(img,100,100)
    Draw()
    WaitForAnyKeyPress()
   }

**See Also:**

:func:`RemoveObject()`


.. index:: Bezier

Bezier()
--------

*Creates bezier curve centered at x,y with relative points*

**Description:**

Creates a smoothed line through the  points specified by ``<xpoints>``, ``<ypoints>``. The lists ``<xpoints>`` and ``<ypoints>`` are adjusted by  ``<x>`` and ``<y>``, so they should be relative to 0, not the location you want the points to be at.  Like other drawn objects, the bezier must then be added to the window to appear. <steps> denotes how smooth the approximation will be.

**Usage:**

.. code-block:: pebl

   Bezier(<x>,<y>,<xpoints>,<ypoints>,
            <steps>,<color>)

**Example:**

.. code-block:: pebl

   win <- MakeWindow()
      #This makes a T
      xpoints <- [-10,10,10,20,20,-20,-20,-10]
      ypoints <- [-20,-20,40,40,50,50,40,40]
     p1 <-    Bezier(100,100,xpoints, ypoints,
              5, MakeColor("black"))
     AddObject(p1,win)
     Draw()

**See Also:**

:func:`BlockE()`, :func:`Polygon()`, :func:`MakeStarPoints()`, :func:`MakeNGonPoints()`


.. index:: Circle

Circle()
--------

*Creates circle with radius r centered at position x,y*

**Description:**

Creates a circle for graphing at x,y with radius r.   Circles must be added to a parent widget before it can be drawn; it   may be added to widgets other than a base window. The properties of   circles may be changed by accessing their properties directly,   including the FILLED property which makes the object an outline   versus a filled shape.

**Usage:**

.. code-block:: pebl

   Circle(<x>, <y>, <r>,<color>)

**Example:**

.. code-block:: pebl

   c <- Circle(30,30,20, MakeColor(green))
     AddObject(c, win)
     Draw()

**See Also:**

:func:`Square()`, :func:`Ellipse()`, :func:`Rectangle()`, :func:`Line()`


.. index:: Draw

Draw()
------

*Redraws a widget and its children*

**Description:**

Redraws the screen or a specific widget.

**Usage:**

.. code-block:: pebl

   Draw()
   Draw(<object>)

**See Also:**

:func:`DrawFor()`, :func:`Show()`, :func:`Hide()`


.. index:: DrawFor

DrawFor()
---------

**Description:**

Draws a screen or widget, returning after   ``<cycles>`` refreshes. This function currently does not work as   intended in the SDL implementation, because of a lack of control   over the refresh blank.  It may work in the future.

**Usage:**

.. code-block:: pebl

   DrawFor( <object>, <cycles>)

**See Also:**

:func:`Draw()`, :func:`Show()`, :func:`Hide()`


.. index:: Ellipse

Ellipse()
---------

*Creates ellipse with radii rx and ry centered at position x,y*

**Description:**

Creates a ellipse for graphing at x,y with radii   rx and ry. Ellipses are only currently definable oriented in   horizontal/vertical directions.  Ellipses  must be added   to a parent widget before it can be drawn; it may be added to   widgets other than a base window.  The properties of ellipses may be   changed by accessing their properties directly, including the FILLED   property which makes the object an outline versus a filled shape.

**Usage:**

.. code-block:: pebl

   Ellipse(<x>, <y>, <rx>, <ry>,<color>)

**Example:**

.. code-block:: pebl

   e <- Ellipse(30,30,20,10, MakeColor(green))
     AddObject(e, win)
     Draw()

**See Also:**

:func:`Square()`, :func:`Circle()`, :func:`Rectangle()`, :func:`Line()`


.. index:: GetCursorPosition

GetCursorPosition()
-------------------

**Description:**

Returns an integer specifying where in a textbox the edit cursor is.  The value indicates which character it is on.

**Usage:**

.. code-block:: pebl

   GetCursorPosition(<textbox>)

**See Also:**

:func:`SetCursorPosition()`, :func:`MakeTextBox()`, :func:`SetText()`


.. index:: GetLineBreaks

GetLineBreaks()
---------------

**Description:**

This gets linebreaks for a textbox.  It is mainly used internally for 	text rendering/layout, but could be useful in other contexts.

**Example:**

.. code-block:: pebl

   gWin <- MakeWindow()
   	obj <- EasyTextbox("test a b c
   	 d e f
   	 g h i j k
   	 l m n o p q r 
   	 s t u v",30,30,gWin,22, 40,200)
   	
   	breaks <- GetLineBreaks(obj)
   	Print("Number of lines:" + Length(breaks))


.. index:: GetParent

GetParent()
-----------

**Description:**

This gets parent of a widget.

**Example:**

.. code-block:: pebl

   gWin <- MakeWindow()
      obj <- EasyLabel("test",30,30,gWin,22)
      
      ## later
      
      win <- GetParent(obj) ##should be gWin


.. index:: GetPixelColor

GetPixelColor()
---------------

*Gets the color of a specified pixel on a widget*

**Description:**

Gets a color object specifying the color of a particular pixel on a widget.

**Usage:**

.. code-block:: pebl

   color <- GetPixelColor(widget,x,y)

**Example:**

.. code-block:: pebl

   ##Judge brightness of a pixel
     img <- MakeImage("test.png")
     col <- GetPixelColor(img,20,20)
     hsv <- RGBtoHSV(col)
     Print(Third(hsv))

**See Also:**

:func:`SetPixel()`


.. index:: GetProperty

GetProperty()
-------------

*Returns value of property*

**Description:**

Gets a particular named property of an object. This works for custom or built-in objects.  If the property does not exist, a fatal error will be signaled, and so you should check using PropertyExists() if there is any chance the property does not exist.

**Example:**

.. code-block:: pebl

   obj <- MakeCustomObject("myobject")
   obj.taste <- "buttery"
   obj.texture <- "creamy"
   SetProperty(obj,"flavor","tasty")
     
   list <- GetPropertyList(obj)
   loop(i,list)
      {
        if(PropertyExists(obj,i)
         {
           Print(i  + ":  " + GetProperty(obj,i))
         }
      }

**See Also:**

:func:`GetPropertyList()`, :func:`PropertyExists()`, :func:`SetProperty()`, :func:`MakeCustomObject()`, :func:`PrintProperties()`


.. index:: GetPropertyList

GetPropertyList()
-----------------

*Gets a list of all the property names of an object*

**Description:**

Gets a list of all of the properties an object has.  This works for custom or built-in objects.

**Example:**

.. code-block:: pebl

   obj <- MakeCustomObject("myobject")
     obj.taste <- "buttery"
     obj.texture <- "creamy"
     SetProperty(obj,"flavor","tasty")
     
     list <- GetPropertyList(obj)
     loop(i,list)
      {
        if(PropertyExists(obj,i)
         {
           Print(i  + ":  " + GetProperty(obj,i))
         }
      }

**See Also:**

:func:`GetProperty`, :func:`PropertyExists`, :func:`SetProperty` :func:`MakeCustomObject`, :func:`PrintProperties`


.. index:: GetSize

GetSize()
---------

**Description:**

Returns a list of ``[height, width]``,   specifying the size of the widget.   The .width and .height properties can also be used instead of this function

**Usage:**

.. code-block:: pebl

   GetSize(<widget>)

**Example:**

.. code-block:: pebl

   image <- MakeImage("stim1.bmp")
   xy <- GetSize(image)
   x <- Nth(xy, 1)
   y <- Nth(xy, 2)


.. index:: GetText

GetText()
---------

*Returns the text in a textbox or label*

**Description:**

Returns the text stored in a text object  		(either a textbox or a label).  The .text properties can also   be used instead of this function.

**Usage:**

.. code-block:: pebl

   GetText(<widget>)

**See Also:**

:func:`SetCursorPosition()`, :func:`GetCursorPosition()`, :func:`SetEditable()`, :func:`MakeTextBox()`


.. index:: GetVocalResponseTime

GetVocalResponseTime()
----------------------

*A simple voice key*

**Description:**

This is a simple audio amplitude voice key controlled by two parameters  *ONLY AVAILABLE ON WINDOWS AND LINUX*.

**Usage:**

.. code-block:: pebl

   GetVocalResponseTime(buffer, 
                        timethreshold,
                        energythreshold)

**Example:**

.. code-block:: pebl

   buffer <- MakeAudioInputBuffer(5000)
     resp0 <-  GetVocalResponseTime(buffer,.35, 200)
     SaveAudioToWaveFile("output.wav",buffer)

**See Also:**

:func:`MakeAudioInputBuffer()`, :func:`SaveAudioToWaveFile()`,


.. index:: Hide

Hide()
------

*Hides an object*

**Description:**

Makes an object invisible, so it will not be drawn.

**Usage:**

.. code-block:: pebl

   Hide(<object>)

**Example:**

.. code-block:: pebl

   window <- MakeWindow()
   image1  <- MakeImage("pebl.bmp")
   image2  <- MakeImage("pebl.bmp")
   AddObject(image1, window)
   AddObject(image2, window)
   Hide(image1)
   Hide(image2)
   Draw()		# empty screen will be drawn.
   	
   Wait(3000)
   Show(image2)
   Draw()		# image2 will appear.
   
   Hide(image2)
   Draw()		# image2 will disappear.
   
   Wait(1000)
   Show(image1)
   Draw()		# image1 will appear.

**See Also:**

:func:`Show()`


.. index:: Line

Line()
------

*Creates line starting at x,y and ending at x+dx, y+dy*

**Description:**

Creates a line for graphing at x,y ending at x+dx,   y+dy.  dx and dy describe the size of the line.  Lines must be added   to a parent widget before it can be drawn; it may be added to   widgets other than a base window. Properties of lines may be   accessed and set later.

**Usage:**

.. code-block:: pebl

   Line(<x>, <y>, <dx>, <dy>, <color>)

**Example:**

.. code-block:: pebl

   l <- Line(30,30,20,20, MakeColor("green")
     AddObject(l, win)
     Draw()

**See Also:**

:func:`Square()`, :func:`Ellipse()`, :func:`Rectangle()`, :func:`Circle()`


.. index:: LoadAudioFile

LoadAudioFile()
---------------

*Load an audio file*

**Description:**

Loads an audio file supported by  the ffmpeg library.  It is nearly identical to LoadMovie(), but only works for audio files (.ogg, .mp3, .wav, .aiff, .wma, et.).  It creates a movie object, which can then be played using PlayMovie() or StartPlayback() functions.  Currently, only supported on Windows and Linux.  The ffmpeg (``http://ffmpeg.org``) library supports a wide range of audio formats, including most .wav, .mp3, .ogg, .flac, .aiff, .wma, and others.   Currently, there appears to sometimes be playback problems if the audio stream is not stereo, so be sure to convert your audio to stereo. Also, there appears to be some problems with .flac data formats.  If you have problems with playback,  you should verify that your media file loads with another ffmpeg media player.

**Usage:**

.. code-block:: pebl

   LoadAudioFile(audiofile)

**Example:**

.. code-block:: pebl

   movie <- LoadAudioFile("instuctions.mp3")
      PrintProperties(inst)
      PlayMovie(inst)
      PausePlayback(insnt)

**See Also:**

:func:`LoadMovie()`, :func:`PlayMovie()`, :func:`StartPlayback()`, :func:`PausePlayback()`


.. index:: LoadMovie

LoadMovie()
-----------

*Load a movie file*

**Description:**

DOES NOT WORK IN PEBL 2.0+    Loads a movie file using the ffmpeg library.  It creates a movie object, which can then be played using PlayMovie() or StartPlayback() functions.  Currently, only supported on Windows and Linux.  The ffmpeg (``http://ffmpeg.org``) library supports a wide range of video and audio formats, including most .mpg, .avi, .ogg and .mp3 type formats.  Audio-only formats should load and play with LoadMovie, but another function, LoadAudioFile(), has been created for these, as they do not need to be added to a window to work.  If you have problems with playback,  you should verify that your media file loads with another ffmpeg media player.  For technical reasons, a movie MUST be loaded directly onto a window, and not another widget.

**Usage:**

.. code-block:: pebl

   LoadMovie(movie,window, width, height)

**Example:**

.. code-block:: pebl

   movie <- LoadMovie("movie.avi",gWin,640,480)
      PrintProperties(movie)
      Move(movie,20,20)
      Draw() 
      StartPlayback(movie)
      Wait(500) #Play 500 ms of the movie.
      PausePlayback(movie)

**See Also:**

:func:`LoadAudioFile()`, :func:`LoadMovie()`, :func:`PlayMovie()`, :func:`StartPlayback()`, :func:`PausePlayback()`


.. index:: LoadSound

LoadSound()
-----------

*Loads a soundfile from the filename, returning a variable that can be played*

**Description:**

Loads a soundfile from ``<filename>``,  returning a variable that can be played using the PlayForeground or PlayBackground functions.  ``LoadSound`` As of PEBL version 2.1, LoadSound will load raw and compressed audio files of various sorts.  This includes uncompressed .wav files, .mp3, .ogg, .flac, and .midi files. This is based on the sdl2\_mixer library, and so more details about the file formats accepted can be found by examining that library.  Examples of using LoadSound are found in ``demo tests testaudio.pbl``  When the file gets loaded, it gets automatically transcoded into a stereo 44100-sampling rate audio stream, regardless of its original playback rate.  We have reports that in some cases, this can cause some problems, especially if a mono file gets loaded multiple times in an experiment. If you experience playback problems, try converting your audio to  stereo 44100 hz and see if it helps.

**Usage:**

.. code-block:: pebl

   LoadSound(<filename>)

**Example:**

.. code-block:: pebl

   woof   <- LoadSound("dog.wav")
     PlayBackground(woof)
     Wait(200)
     Stop(woof)
     PlayForeground(woof)

**See Also:**

:func:`PlayForeground`, :func:`PlayBackground`, :func:`LoadAudioFile`, :func:`LoadMovie`


.. index:: MakeAudioInputBuffer

MakeAudioInputBuffer()
----------------------

*Creates a buffer to record audio input*

**Description:**

Creates a sound buffer to use for audio recording or voicekey sound input.  It is currently very simple, allowing only to set the duration.  By default, it record mono at 44100 hz.

**Usage:**

.. code-block:: pebl

   MakeAudioInputBuffer(<time-in-ms>)

**Example:**

.. code-block:: pebl

   buffer <- MakeAudioInputBuffer(5000)
     resp0 <-  GetVocalResponseTime(buffer,.35, 200)
     SaveAudioToWaveFile("output.wav",buffer)

**See Also:**

:func:`GetVocalResponseTime()`, :func:`SaveAudioToWaveFile()`,


.. index:: MakeCanvas

MakeCanvas()
------------

*Creates a blank canvas*

**Description:**

Makes a canvas object  ``<x>`` pixels by   ``y`` pixels, in color ``<color>``.  A canvas is an object that other objects can be attached to, and imprinted upon. When the canvas gets moved, the attached objects move as well. The background of a canvas can be made invisible by using a color with alpha channel == 0. The Setpixel and SetPoint functions let you change individual pixels on a canvas, to enable adding noise, drawing functional images, etc. A canvas gets 'cleared' by calling ResetCanvas(canvas). Any object added to a canvas creates an 'imprint' on the canvas that remains if the object is moved.  This allows you to use another image as a paintbrush on the canvas, and lets you to add noise to text. Because a text label gets re-rendered when its drawn, if you want to add pixel noise to a stimulus, you can create a label, add it to a canvas, then add pixel noise to the canvas.

**Usage:**

.. code-block:: pebl

   MakeCanvas(<x>, <y>, <color>)

**Example:**

.. code-block:: pebl

   gWin <- MakeWindow()
     clear <- MakeColor("white")
     clear.alpha <- 0
     #make a transparent canvas:
     x <- MakeCanvas(300,300,clear)  
     AddObject(x,gWin)
     Move(x,300,300)
     img <- MakeImage("pebl.png")
     AddObject(img,x)
     Move(img,100,100)
     Draw(x)          #imprint the image on the canvas
     Move(img,100,200)
     Draw(x)          #imprint the image on the canvas
     Hide(img)
   
     #draw a line on the canvas
      i <- 10
      red <- MakeColor("red")
     while(i < 200)
      {
        SetPixel(x,20,i,red)
        i <- i + 1
      }
     Draw()
     WaitForAnyKeyPress()

**See Also:**

:func:`MakeImage()`, :func:`SetPixel()`,   :func:`MakeGabor()`, :func:`ResetCanvas()`


.. index:: MakeColor

MakeColor()
-----------

*Creates a color based on a color name*

**Description:**

Makes a color from ``<colorname>`` such as   ``red'', ``green'', and nearly 800 others.  Color names and   corresponding RGB values can be found in ``doc/colors.txt``.

**Usage:**

.. code-block:: pebl

   MakeColor(<colorname>)

**Example:**

.. code-block:: pebl

   green <- MakeColor("green")
    black <- MakeColor("black")

**See Also:**

:func:`MakeColorRGB()`, :func:`RGBtoHSV()`


.. index:: MakeColorRGB

MakeColorRGB()
--------------

*Creates a color based on red, green, and blue values*

**Description:**

Makes an RGB color by specifying ``<red>``,   ``<green>``, and ``<blue>`` values (between 0 and 255).

**Usage:**

.. code-block:: pebl

   MakeColorRGB(<red>, <green>, <blue>)

**See Also:**

:func:`MakeColor()`, :func:`RGBtoHSV()`


.. index:: MakeCustomObject

MakeCustomObject()
------------------

*Creates custom object.*

**Description:**

Creates a 'custom' object that can encapsulate multiple properties. It takes a name as an argument, but this is currently not accessible.

**Example:**

.. code-block:: pebl

   obj <- MakeCustomObject("myobject")
     obj.taste <- "buttery"
     obj.texture <- "creamy"
     SetProperty(obj,"flavor","tasty")
     
     list <- GetPropertyList(obj)
     loop(i,list)
      {
        if(PropertyExists(obj,i)
         {
           Print(i  + ":  " + GetProperty(obj,i))
         }
      }

**See Also:**

:func:`GetPropertyList()`, :func:`PropertyExists()`, :func:`SetProperty()`, :func:`IsCustomObject()`, :func:`PrintProperties()`, :func:`GetProperty()`


.. index:: MakeFont

MakeFont()
----------

*Creates a font which can be used to make labels*

**Description:**

Makes a font.  The first argument must be a text   name of a font.  The font can reside anywhere in PEBL's search path,   which would primarily include the media/fonts directory, and the   working directory (where the script is saved).   

- style changes from normal to bold/underline, italic. 0=normal, 1=underline, 2=italic,3=bolditalic
- fgcolor and bgcolor need to be colors, not just names of colors
- if show-backing is 0, the font gets rendered with an invisible
- background; otherwise with a bgcolor background. (Note: previous to PEBL 0.11, the final argument = 0 rendered the font  with non anti-aliased background, which I can see almost no use for.)


**Usage:**

.. code-block:: pebl

   MakeFont(<ttf_filename>, <style>, <size>, 
            <fgcolor>, <bgcolor>, <show-backing>)

**Example:**

.. code-block:: pebl

   font <- MakeFont("Vera.ttf",0,22,MakeColor("black"),
                       MakeColor("white"),1)


.. index:: MakeImage

MakeImage()
-----------

*Creates an image by reading in an image file (jpg, gif, png, bmp, etc.)*

**Description:**

Makes an image widget from an image file. 		``.bmp`` formats should be supported; others may be as well.

**Usage:**

.. code-block:: pebl

   MakeImage(<filename>)


.. index:: MakeLabel

MakeLabel()
-----------

**Description:**

Makes a text label for display on-screen. Text will   be on a single line, and the ``Move()`` command centers   ``<text>`` on the specified point.

**Usage:**

.. code-block:: pebl

   MakeLabel(<text>, <font>)


.. index:: MakeSineWave

MakeSineWave()
--------------

*Creates a pure sine wave.*

**Description:**

Creates a sine wave that can be played using the Play() or PlayBackground() functions.  It will create a single-channel sound at 44100 bitrate, 16 bit precision.

**Usage:**

.. code-block:: pebl

   MakeSineWave(<duration_in_ms>, <hz>, <amplitude>)

**Example:**

.. code-block:: pebl

   ##Make a sound that is 1000 ms, but just play 300 ms
      sound  <- MakeSineWave(200, 220, 1000)
      PlayBackground(sound)
      Wait(300)
      Stop(sound)

**See Also:**

:func:`PlayForeground()`, :func:`PlayBackGround()`, :func:`Stop()`


.. index:: MakeTextBox

MakeTextBox()
-------------

*Creates a sized box filled*

**Description:**

Creates a textbox in which to display text.  		Textboxes allow multiple lines of text to be rendered; 		automatically breaking the text into lines.

**Usage:**

.. code-block:: pebl

   MakeTextbox(<text>,<font>,<width>,<height>)

**Example:**

.. code-block:: pebl

   font <-MakeFont("Vera.ttf", 1, 12, MakeColor("red"), 
   MakeColor("green"), 1)
   tb <- MakeTextBox("This is the text in the textbox", 
   font, 100, 250)

**See Also:**

:func:`MakeLabel()`, :func:`GetText()`, :func:`SetText()`, :func:`SetCursorPosition()`, 		:func:`GetCursorPosition()`, :func:`SetEditable()`


.. index:: MakeWindow

MakeWindow()
------------

*Creates main window, in color named by argument, or grey if no argument is named*

**Description:**

Creates a window to display things in. 		Background is specified by ``<color>``.

**Usage:**

.. code-block:: pebl

   MakeWindow(opt:<color>, opt:<width>,opt:<height>)

**Example:**

.. code-block:: pebl

   win <- MakeWindow()
     gWin <- MakeWindow("white")
   
     ##make a second window for debugging or experimenter data entry.
     gWin2 <- MakeWindow("black",400,200)


.. index:: Move

Move()
------

**Description:**

Moves an object to a specified location.   		Images and Labels are moved according to their center;  		TextBoxes are moved according to their upper left corner.

**Usage:**

.. code-block:: pebl

   Move(<object>, <x>, <y>)

**Example:**

.. code-block:: pebl

   Move(label, 33, 100)

**See Also:**

:func:`MoveCorner()`, :func:`MoveCenter()`, :func:`.X` and :func:`.Y` properties.


.. index:: PausePlayback

PausePlayback()
---------------

*Pauses playback of movie*

**Description:**

Pauses a playing movie or audio stream.  This is used for  movies whose playback was initiated using ``StartPlayback``, which then ran as background threads during a Wait() function.

**Usage:**

.. code-block:: pebl

   PausePlayBack(movie)

**Example:**

.. code-block:: pebl

   movie <- LoadMovie("movie.avi",gWin,640,480)
      PrintProperties(movie)
      Move(movie,20,20)
      Draw() 
      StartPlayback(movie)
      Wait(500) #Play 500 ms of the movie.
      PausePlayback(movie)
      Wait(500)

**See Also:**

:func:`LoadAudioFile()`, :func:`LoadMovie()`, :func:`PlayMovie()`, :func:`StartPlayback()`


.. index:: PlayBackground

PlayBackground()
----------------

*Plays the sound 'in the background', returning immediately*

**Description:**

Plays the sound 'in the background', returning immediately.

**Usage:**

.. code-block:: pebl

   PlayBackground(<sound>)

**Example:**

.. code-block:: pebl

   sound  <- MakeSineWave(200, 220, 1000)
      PlayBackground(sound)
      Wait(300)
      Stop(sound)

**See Also:**

:func:`PlayForeground()`, :func:`Stop()`


.. index:: PlayForeground

PlayForeground()
----------------

*Plays the sound 'in the foreground', not returning until the sound is complete*

**Description:**

Plays the sound 'in the foreground';  		does not return until the sound is complete.

**Usage:**

.. code-block:: pebl

   PlayForeground(<sound>)

**Example:**

.. code-block:: pebl

   sound  <- MakeSineWave(200, 220, 1000)
      PlayForeground(sound)

**See Also:**

:func:`PlayBackground()`, :func:`Stop()`


.. index:: Polygon

Polygon()
---------

**Description:**

Creates a polygon in the shape of the points specified by ``<xpoints>``, ``<ypoints>``. The lists ``<xpoints>`` and ``<ypoints>`` are adjusted by  ``<x>`` and ``<y>``, so they should be relative to 0, not the location you want the points to be at.  Like other drawn objects, the polygon must then be added to the window to appear.

**Usage:**

.. code-block:: pebl

   Polygon(<x>,<y>,<xpoints>,<ypoints>,
             <color>,<filled>)

**Example:**

.. code-block:: pebl

   win <- MakeWindow()
      #This makes a T
      xpoints <- [-10,10,10,20,20,-20,-20,-10]
      ypoints <- [-20,-20,40,40,50,50,40,40]
     p1 <-    Polygon(100,100,xpoints, ypoints,
                      MakeColor("black"),1)
     AddObject(p1,win)
     Draw()

**See Also:**

:func:`BlockE()`, :func:`Bezier()`, :func:`MakeStarPoints()`, :func:`MakeNGonPoints()`


.. index:: PrintProperties

PrintProperties()
-----------------

*Prints a list of all available properties of an object (for debugging)*

**Description:**

Prints .properties/values for any complex object.   These include textboxes, fonts, colors, images, shapes, etc. Mostly   useful as a debugging tool.

**Usage:**

.. code-block:: pebl

   PrintProperties(<object>)

**Example:**

.. code-block:: pebl

   win <- MakeWindow()
      tb <- EasyTextbox("one",20,20,win,22,400,80)
      PrintProperties(tb)
   
   ##Output:
   ----------
   [CURSORPOS]: 0
   [EDITABLE]: 0
   [HEIGHT]: 80
   [ROTATION]: 0
   [TEXT]: one
   [VISIBLE]: 1
   [WIDTH]: 400
   [X]: 20
   [Y]: 20
   [ZOOMX]: 1
   [ZOOMY]: 1
   ----------

**See Also:**

:func:`Print()`


.. index:: PropertyExists

PropertyExists()
----------------

*Determines whether a particular property exists*

**Description:**

Tests whether a particular named property exists. This works for custom or built-in objects. This is important to check properties that might not exist, because trying to ``GetProperty`` of a non-existent property will cause a fatal error.

**Example:**

.. code-block:: pebl

   obj <- MakeCustomObject("myobject")
     obj.taste <- "buttery"
     obj.texture <- "creamy"
     SetProperty(obj,"flavor","tasty")
     
     list <- GetPropertyList(obj)
     loop(i,list)
      {
        if(PropertyExists(obj,i)
         {
           Print(i  + ":  " + GetProperty(obj,i))
         }
      }

**See Also:**

:func:`GetPropertyList`, :func:`GetProperty`, :func:`SetProperty` :func:`MakeCustomObject`, :func:`PrintProperties`


.. index:: Rectangle

Rectangle()
-----------

*Creates rectangle with size (dx, dy) centered at position x,y*

**Description:**

Creates a rectangle for graphing at x,y with size   dx and dy. Rectangles are only currently definable oriented in   horizontal/vertical directions.  A rectangle  must be added   to a parent widget before it can be drawn; it may be added to   widgets other than a base window.  The properties of rectangles may be   changed by accessing their properties directly, including the FILLED   property which makes the object an outline versus a filled shape.

**Usage:**

.. code-block:: pebl

   Rectangle(<x>, <y>, <dx>, <dy>, <color>)

**Example:**

.. code-block:: pebl

   r <- Rectangle(30,30,20,10, MakeColor(green))
     AddObject(r, win)
     Draw()

**See Also:**

:func:`Circle()`, :func:`Ellipse()`, :func:`Square()`, :func:`Line()`


.. index:: RemoveObject

RemoveObject()
--------------

*Removes an object from a parent window*

**Description:**

Removes a child widget from a parent.  Useful if   you are adding a local widget to a global window inside a loop.  If   you do not remove the object and only ``Hide()`` it, drawing will   be sluggish.  Objects that are local to a function are removed   automatically when the function terminates, so you do not need to   call ``RemoveObject()`` on them at the end of a function.

**Usage:**

.. code-block:: pebl

   RemoveObject( <object>, <parent>)


.. index:: ResizeWindow

ResizeWindow()
--------------

*Resizes a window to a specified width and height*

**Description:**

Resizes an existing window to the specified dimensions. This allows you to dynamically change the size of a window during program execution.

**Usage:**

.. code-block:: pebl

   ResizeWindow(<window>, <width>, <height>)

**Example:**

.. code-block:: pebl

   win <- MakeWindow("grey")
   ##Start with default size, then resize
   ResizeWindow(win, 1024, 768)
   Draw()

**See Also:**

:func:`MakeWindow()`


.. index:: RotoZoom

RotoZoom()
----------

*Rotates and zooms a graphical widget*

**Description:**

Rotates and zooms a widget (such as an image or label) by specified amounts. The rotation parameter specifies rotation in degrees. The xzoom and yzoom parameters specify scaling factors for horizontal and vertical dimensions (1.0 = no change, 2.0 = double size, 0.5 = half size). The smooth parameter (0 or 1) determines whether to use anti-aliasing for smoother appearance.

**Usage:**

.. code-block:: pebl

   RotoZoom(<widget>, <rotation>, <xzoom>, <yzoom>, <smooth>)

**Example:**

.. code-block:: pebl

   img <- MakeImage("stimulus.png")
   AddObject(img, win)
   Move(img, 320, 240)

   ##Rotate 45 degrees, double size, with smoothing
   RotoZoom(img, 45, 2.0, 2.0, 1)
   Draw()

   ##Rotate 90 degrees, normal size, no smoothing
   RotoZoom(img, 90, 1.0, 1.0, 0)
   Draw()

**See Also:**

:func:`Move()`, :func:`MakeImage()`, :func:`MakeLabel()`


.. index:: SaveAudioToWaveFile

SaveAudioToWaveFile()
---------------------

*Saves buffer to a .wav file format*

**Description:**

Saves a buffer, recorded using the GetAudioInputBuffer, to a .wav file for later analysis or archive.

**Usage:**

.. code-block:: pebl

   SaveAudioToWaveFile(filename, buffer)

**Example:**

.. code-block:: pebl

   gResponseBuffer <- MakeAudioInputBuffer(5000)
   	  resp0 <-  GetVocalResponseTime(gResponseBuffer,.35, 200)
         SaveAudioToWaveFile("output.wav",gResponseBuffer)

**See Also:**

:func:`GetVocalResponseTime()`, :func:`MakeAudioInputBuffer()`, :func:`RecordToBuffer()`


.. index:: RecordToBuffer

RecordToBuffer()
----------------

*Records audio input to a pre-allocated buffer*

**Description:**

Records audio from the microphone directly into a pre-allocated audio buffer. This function allows precise control over recording duration and provides a synchronous recording interface. The buffer must be created first using :func:`MakeAudioInputBuffer()`. An optional duration parameter can specify recording time in milliseconds; if omitted, the function records for the full buffer duration. *ONLY AVAILABLE ON WINDOWS AND LINUX*.

**Usage:**

.. code-block:: pebl

   RecordToBuffer(<buffer>)
   RecordToBuffer(<buffer>, <duration_ms>)

**Example:**

.. code-block:: pebl

   ## Record for full buffer duration (5 seconds)
   buffer <- MakeAudioInputBuffer(5000)
   RecordToBuffer(buffer)
   SaveAudioToWaveFile("recording.wav", buffer)

   ## Record for specific duration (3 seconds into 5 second buffer)
   buffer <- MakeAudioInputBuffer(5000)
   RecordToBuffer(buffer, 3000)
   SaveAudioToWaveFile("recording-3s.wav", buffer)

**See Also:**

:func:`MakeAudioInputBuffer()`, :func:`SaveAudioToWaveFile()`, :func:`GetVocalResponseTime()`, :func:`StartAudioMonitor()`


.. index:: StartAudioMonitor

StartAudioMonitor()
-------------------

*Starts real-time audio monitoring with ring buffer*

**Description:**

Creates and starts an audio monitoring system that continuously records audio in a ring buffer. This is useful for real-time audio analysis, voice key calibration, and audio level visualization. The monitor runs in the background and can be queried with :func:`GetAudioStats()` to retrieve recent audio statistics. The monitor must be stopped with :func:`StopAudioMonitor()` to free audio hardware resources. *ONLY AVAILABLE ON WINDOWS AND LINUX*.

**Usage:**

.. code-block:: pebl

   StartAudioMonitor(<buffer_size_ms>)

**Example:**

.. code-block:: pebl

   ## Start monitoring with 3-second ring buffer
   monitor <- StartAudioMonitor(3000)

   ## Continuously check audio levels
   loop(i, Sequence(1, 100, 1))
   {
       Wait(100)
       stats <- GetAudioStats(monitor, 500)  ## Last 500ms
       energy <- First(stats)
       Print("Energy: " + energy)
   }

   StopAudioMonitor(monitor)

**See Also:**

:func:`StopAudioMonitor()`, :func:`GetAudioStats()`, :func:`GetVocalResponseTime()`


.. index:: StopAudioMonitor

StopAudioMonitor()
------------------

*Stops audio monitoring and releases audio hardware*

**Description:**

Stops an audio monitor created by :func:`StartAudioMonitor()` and releases the audio hardware. This function performs complete cleanup including pausing recording, closing the SDL audio device, and clearing global audio state. It is critical to call this function before starting other audio operations like :func:`GetVocalResponseTime()` to ensure the audio hardware is available. *ONLY AVAILABLE ON WINDOWS AND LINUX*.

**Usage:**

.. code-block:: pebl

   StopAudioMonitor(<monitor>)

**Example:**

.. code-block:: pebl

   ## Monitor audio for 10 seconds then stop
   monitor <- StartAudioMonitor(2000)
   Wait(10000)
   StopAudioMonitor(monitor)

   ## Now audio hardware is free for other operations
   buffer <- MakeAudioInputBuffer(5000)
   rt <- GetVocalResponseTime(buffer, 0.35, 200)

**See Also:**

:func:`StartAudioMonitor()`, :func:`GetAudioStats()`


.. index:: GetAudioStats

GetAudioStats()
---------------

*Retrieves audio statistics from monitoring buffer*

**Description:**

Returns audio statistics from the most recent N milliseconds of an audio monitor created by :func:`StartAudioMonitor()`. The function returns a list containing three values: [energy, power, rmssd]. Energy represents total signal energy, power represents average power, and rmssd (root mean square of successive differences) indicates signal variability. These statistics are useful for voice key calibration, detecting speech onset, and monitoring audio levels. *ONLY AVAILABLE ON WINDOWS AND LINUX*.

**Usage:**

.. code-block:: pebl

   GetAudioStats(<monitor>, <window_ms>)

**Example:**

.. code-block:: pebl

   ## Monitor and display real-time audio statistics
   monitor <- StartAudioMonitor(5000)

   loop(i, Sequence(1, 50, 1))
   {
       Wait(200)
       stats <- GetAudioStats(monitor, 1000)  ## Last 1 second
       energy <- First(stats)
       power <- Nth(stats, 2)
       rmssd <- Third(stats)

       Print("Energy: " + energy + " Power: " + power + " RMSSD: " + rmssd)
   }

   StopAudioMonitor(monitor)

**See Also:**

:func:`StartAudioMonitor()`, :func:`StopAudioMonitor()`, :func:`GetVocalResponseTime()`


.. index:: SetCursorPosition

SetCursorPosition()
-------------------

**Description:**

Moves the editing cursor to a specified character 		position in a textbox.

**Usage:**

.. code-block:: pebl

   SetCursorPosition(<textbox>, <integer>)

**Example:**

.. code-block:: pebl

   SetCursorPosition(tb, 23)

**See Also:**

:func:`SetEditable()`, :func:`GetCursorPosition()`, :func:`SetText()`, :func:`GetText()`


.. index:: SetEditable

SetEditable()
-------------

*Turns on or off the editing cursor*

**Description:**

Sets the ``editable'' status of the textbox.  All   this really does is turns on or off the cursor; editing must be done   with the (currently unsupported) device function ``GetInput()``.

**Usage:**

.. code-block:: pebl

   SetEditable()

**Example:**

.. code-block:: pebl

   SetEditable(tb, 0)
   SetEditable(tb, 1)

**See Also:**

:func:`GetEditable()`


.. index:: SetFont

SetFont()
---------

*Changes the font of a text object*

**Description:**

Resets the font of a textbox or label.  Change will   not appear until the next ``Draw()`` function is called.  Can be   used, for example, to change the color of a label to give richer   feedback about correctness on a trial (see example below).  Font can alse be set by assigning to the object.font property of an object.

**Usage:**

.. code-block:: pebl

   SetFont(<text-widget>, <font>)

**Example:**

.. code-block:: pebl

   fontGreen <- MakeFont("vera.ttf",1,22, 
                         MakeColor("green"),
                         MakeColor("black"), 1)
   fontRed   <- MakeFont("vera.ttf",1,22,
                         MakeColor("red"),
                         MakeColor("black"), 1)
   label <- MakeLabel(fontGreen, "Correct")
   
   #Do trial here.       	
   
   if(response == 1)
   {
   SetText(label, "CORRECT")
   SetFont(label, fontGreen)
   } else {
   SetText(label, "INCORRECT")
   SetFont(label, "fontRed)
   }
   Draw()

**See Also:**

:func:`SetText()`


.. index:: SetPanning

SetPanning()
------------

*Sets volume of left and right channel.*

**Description:**

Sets the audio panning; the volume of the left and right audio channels.

**Usage:**

.. code-block:: pebl

   SetPanning(<audio>,<left>,<right>)

**Example:**

.. code-block:: pebl

   one <- LoadSound("1.wav")
      PlayForeground(one)
      SetPanning(one,1.0,0.0)
      PlayForeground(one)
      SetPanning(one,.5,.5)
      PlayForeground(one)

**See Also:**

:func:`LoadSound`


.. index:: SetPlayRepeats

SetPlayRepeats()
----------------

*Sets a repeat count on a sound playback.*

**Description:**

Sets repetition count on an audio file.  When played back, i   will play this sound reps+1 times. If set to 0, it will play just once.  If set to -1, it   will repeat indefinitely.

**Usage:**

.. code-block:: pebl

   SetPlayRepeats(<audio>,<reps>)

**Example:**

.. code-block:: pebl

   one <- LoadSound("1.wav")
      PlayForeground(one)
      SetPlayRepeats(one,5)
      PlayForeground(one)
      SetPanning(one,-1)
      PlayBackground(one)
      Wait(5000)
      Stop(one)

**See Also:**

:func:`LoadSound`


.. index:: SetProperty

SetProperty()
-------------

*Sets property of an object*

**Description:**

Sets a a property of a custom object.   This works for custom or built-in objects, but new properties can only be set on custom object. This function works essentially identically to the obj.property assignment, but it allows you to create property names from input. It is used extensively for the PEBL parameter setting.

**Example:**

.. code-block:: pebl

   obj <- MakeCustomObject("myobject")
     obj.taste <- "buttery"
     obj.texture <- "creamy"
     SetProperty(obj,"flavor","tasty")
     
     list <- GetPropertyList(obj)
     loop(i,list)
      {
        if(PropertyExists(obj,i)
         {
           Print(i  + ":  " + GetProperty(obj,i))
         }
      }

**See Also:**

:func:`GetProperty()`, :func:`PropertyExists()`, :func:`GetPropertyList()`, :func:`MakeCustomObject()`, :func:`PrintProperties()`


.. index:: SetPixel

SetPixel()
----------

*Sets the color of a pixel at specified coordinates*

**Description:**

Sets the pixel at x,y to a particular color. SetPixel is primarily useful for images and canvases--labels and textboxes get re-rendered upon draw so any use of SetPixel will get overwritten when it gets drawn. It won't work on windows or shapes. This function can also be called using SetPoint().

**Usage:**

.. code-block:: pebl

   SetPixel(<widget>, <x>, <y>, <color>)

**Example:**

.. code-block:: pebl

   back <- MakeCanvas(50,50)
   AddObject(back,gWin)
   col <- MakeColor("green")
   xy <- [[10,10],[10,11],[10,12],[10,13]]
   loop(i,xy)
   {
      SetPixel(back, First(i), Second(i), col)
   }
   Draw()

**See Also:**

:func:`SetPoint()`, :func:`MakeGabor()`, :func:`MakeCanvas()`, :func:`GetPixelColor()`


.. index:: SetPoint

SetPoint()
----------

*Sets the color of a point at specified coordinates (alias for SetPixel)*

**Description:**

Sets the pixel at x,y to a particular color. SetPoint is an alias for SetPixel() and functions identically. It is primarily useful for images and canvases--labels and textboxes get re-rendered upon draw so any use of SetPoint will get overwritten when it gets drawn. It won't work on windows or shapes.

**Usage:**

.. code-block:: pebl

   SetPoint(<widget>, <x>, <y>, <color>)

**Example:**

.. code-block:: pebl

   back <- MakeCanvas(50,50)
   AddObject(back,gWin)
   col <- MakeColor("green")
   xy <- [[10,10],[10,11],[10,12],[10,13]]
   loop(i,xy)
   {
      SetPoint(back, First(i), Second(i), col)
   }
   Draw()

**See Also:**

:func:`SetPixel()`, :func:`MakeGabor()`, :func:`MakeCanvas()`, :func:`GetPixelColor()`


.. index:: SetText

SetText()
---------

*Sets the text in a textbox or label*

**Description:**

Resets the text of a textbox or label.  Change will not 		appear until the next ``Draw()`` function is called.  The object.text property can also be used to change text of an object, by doing: ``object.text <- "new text"``

**Usage:**

.. code-block:: pebl

   SetText(<text-widget>, <text>)

**Example:**

.. code-block:: pebl

   # Fixation Cross:
   label <- MakeLabel(font, "+")
   Draw()
   
   SetText(label, "X")
   Wait(100)
   Draw()

**See Also:**

:func:`GetText()`, :func:`SetFont()`


.. index:: Show

Show()
------

*Shows an object*

**Description:**

Sets a widget to visible, once it has been added to   a parent widget.  This just changes the visibility property, it does   not make the widget appear.  The widget will not be displayed until   the ``Draw()`` function is called.  The .visible property of objects can also be used to hide or show the object.

**Usage:**

.. code-block:: pebl

   Show(<object>)

**Example:**

.. code-block:: pebl

   window <- MakeWindow()
   image1  <- MakeImage("pebl.bmp")
   image2  <- MakeImage("pebl.bmp")
   AddObject(image1, window)
   AddObject(image2, window)
   Hide(image2)
   Draw()
   Wait(300)
   Show(image2)
   Draw()

**See Also:**

:func:`Hide()`


.. index:: Square

Square()
--------

*Creates square with width size centered at position x,y*

**Description:**

Creates a square for graphing at x,y with size   ``<size>``. Squares are only currently definable oriented in   horizontal/vertical directions.  A square  must be added   to a parent widget before it can be drawn; it may be added to   widgets other than a base window.  The properties of squares may be   changed by accessing their properties directly, including the FILLED   property which makes the object an outline versus a filled shape.

**Usage:**

.. code-block:: pebl

   Ellipse(<x>, <y>, <size>, <color>)

**Example:**

.. code-block:: pebl

   s <- Square(30,30,20, MakeColor(green))
     AddObject(s, win)
     Draw()

**See Also:**

:func:`Circle()`, :func:`Ellipse()`, :func:`Rectangle()`, :func:`Line()`


.. index:: StartPlayback

StartPlayback()
---------------

*Initiates playback in background, updated with Wait()*

**Description:**

Initiates playback of a movie so that it will play in the background when a Wait() or WaitFor() function is called.  This allows one to collect a response while  playing a movie.  The movie will not actually play until the event loop is started, typically with something like Wait().

**Usage:**

.. code-block:: pebl

   StartPlayBack(movie)

**Example:**

.. code-block:: pebl

   movie <- LoadMovie("movie.avi",gWin,640,480)
      PrintProperties(movie)
      Move(movie,20,20)
      Draw() 
      StartPlayback(movie)
      Wait(500) #Play 500 ms of the movie.
      PausePlayback(movie)

**See Also:**

:func:`LoadAudioFile()`, :func:`LoadMovie()`, :func:`PlayMovie()`, :func:`PausePlayback()`


.. index:: Stop

Stop()
------

*Stops a sound playing in the background from playing*

**Description:**

Stops a sound playing in the background from   playing.  Calling ``Stop()`` on a sound object that is not   playing should have no effect, but if an object is aliased,   ``Stop()`` will stop the file.  Note that sounds play in a   separate thread, so interrupting the thread has a granularity up to   the duration of the thread-switching quantum on your computer; this   may be tens of milliseconds.

**Usage:**

.. code-block:: pebl

   Stop(<sound-object>)

**Example:**

.. code-block:: pebl

   buzz <- LoadSound("buzz.wav")
   PlayBackground(buzz)
   Wait(50)
   Stop(buzz)

**See Also:**

:func:`PlayForeground()`, :func:`PlayBackGround()`


.. index:: ThickLine

ThickLine()
-----------

*Creates a thick line between two points*

**Description:**

Makes a thick line between two coordinates. This uses the SDL\_gfx thickline primitive.

**Usage:**

.. code-block:: pebl

   ThickLine(<x1>,<y1>,<x2>,<y2>,
             <size-in-pixels>,<color>)

**Example:**

.. code-block:: pebl

   a <- ThickLine(10,10,300,400,20,
                   MakeColor("red"))
      AddObject(a,gWin)
      Draw()

**See Also:**

:func:`Line()`, :func:`Polygon()`
