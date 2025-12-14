================================================================================
Utility Library - Helpers and Utilities
================================================================================

This library contains utility functions for file operations, data management, and common helper tasks.

.. contents:: Function Index
   :local:
   :depth: 0



.. index:: CalibrateScreen

CalibrateScreen()
-----------------

**Description:**

Main calibration function
Returns custom object with calibration data

**Usage:**

.. code-block:: pebl

   define CalibrateScreen(win)



.. index:: ConcatenateList

ConcatenateList()
-----------------

*Combines list*

**Description:**

Combines a list together to form a single string. Like ListToString but defaults to a separator of " " (space).

**Usage:**

.. code-block:: pebl

   define ConcatenateList(...)

**Example:**

.. code-block:: pebl

   
   	ConcatenateList([1,2,3,444])	        	# == "1 2 3 444"
   	ConcatenateList(["a","b","c","d","e"],",")	# == "a,b,c,d,e"
   	
   	

**See Also:**

:func:`SubString()`, :func:`StringLength()`, :func:`FoldList()`,
	 :func:`ModList()`



.. index:: ConvertIPString

ConvertIPString()
-----------------

*Converts an ip-number-as-string to usable address*

**Description:**

Converts an IP address specified as a string into   an integer that can be used by ConnectToIP.

**Usage:**

.. code-block:: pebl

   define ConvertIPString(...)

**Example:**

.. code-block:: pebl

   See nim.pbl for example of two-way network connection.
   
     ip <- ConvertIPString("192.168.0.1")
     net <- ConnectToHost(ip,1234)
     dat <- GetData(net,20)
     Print(dat)
     CloseNetworkConnection(net)
   

**See Also:**

:func:`ConnectToHost()`, :func:`ConnectToIP()`, :func:`GetData()`, :func:`WaitForNetworkConnection()`,
   :func:`SendData()`, :func:`CloseNetworkConnection()`



.. index:: CR

CR()
----

**Description:**

Produces ``<number>`` linefeeds which can be added to a   string and printed or saved to a file.  CR is an abbreviation for ``Carriage Return``.

**Usage:**

.. code-block:: pebl

   define CR(...)

**Example:**

.. code-block:: pebl

   
            Print("Number: "  Tab(1) + number  + CR(2))
            Print("We needed space before this line.")
   

**See Also:**

:func:`Format()`, :func:`Tab()`



.. index:: DirlistToText

DirlistToText()
---------------

**Description:**

appends a set of nested directories into a path.

**Usage:**

.. code-block:: pebl

   define DirlistToText(list)



.. index:: DrawObject

DrawObject()
------------

*Calls the .draw property of an object*

**Description:**

Calls the function named by the  .draw property of a custom object.  Useful for handling drawing of a bunch of different objects. This is essentially the same as CallFunction(obj.draw, [obj]), but falls back to a normal Draw() command so it handles built-in objects as well.

**Usage:**

.. code-block:: pebl

   define DrawObject(...)

**Example:**

.. code-block:: pebl

   
     
      ##This overrides buttons placement at the center:  
      done <- MakeButton("QUIT",400,250,gWin,150)
      WaitForClickOnTarget([done],[1])
      Clickon(done,gClick)
      DrawObject(done)
       
   

**See Also:**

:func:`Inside()`, :func:`ClickOnCheckbox()`, :func:`MoveObject()`, :func:`Draw()`



.. index:: EasyLabel

EasyLabel()
-----------

**Description:**

Creates and adds to the window location a label   at specified location. Uses standard vera font with grey background.    (May in the future get background color from window).   Easy-to-use replacement for the ``MakeFont``,  ~``MakeLabel``,  ~ ``AddObject``, ~ ``Move``, steps you typically have to go through.    The optional argument fontsize defaults to 16-point.  The optional argument fg specifies a color name (e.g., ``"red"``) to use, and style specifies the font style, where 0,1,2,3 = normal, italic, bold, bolditalic.

**Usage:**

.. code-block:: pebl

   define EasyLabel(...)

**Example:**

.. code-block:: pebl

   
     win <- MakeWindow()
     lab <- EasyLabel("What?",200,100,win)
     Draw()
     lab <- EasyLabel("What?",200,100,win,12)
   
   

**See Also:**

:func:`EasyTextBox()`, :func:`MakeLabel()`



.. index:: EasyTextBox

EasyTextBox()
-------------

**Description:**

Creates and adds to the window location a textbox   at specified location. Uses standard vera font with white background.   Easy-to-use replacement for the MakeFont,MakeTextBox,   AddObject, Move, steps.    The optional arguments fgcolor and bgcolor should specify color names (like white and black).  By default, the textbox is created with a foreground of ``"black"`` and a background of ``"white"``.

**Usage:**

.. code-block:: pebl

   define EasyTextBox(...)

**Example:**

.. code-block:: pebl

   
     win <- MakeWindow()
     entry <- EasyTextBox("1 2 3 4 5",200,100,
                           win,12,200,50)
     Draw()
     entry <- EasyTextBox("1 2 3 4 5",200,100,
                           win,12,200,50,"red","blue")
     Draw()
   

**See Also:**

:func:`EasyLabel()`, :func:`MakeTextBox()`



.. index:: EndsWith

EndsWith()
----------

**Description:**

############################################################################
Parameter file handling - supports both legacy CSV and modern JSON formats
############################################################################
Helper function: Check if a string ends with a given suffix
Used to detect .json file extensions for parameter files

**Usage:**

.. code-block:: pebl

   define EndsWith(string, suffix)



.. index:: Enquote

Enquote()
---------

*Returns string surrounded by quote marks.*

**Description:**

Surrounds the argument with quotes.

**Usage:**

.. code-block:: pebl

   define Enquote(...)

**Example:**

.. code-block:: pebl

   
    ##use to add quoted text to instructions.
    instructions <- "Respond whenever you see an "+ 
                     Enquote("X")
   
     ##Use it for saving data that may have spaces: 
     resp <-  GetInput(tb, "<enter>")
     FilePrint(fileout, Enquote(resp))
   
   

**See Also:**

gQuote



.. index:: FetchText

FetchText()
-----------

**Description:**

Helper function: Fetch text from URL or local file
Handles both http:// URLs and local file paths

**Usage:**

.. code-block:: pebl

   define FetchText(source)



.. index:: FilePrintList

FilePrintList()
---------------

**Description:**

Prints a list to a file, without the ','s or []   characters. Puts a carriage return at the end.  Returns a string   that was printed.  If a list contains other lists, the printing will   wrap multiple lines and the internal lists will be printed as   normal.  To avoid this, try FilePrintList(file,Flatten(list)).

**Usage:**

.. code-block:: pebl

   define FilePrintList(...)

**Example:**

.. code-block:: pebl

   
   
   FilePrintList(fstream, [1,2,3,4,5,5,5])
   ##
   ##  Produces:
   ##1 2 3 4 5 5 5
   FilePrintList(fstream,[[1,2],[3,4],[5,6]])
   #Produces:
   # [1,2]
   #,[3,4]
   #,[5,6]
   
   FilePrintList(fstream,Flatten([[1,2],[3,4],[5,6]]))
   #Produces:
   # 1 2 3 4 5 6
   
   
   

**See Also:**

:func:`Print()`, :func:`Print_()`, :func:`FilePrint()`, :func:`FilePrint_()`, :func:`PrintList()`,



.. index:: FormatText

FormatText()
------------

**Description:**

this works at replacing carriage returns (\n) etc. from text

**Usage:**

.. code-block:: pebl

   define FormatText(text)



.. index:: GetEasyChoice

GetEasyChoice()
---------------

*Simple multiple choice*

**Description:**

Hides what is on the screen and presents a textbox with   specified message, and a series of options to select from. Returns element from corresponding position of the ``<output>`` list.

**Usage:**

.. code-block:: pebl

   define GetEasyChoice(...)

**Example:**

.. code-block:: pebl

   The code snippet below produces the following screen: gWin <- MakeWindow("white")
    inp <-  GetEasyChoice("What Year are you in school",
                           ["First-year","Sophomore",
                           "Junior","Senior","Other"],
                           [1,2,3,4,5],  gWin)
    
   
   

**See Also:**

:func:`MessageBox()`, :func:`GetEasyChoice()`, :func:`EasyTextBox()`



.. index:: GetEasyInput

GetEasyInput()
--------------

*Gets typed input based on a prompt.*

**Description:**

Hides what is on the screen and presents a textbox with   specified message, and a second text box to enter input.  Continues   when 'enter' it hit at the end of text entry.

**Usage:**

.. code-block:: pebl

   define GetEasyInput(...)

**Example:**

.. code-block:: pebl

   
    gWin <- MakeWindow()
    inp <-  GetEasyInput("Enter Participant ID Code",gWin)
   

**See Also:**

:func:`MessageBox()`, :func:`GetEasyChoice()`, :func:`EasyTextBox()`



.. index:: GetEasyMultiChoice

GetEasyMultiChoice()
--------------------

*Simple select-multiple choice*

**Description:**

The minchoices and maxchoices gives the range of the number of choices permitted.


**Usage:**

.. code-block:: pebl

   define GetEasyMultiChoice(text,choices,output,win,minchoices:1, maxchoices:1)



.. index:: GetNewDataFile

GetNewDataFile()
----------------

*Opens a data file in subnum directory*

**Description:**

Creates a data file for output, asking for either append or renumbering the subject code if the specified file is already in use.

**Usage:**

.. code-block:: pebl

   define GetNewDataFile(...)

**Example:**

.. code-block:: pebl

   
     file1 <- GetNewDataFile("1",gWin,"memorytest","csv",
                     "sub,trial,word,answer,rt,corr")
   ##above creates a file data\1\memorytest-1.csv
   
    file2 <- GetNewDataFile("1",gWin,"memorytest","csv",
                     "sub,trial,word,answer,rt,corr")
   # above will prompt you for new subject code
   
    file3 <- GetNewDataFile("1",gWin,"memorytest-report","txt",
                     "")
   ##No header is needed on a text-based report file.
   
   

**See Also:**

:func:`FileOpenWrite()`, :func:`FileOpenAppend()`, :func:`FileOpenOverwrite()`



.. index:: GetNIMHDemographics

GetNIMHDemographics()
---------------------

*Asks NIMH-related questions*

**Description:**

Gets demographic information that are normally required for NIMH-related research.  Currently are gender (M/F/prefer not to say), ethnicity (Hispanic or not), and race (A.I./Alaskan, Asian/A.A., Hawaiian, black/A.A., white/Caucasian, other).   		It then prints their responses in a single line in the demographics file, along with any special code you supply and a time/date stamp. This code might include a subject number, experiment number, or something else, but many informed consent forms assure the subject that this information cannot be tied back to them or their data, so be careful about what you record. The file output will look something like:  

.. code-block:: text

   ----  31,Thu May 12 17:00:35 2011,F,hisp,asian,3331 32,Thu May 12 22:49:10 2011,M,nothisp,amind,3332 ----

   	The first column is the user-specified code (in this  	case, indicating the experiment number).  The middle columns  	indicate date/time, and the last three columns indicate  	gender (M, F, other), Hispanic (Y/N), and race.

**Usage:**

.. code-block:: pebl

   define GetNIMHDemographics(...)

**Example:**

.. code-block:: pebl

   
   GetNIMHDemographics("x0413", gwindow, 
                       "x0413-demographics.txt")
   



.. index:: GetSubNum

GetSubNum()
-----------

*Asks user to enter subject number*

**Description:**

Creates dialog to ask user to input a subject code

**Usage:**

.. code-block:: pebl

   define GetSubNum(...)

**Example:**

.. code-block:: pebl

   
   ## Put this at the beginning of an experiment, 
   ## after a window gWin has been defined.
   ##
    if(gSubNum == 0)
     {
       gSubNum <- GetSubNum(gWin)
     }
   
   Note: gSubNum can also be set from the command line.



.. index:: GetTranslations

GetTranslations()
-----------------

*Loads multilingual translations with automatic language fallback*

**Description:**

Automatically loads translation strings for experiments from JSON translation files. Implements a three-tier fallback system: (1) uses explicitly specified language if file exists, (2) falls back to system locale language if available, (3) defaults to English if no other translations are found. Normalizes all language codes to lowercase 2-letter codes (e.g., "en", "es", "ar"). Returns a custom object with translated strings as properties that can be accessed throughout the experiment.

**Usage:**

.. code-block:: pebl

   GetTranslations(testname, lang)

**Example:**

.. code-block:: pebl

   ##Load translations with explicit language
   gLanguage <- "es"  ##Spanish
   gStrings <- GetTranslations("stroop", gLanguage)
   MessageBox(gStrings.instructions, gWin)

   ##Let it auto-detect from system locale
   gLanguage <- ""  ##Empty means use system locale
   gStrings <- GetTranslations("flanker", gLanguage)
   ##Will try system language, fall back to English

   ##Translation files are expected at:
   ##translations/stroop.pbl-es.json
   ##translations/stroop.pbl-en.json
   ##etc.

**See Also:**

:func:`ReadTranslationJSON()`, :func:`ReadTranslation()`, :func:`GetSystemLocale()`, :func:`Lowercase()`



.. index:: InitializeUpload

InitializeUpload()
------------------

**Description:**

############################################################################
Token-based multi-test hosting support
############################################################################
Initialize token-based upload configuration for uploading on emscipten branch.

This is only needed on emscripten to set up the virtual file system to store data
in an easy-to-retrieve way.  On native platforms, the Upload


Call this at the start of battery tests that will be hosted online
Reads upload.json (written by JavaScript launcher) and sets up:
- gToken: Study identifier
- gTestName: Test name (e.g., "stroop", "flanker")
- gUploadURL: Server endpoint for data upload (not needed here)
- gParticipant: Participant ID
- gDataDirectory: Centralized data path: /data/{token}/{test}/{participant}/
- gUploadSettings: Full configuration object
- gUseUpload: Flag indicating token mode is active

**Usage:**

.. code-block:: pebl

   define InitializeUpload(file:"")



.. index:: Inside

Inside()
--------

*Determines whether a point is inside a graphical object*

**Description:**

Determines whether an ``[x,y]`` point is inside another   object.  Will operate correctly for rectangles, squares, circles,   textboxes, images, and labels. For custom objects having a function name bound to their .inside property, it will use that function to test for insideness. ``[xylist]`` can be a list containing   [x,y], and if it is longer the other points will be ignored (such as   the list returned by ``WaitForMouseButton()``.  Returns 1 if inside, 0   if not inside.

**Usage:**

.. code-block:: pebl

   define Inside(...)

**Example:**

.. code-block:: pebl

   
         button <- EasyLabel("Click me to continue", 100,100,gWin,12)
   
         continue <- 1
         while(continue)
         {
            xy <- WaitForMouseButton()
            continue <- Inside(xy,button)
         }
   

**See Also:**

:func:`WaitForMouseButton()`, :func:`GetMouseCursorPosition()`, :func:`InsideTB()`



.. index:: IsURL

IsURL()
-------

**Description:**

Helper function: Check if a string is a URL
Used to detect http:// or https:// URLs for remote parameter loading

**Usage:**

.. code-block:: pebl

   define IsURL(string)



.. index:: JSONText

JSONText()
----------

**Description:**

this will print the JSON object in a format that can be saved.
It requires an PCustomObject, which is created with ParseJSON() function.

**Usage:**

.. code-block:: pebl

   define JSONText(obj, indent:0)



.. index:: LikertTrial

LikertTrial()
-------------

**Description:**

These helper functions require gTextBox, gHeader, and gFooter to work.

**Usage:**

.. code-block:: pebl

   define LikertTrial(text)



.. index:: ListToHumanText

ListToHumanText()
-----------------

**Description:**

Converts a list of a text listing of options

**Usage:**

.. code-block:: pebl

   define ListToHumanText(...)

**Example:**

.. code-block:: pebl

   
   ListToHumanText([1,2,3,444])
   
   "1, 2, 3, or 444"
   
   ListToHumanText(["a","b","c","d","e"],"and")
   "a, b, c, d, and e"
   
   

**See Also:**

:func:`ConcatenateList()`, :func:`PrintList()`, :func:`ListToString()`



.. index:: Lookup

Lookup()
--------

**Description:**

Returns element in ``<database>`` corresponding to element of ``<keylist>`` that matches ``<key>``.  If no match exists, Match returns an empty list.

**Usage:**

.. code-block:: pebl

   define Lookup(...)

**Example:**

.. code-block:: pebl

   
    keys     <- [1,2,3,4,5]
    database <- ["market","home","roast beef",
                 "none","wee wee wee"]
    Print(Lookup(3,keys,database))) 
   
   ## Or, do something like this:
     
   data  <- [["punky","brewster"],
             ["arnold","jackson"],
             ["richie","cunningham"],
             ["alex","keaton"]]
   
   d2 <- Transpose(data)
   key <- First(data)
   
   Print(Lookup("alex", key, data))
   ##Returns ["alex","keaton"]
   

**See Also:**

:func:`Match()`



.. index:: MakeParameterObject

MakeParameterObject()
---------------------

**Description:**

This creates an object called 'parameters' with
property-value pairs specified by pairs, and
will load duplicate properties into lists.

**Usage:**

.. code-block:: pebl

   define MakeParameterObject(pairs)



.. index:: MessageBox

MessageBox()
------------

*Pops up a message, overtop the entire screen, and waits for a click to continue.*

**Description:**

Hides what is on the screen and presents a textbox with   specified message, with a button to click at the bottom to continue.   All arguments after window are optional, but permit changing the size of the text box, (left and right separately), removing the background, and allowing keyboard responses to advance.    By default, if acknowledgement is set to <OK>, the messagebox will continue when the mouse button clicks   an on-screen button labeled OK. If set to a key (e.g., 'x'), it will continue when that key is pressed.

**Usage:**

.. code-block:: pebl

   define MessageBox(...)

**Example:**

.. code-block:: pebl

   
    gWin <- MakeWindow()
    MessageBox("Click below to begin.",gWin)
    
     MessageBox("this makes a messagebox filling the left side, permitting
     graphics you might have put on the right to be displayed.",
                gWin,40,100,gVideoWidth/2,
                300,0,"<OK>")
               
     MessageBox("This messagebox allows you to continue by hitting the x or z keys",
        gWin,20,100,100,300,1,["X","Z"]) 
                
   

**See Also:**

:func:`GetEasyInput()`, :func:`EasyTextBox()`, :func:`PopUpMessageBox()`



.. index:: MoveCenter

MoveCenter()
------------

**Description:**

Moves a TextBox to a specified location 		according to its center, instead of its upper left corner.

**Usage:**

.. code-block:: pebl

   define MoveCenter(...)

**Example:**

.. code-block:: pebl

   
   MoveCenter(TextBox, 33, 100)
   

**See Also:**

:func:`Move()`, :func:`MoveCenter()`, ``.X`` and ``.Y`` properties



.. index:: MoveCorner

MoveCorner()
------------

*Moves an image or label by its upper corner.*

**Description:**

Moves a label or image to a specified location 		according to its upper left corner, instead of its center.

**Usage:**

.. code-block:: pebl

   define MoveCorner(...)

**Example:**

.. code-block:: pebl

   
   MoveCorner(label, 33, 100)
   

**See Also:**

:func:`Move()`, :func:`MoveCenter()`, ``.X`` and ``.Y`` properties



.. index:: MoveObject

MoveObject()
------------

*Calls the .move property of an object*

**Description:**

Calls the function named by the  .move property of a custom object.  Useful if a custom object has complex parts that need to be moved; you can bind .move to a custom move function and then call it (and anything else) using MoveObject. ``MoveObject`` will fall back on a normal move, so you can handle movement of many built-in objects with it

**Usage:**

.. code-block:: pebl

   define MoveObject(...)

**Example:**

.. code-block:: pebl

   
     
      ##This overrides buttons placement at the center:  
      done <- MakeButton("QUIT",400,250,gWin,150)
      done.move <- "MoveCorner"
      MoveObject(done, 100,100)
       
   

**See Also:**

:func:`Inside()`, :func:`Move()`, :func:`ClickOn()`, :func:`DrawObject()`



.. index:: PrintList

PrintList()
-----------

**Description:**

Prints a list, without the ','s or []   characters. Puts a carriage return at the end.  Returns a string   that was printed.  If a list contains other lists, the printing will   wrap multiple lines and the internal lists will be printed as   normal.  To avoid this, try PrintList(Flatten(list)).

**Usage:**

.. code-block:: pebl

   define PrintList(...)

**Example:**

.. code-block:: pebl

   
   PrintList( [1,2,3,4,5,5,5])
   ##
   ##  Produces:
   ##1 2 3 4 5 5 5
   PrintList([[1,2],[3,4],[5,6]])
   #Produces:
   # [1,2]
   #,[3,4]
   #,[5,6]
   
   PrintList(Flatten([[1,2],[3,4],[5,6]]))
   #Produces:
   # 1 2 3 4 5 6
   
   

**See Also:**

:func:`Print()`, :func:`Print_()`, :func:`FilePrint()`, :func:`FilePrint_()`, :func:`FilePrintList()`,



.. index:: ReadCSV

ReadCSV()
---------

*Opens a csv file returning a table with its elements*

**Description:**

Reads a comma-separated  value file into a nested   list.  Need not be named with a .csv extension.  It should properly   strip quotes from cells, and not break entries on commas embedded   within quoted text.

**Usage:**

.. code-block:: pebl

   define ReadCSV(...)

**Example:**

.. code-block:: pebl

   
   	table <- ReadCSV("datafile.csv")
   

**See Also:**

:func:`FileReadTable()`, :func:`FileReadList()`, :func:`StripQuotes()`



.. index:: ReadJSONParameters

ReadJSONParameters()
--------------------

**Description:**

Read JSON parameter file and convert to parameter object
JSON format: simple key-value pairs, e.g., {"dopractice": 1, "isi": 1000}
Returns a custom object with parameters as properties
Supports both local files and URLs (http:// or https://)
Returns empty parameter object if fetch/parse fails (allows fallback to defaults)

**Usage:**

.. code-block:: pebl

   define ReadJSONParameters(filename)



.. index:: RemoveObjects

RemoveObjects()
---------------

*Removes a (possibly nested) list of objects from a parent window*

**Description:**

This is a recursive removeobjects

**Usage:**

.. code-block:: pebl

   define RemoveObjects(list,win)



.. index:: ReplaceChar

ReplaceChar()
-------------

**Description:**

Substitutes  ``<char2>`` for ``<char>``   in ``<string>``. Useful for saving subject entry data in a file;   replacing spaces with some other character.  The second argument can either be a character to match, or a list of characters to match, in which case they all get replaced with the third argument.

**Usage:**

.. code-block:: pebl

   define ReplaceChar(...)

**Example:**

.. code-block:: pebl

   
   
   x <- ["Sing a song of sixpence"]
   rep <- ReplaceChar(x," ", "_")
   Print(rep)
   # Result:  Sing_a_song_of_sixpence
   
   x <- ["sing a song of sixpence"]
   rep <- ReplaceChar(x,["s","x"], "p")
   Print(rep)
   # Result:  ping a pong of pippence
   

**See Also:**

for list items: :func:`Replace()` , :func:`SplitString()`,



.. index:: SplitStringSlow

SplitStringSlow()
-----------------

**Description:**

Splits a string into tokens. ``<split>`` must be a string. If  		``<split>`` is not found in ``<string>``, a list containing the entire  		string is returned; if split is equal to ``""``, the each letter  		in the string is placed into a different item in the list.  The entire text of ``<split>`` is used to tokenize, but as a consequence this function is relatively slow, and should be avoided if your string is longer than a few hundred characters.

**Usage:**

.. code-block:: pebl

   define SplitStringSlow(...)

**Example:**

.. code-block:: pebl

   
   SplitStringSlow("Everybody Loves a Clown", " ") 
   # Produces ["Everybody", "Loves", "a", "Clown"]
   SplitStringSlow("she sells seashells", "ll")
   #produces ["she se","s seashe", "s"] 
   
   

**See Also:**

:func:`Splitstring()`     	:func:`FindInString()`, :func:`ReplaceChar()`



.. index:: StripQuotes

StripQuotes()
-------------

**Description:**

Strips quotation marks from the outside of a   string.  Useful if you are reading in data that is quoted.

**Usage:**

.. code-block:: pebl

   define StripQuotes(...)

**Example:**

.. code-block:: pebl

   
    text <- gQuote + "abcd" + gQuote
    Print(StripQuotes(text))  ## abcd
    Print(StripQuotes("aaa")) ##aaa
   

**See Also:**

:func:`StripSpace()`



.. index:: StripSpace

StripSpace()
------------

**Description:**

Strips spaces from the start and end of a   string.  Useful for cleaning up input and such.

**Usage:**

.. code-block:: pebl

   define StripSpace(...)

**Example:**

.. code-block:: pebl

   
    text <-  " abcd  "
    Print(StripSpace(text))  ## 'abcd'
    Print(StripSpace("aaa")) ## 'aaa'
   

**See Also:**

:func:`StripQuotes()`



.. index:: Tab

Tab()
-----

**Description:**

Produces a tab character which can be added to a   string. If displayed in a text box, it will use a 4-item tab stop.

**Usage:**

.. code-block:: pebl

   define Tab(...)

**Example:**

.. code-block:: pebl

   
            Print("Number: "  Tab(1) + number )
            Print("Value: "  Tab(1) + value )
            Print("Size: "  Tab(1) + size )
   

**See Also:**

:func:`Format()`, :func:`CR()`



.. index:: WaitForButtonClickOnTarget

WaitForButtonClickOnTarget()
----------------------------

**Description:**

targetlist is a set of graphical objects,
keylist is a set of keys whose corresponding
value should be returned when a graphical object is clicked upon.
This modifies the built-in waitforclickontarget so that it will
Return the button that is clicked, along with the target,
and the target object

**Usage:**

.. code-block:: pebl

   define WaitForButtonClickOnTarget(targetlist,keylist)



.. index:: WaitForClickOnTarget

WaitForClickOnTarget()
----------------------

*Waits until any of a set of target objects are clicked.*

**Description:**

Allows you to specify a list of graphical objects in ``<objectlist>`` and awaits a click   on any one of them, returning the corresponding key in <keylist>.  Also, sets the    global variable gClick which saves the location of the click, if    you need it for something else.

**Usage:**

.. code-block:: pebl

   define WaitForClickOnTarget(...)

**Example:**

.. code-block:: pebl

   
     resp <- Sequence(1,5,1)
     objs <- []
     loop(i,resp)
     {
       tmp <- EasyLabel(i +". ",
                100+50*i,100,gWin,25)
       objs <- Append(objs, tmp)
     }
     Draw()
     click  <- WaitForClickOnTarget(objs,resp)
     Print("You clicked on " + click)
     Print("Click location: [" + First(gClick) + 
           ", " + Second(gClick) + "]")
   



.. index:: WaitForClickOnTargetWithTimeout

WaitForClickOnTargetWithTimeout()
---------------------------------

**Description:**

Allows you to specify a list of graphical objects in ``<objectlist>`` and awaits a click   on any one of them, returning the corresponding key in ``<keylist>``.  Also, sets the    global variable gClick which saves the location of the click, if    you need it for something else.  The function will return after the specified time limit.    If no response is made by timeout, the text <timeout> will be returned (instead of the corresponding keylist element), and gClick will be set to [-1, -1].  This function can also be useful to dynamically update some visual object while waiting for a response.  Give timeout some small number (below 50 ms, as low as 1-5), and loop over this repeatedly until a 'proper' response is given, redrawing a timer or other dynamic visual element each time.  By default, this will only activate when a normal (left-click) is made on button 1.  However, the three optional arguments button1, button2, and button3 permit waiting for any or all left, right, or center buttons.

**Usage:**

.. code-block:: pebl

   define WaitForClickOnTargetWithTimeout(...)

**Example:**

.. code-block:: pebl

   
     resp <- Sequence(1,5,1)
     objs <- []
     loop(i,resp)
     {
       tmp <- EasyLabel(i +". ",
                100+50*i,100,gWin,25)
       objs <- Append(objs, tmp)
     }
     Draw()
     click  <- WaitForClickOnTargetWithTimeout(objs,resp,3000)
     Print("You clicked on " + click)
     Print("Click location: [" + First(gClick) + 
           ", " + Second(gClick) + "]")
   
     ##wait for a center-click.
     click  <- WaitForClickOnTargetWithTimeout(objs,resp,3000,0,0,1)
   
     
   

**See Also:**

:func:`WaitForDownClick()`, :func:`WaitForMouseButton()`



.. index:: WaitForDownClick

WaitForDownClick()
------------------

*Waits for mouse button to be clicked*

**Description:**

Will wait until the mouse button is clicked down.  Returns   the same 4-tuple as ``WaitForMouseButton:`` 

.. code-block:: text

   [xpos,    ypos,     button id [1-3],     "<pressed>" or "<released>"]

  but the last element will always be ``<pressed>``.  Useful as a 'click mouse to continue' probe.

**Usage:**

.. code-block:: pebl

   define WaitForDownClick(...)

**Example:**

.. code-block:: pebl

   
     x <- WaitForDownClick()
     Print("Click location: [" + First(x) + 
           ", " + Second(x) + "]")  
   

**See Also:**

:func:`WaitForClickOnTarget()`, :func:`WaitForMouseButton()`



.. index:: YesNoTrial

YesNoTrial()
------------

**Description:**

These helper functions require gTextBox, gHeader, and gFooter to work.

**Usage:**

.. code-block:: pebl

   define YesNoTrial(text)



.. index:: ZeroPad

ZeroPad()
---------

*Pads the beginning of a number with 0s so the number is size long*

**Description:**

Takes a number and pads it with zeroes left of the   decimal point so that its length is equal to <size>. Argument must   be a positive integer and less than ten digits.  Returns a string.

**Usage:**

.. code-block:: pebl

   define ZeroPad(...)

**Example:**

.. code-block:: pebl

   
     Print(ZeroPad(33,5))     # "00033"
     Print(ZeroPad(123456,6)) #"123456"
     Print(ZeroPad(1,8))      #"00000001"
   

**See Also:**

:func:`Format()`


Functions Pending Documentation
--------------------------------



.. index:: AppendDirList

AppendDirList()
---------------

*Manages a list of directories for path navigation*

**Description:**

Appends a directory to a directory list, handling special directory names like "." (current directory) and ".." (parent directory). When ".." is encountered, it removes the last directory from the list (going up one level). When "." is encountered, the list remains unchanged. Regular directory names are simply appended. Useful for building and navigating directory paths dynamically.

**Usage:**

.. code-block:: pebl

   define AppendDirList(dirlist, dir)

**Example:**

.. code-block:: pebl


   # Build a directory path dynamically
   dirs <- []
   dirs <- AppendDirList(dirs, "data")      # ["data"]
   dirs <- AppendDirList(dirs, "exp1")      # ["data", "exp1"]
   dirs <- AppendDirList(dirs, "results")   # ["data", "exp1", "results"]
   dirs <- AppendDirList(dirs, "..")        # ["data", "exp1"] (go up)
   dirs <- AppendDirList(dirs, "analysis")  # ["data", "exp1", "analysis"]

   # Current directory is ignored
   dirs <- AppendDirList(dirs, ".")         # ["data", "exp1", "analysis"] (unchanged)


**See Also:**

:func:`DirToText()`, :func:`GetDirectory()`, :func:`GetDirectoryListing()`



.. index:: CreateParameters

CreateParameters()
------------------

*Creates a parameter object from defaults and an optional parameter file*

**Description:**

Creates a parameter object by combining default values with parameters from a file. Supports both legacy CSV format (.par) and modern JSON format (.par.json). Can load parameters from local files or remote URLs (http:// or https://). If the file doesn't exist or a URL cannot be fetched, only the defaults are used. Parameters from the file override defaults.

The function auto-detects file format based on extension: .json files (and all URLs) use JSON format, while .par files use legacy CSV format.

**Usage:**

.. code-block:: pebl

   define CreateParameters(defaults, file)

**Example:**

.. code-block:: pebl


   # Create parameters with defaults and local file
   defaults <- [["trials", 20], ["isi", 1000], ["practice", 1]]
   pars <- CreateParameters(defaults, "config.par.json")

   # Now pars.trials, pars.isi, pars.practice are available
   Print("Running " + pars.trials + " trials")

   # Load from URL (uses JSON format automatically)
   pars <- CreateParameters(defaults, "https://example.com/params.json")

   # If file doesn't exist, defaults are used
   pars <- CreateParameters(defaults, "nonexistent.json")
   # pars still has default values


**See Also:**

:func:`ReadJSONParameters()`, :func:`MakeParameterObject()`, :func:`ReadCSV()`



.. index:: DirToText

DirToText()
-----------

*Converts directory and file lists into path strings*

**Description:**

Combines a list of directories and a list of filenames into complete path strings. Prepends directory names with backslashes and merges them with the filelist. This is a helper function for path manipulation when working with directory structures.

**Usage:**

.. code-block:: pebl

   define DirToText(dirlist, filelist, path)

**Example:**

.. code-block:: pebl


   # Combine directory components with files
   dirs <- ["data", "experiment1"]
   files <- ["trial01.csv", "trial02.csv"]
   paths <- DirToText(dirs, files, "")

   # Results in paths with directory prefixes
   loop(p, paths)
   {
      Print(p)
   }


**See Also:**

:func:`AppendDirList()`, :func:`GetDirectory()`, :func:`GetDirectoryListing()`



.. index:: GetDirectory

GetDirectory()
--------------

*Extracts the directory path from a file path*

**Description:**

Extracts and returns the directory portion of a file path, including the trailing separator. The function automatically detects the appropriate separator based on the platform (backslash for Windows, forward slash for Unix/Linux/Mac). Returns all but the last component (the filename). Useful for getting the parent directory of a file or for constructing new file paths in the same directory.

**Usage:**

.. code-block:: pebl

   define GetDirectory(filepath)

**Example:**

.. code-block:: pebl


   # Extract directory from file path
   dir <- GetDirectory("data/experiment/results.csv")
   Print(dir)
   # Result: "data/experiment/" (on Unix/Linux/Mac)

   # Works automatically on Windows too
   dir <- GetDirectory("C:\Users\Lab\data\subject01.txt")
   Print(dir)
   # Result: "C:\Users\Lab\data\" (on Windows)

   # Use with other path operations
   filepath <- "results/trial-data.csv"
   directory <- GetDirectory(filepath)
   newfile <- directory + "summary-data.csv"
   # newfile is "results/summary-data.csv"

   # Common use case: creating pooled data files
   fileOut <- GetNewDataFile(gSubnum, gWin, "test", "csv", "header")
   dir <- GetDirectory(fileOut.filename) + "../"
   pooledFile <- FileOpenAppend(dir + "pooled-results.csv")


**See Also:**

:func:`GetWorkingDirectory()`, :func:`FileExists()`, :func:`DirToText()`



.. index:: GetNewSubNum

GetNewSubNum()
--------------

*Requests a new subject number from a remote server*

**Description:**

Connects to a remote PEBL data server to request and receive a new unique subject number for data collection. The server maintains a counter and returns sequential subject codes. If the server connection fails, a random 6-digit number is returned as a fallback. Used in labs or online studies where centralized subject numbering is needed across multiple testing stations or sessions.

**Usage:**

.. code-block:: pebl

   define GetNewSubNum(server, page, port, username, pword)

**Example:**

.. code-block:: pebl


   # Get new subject number from server
   server <- "data.mylab.edu"
   page <- "get_subject_id.php"
   port <- 80
   username <- "labuser"
   password <- "secure_password"

   gSubNum <- GetNewSubNum(server, page, port, username, password)
   Print("Assigned subject number: " + gSubNum)

   # Use subject number for data files
   datafile <- FileOpenWrite("data/" + gSubNum + "/results.csv")


**See Also:**

:func:`GetSubNum()`, :func:`UploadFile()`, :func:`SyncDataFile()`



.. index:: ReadTranslation

ReadTranslation()
-----------------

*Loads multilingual text from a CSV translation file*

**Description:**

Reads a CSV-format translation file containing text strings in multiple languages. Creates a custom object with text strings as properties that can be accessed in experiments. Useful for creating experiments that can be presented in different languages. The CSV file should have text keys in the first column and translated text in subsequent columns. Line breaks (\\n) in the file are converted to actual carriage returns.

**Usage:**

.. code-block:: pebl

   define ReadTranslation(filename, lang)

**Example:**

.. code-block:: pebl


   # Load English translations from CSV file
   # translations.csv contains:
   # welcome,Welcome to the experiment
   # instructions,Please press any key to begin
   # thanks,Thank you for participating

   trans <- ReadTranslation("translations-en.csv", "en")
   MessageBox(trans.welcome, gWin)
   MessageBox(trans.instructions, gWin)

   # Run experiment...

   MessageBox(trans.thanks, gWin)


**See Also:**

:func:`ReadTranslationJSON()`, :func:`ReadCSV()`, :func:`FormatText()`



.. index:: ReadTranslationJSON

ReadTranslationJSON()
---------------------

*Loads multilingual text from a JSON translation file*

**Description:**

Reads a JSON-format translation file containing text strings in multiple languages. Creates a custom object with text strings as properties that can be accessed in experiments. This is the modern alternative to ``ReadTranslation()`` with better support for complex text and special characters. The JSON file should contain key-value pairs where keys are text identifiers and values are the translated strings. Line breaks (\\n) in the JSON are converted to actual carriage returns.

**Usage:**

.. code-block:: pebl

   define ReadTranslationJSON(filename, lang)

**Example:**

.. code-block:: pebl


   # Load English translations from JSON file
   # translations-en.json contains:
   # {
   #   "welcome": "Welcome to the experiment",
   #   "instructions": "Please press any key to begin",
   #   "thanks": "Thank you for participating"
   # }

   trans <- ReadTranslationJSON("translations-en.json", "en")
   MessageBox(trans.welcome, gWin)
   MessageBox(trans.instructions, gWin)

   # Run experiment...

   MessageBox(trans.thanks, gWin)


**See Also:**

:func:`ReadTranslation()`, :func:`ParseJSON()`, :func:`FormatText()`



.. index:: SubstituteStrings

SubstituteStrings()
-------------------

*Makes multiple substitutions in text*

**Description:**

Performs multiple character substitutions in a text string. Takes a list of replacement pairs and applies each one sequentially using ``ReplaceChar()``. Useful for cleaning up text, converting special characters, or applying multiple text transformations at once.

**Usage:**

.. code-block:: pebl

   define SubstituteStrings(text, replist)

**Example:**

.. code-block:: pebl


   # Replace multiple characters for file-safe names
   text <- "Subject: John Doe (Test #1)"
   replacements <- [[" ", "_"], [":", "-"], ["(", "["], [")", "]"], ["#", "num"]]
   cleaned <- SubstituteStrings(text, replacements)
   Print(cleaned)
   # Result: "Subject-_John_Doe_[Test_num1]"

   # Convert special characters in user input
   userInput <- "Hello\nWorld!"
   subs <- [["\n", " "], ["!", "."]]
   formatted <- SubstituteStrings(userInput, subs)
   Print(formatted)
   # Result: "Hello World."


**See Also:**

:func:`ReplaceChar()`, :func:`Replace()`



.. index:: SyncDataFile

SyncDataFile()
--------------

*Synchronizes a data file with a remote server using HTTP POST*

**Description:**

Uploads a data file to a remote server using HTTP POST with multipart/form-data encoding. Sends authentication credentials, task name, subject code, and optional authorization token along with the file contents. Used for real-time data synchronization during experiments or for batch uploads to a PEBL data collection server. Returns the server's HTTP response.

**Usage:**

.. code-block:: pebl

   define SyncDataFile(server, page, port, username, pword, taskname, subcode, datafilename, token: "")

**Example:**

.. code-block:: pebl


   # Sync data file to server after each trial block
   server <- "data.mylab.edu"
   page <- "sync_data.php"
   port <- 80
   username <- "labuser"
   password <- "secure_password"
   taskname <- "stroop_task"
   subcode <- gSubNum
   datafile <- "data/" + subcode + "/block1.csv"

   response <- SyncDataFile(server, page, port, username,
                            password, taskname, subcode, datafile)

   Print("Server response: " + First(response))

   # With authorization token for secure studies
   token <- "study_auth_token_12345"
   response <- SyncDataFile(server, page, port, username, password,
                            taskname, subcode, datafile, token)


**See Also:**

:func:`UploadFile()`, :func:`GetNewSubNum()`, :func:`PostHTTPFile()`



.. index:: UploadFile

UploadFile()
------------

*Uploads a data file to a remote server*

**Description:**

Uploads experimental data files to a remote server for centralized data collection. Only operates when the ``--upload`` command-line flag is used with PEBL. Reads server configuration from a JSON settings file containing host, page, port, and authentication details. If no_upload flag is set in settings or the file doesn't exist, upload is skipped. Useful for online experiments or multi-site studies requiring automatic data upload.

**Usage:**

.. code-block:: pebl

   define UploadFile(subcode, datafilename, settings: "")

**Example:**

.. code-block:: pebl


   # Upload data file after experiment completes
   # Run PEBL with: pebl experiment.pbl --upload upload.json
   #
   # upload.json contains:
   # {
   #   "host": "data.mylab.edu",
   #   "page": "upload.php",
   #   "port": 80,
   #   "username": "labuser",
   #   "password": "secure_password"
   # }

   gSubNum <- "101"
   datafile <- "data/" + gSubNum + "/results.csv"

   # Upload will only happen if --upload was specified
   UploadFile(gSubNum, datafile)

   # Or specify custom settings file
   UploadFile(gSubNum, datafile, "custom_upload.json")


**See Also:**

:func:`SyncDataFile()`, :func:`GetNewSubNum()`, :func:`InitializeUpload()`



.. index:: ClickOn

ClickOn()
---------

**Description:**

Calls the function named by the .clickon property of a custom object. Useful for handling click events of a bunch of different objects. This is essentially the same as CallFunction(obj.clickon, [obj,gClick]).

**Usage:**

.. code-block:: pebl

   Clickon(obj,[x,y])

**Example:**

.. code-block:: pebl

   ##This overrides buttons placement at the center:  
      done <- MakeButton("QUIT",400,250,gWin,150)
      WaitForClickOnTarget([done],[1])
      Clickon(done,gClick)

**See Also:**

:func:`Inside()`, :func:`ClickCheckbox()`, :func:`MoveObject()`, :func:`DrawObject()`



.. index:: Countdown

Countdown()
-----------

**Description:**

Displays a 3-2-1 countdown on the screen in with 500 ms ISI. ``CountDown`` temporarily hides whatever is on the screen. It is useful in orienting participants to the first trial of a task. The second argument (useBackground) is true (1) by default. In this case, the entire screen will be hidden with a black overlay. If set to 0, this overlay will not be made.

**Usage:**

.. code-block:: pebl

   CountDown(win)
      CountDown(win,(optional)useBackground)

**Example:**

.. code-block:: pebl

   win <- MakeWindow()
     MessageBox("Press any key to begin",win)
     CountDown(win)
     Trial()
   
     CountDown(win,0) 
     Trial

**See Also:**

:func:`MessageBox()`



.. index:: GetInput

GetInput()
----------

**Description:**

Allows user to type input into a textbox.

**Usage:**

.. code-block:: pebl

   GetInput(<textbox>,<escape-key>)

**See Also:**

:func:`SetEditable()`, :func:`GetCursorPosition()`, :func:`MakeTextBox()`, :func:`SetText()`

