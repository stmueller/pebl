================================================================================
UI Library - User Interface
================================================================================

This library contains functions for creating user interface elements like buttons, textboxes, and checkboxes.

.. contents:: Function Index
   :local:
   :depth: 0



.. index:: ClearScrollboxThumbCapture

ClearScrollboxThumbCapture()
----------------------------

**Description:**

this supposedly gets called on a mouse release event?
it should also get called when you leave the window...

**Usage:**

.. code-block:: pebl

   define ClearScrollboxThumbCapture(obj,p,event:0)#+



.. index:: ClickOnMenu

ClickOnMenu()
-------------

*Handles menu click, calling the .clickon function of menu.*

**Description:**

Handles clicking on a menu item. It will call the .clickon property of that item, and then hide the menu.

**Usage:**

.. code-block:: pebl

   define ClickOnMenu(...)

**Example:**

.. code-block:: pebl

   This creates a menu and awaits clicking on.  More complete examples are available in ui.pbl.  It requires that MyMessage is created somewhere
   
   
      menu1 <- MakeMenuItem("File",0,0,gWin,14,10,"MYMESSAGE")
   
   
      menu2<- MakeMenu("Edit",70,0,gWin,14,10, "MYMESSAGE")
      
      menus <- [menu1,menu2]
      opt <- WaitForClickOntarget(menu,[1,2])
      ClickOnMenu(Nth(menus,opt),gClick)
   

**See Also:**

:func:`MakeMenu()`, :func:`OpenSubMenus()`, :func:`MakeMenuItem()`



.. index:: ClickOnScrollbox

ClickOnScrollbox()
------------------

*Handles click on scrollbox.*

**Description:**

Handles a click event on the a  ``ScrollBox``. This should be called after one checks (e.g., via InsideTB) whether the scrollbox was actually clicked on.  It will handle scrolling, moving via the thumb, up/down arrows, and reselection. It is also used to interact with ``ScrollingTextBox``  objects. This function name is bound to the .clickon property of scrollboxes, so it can be called using CallFunction (see example below).

**Usage:**

.. code-block:: pebl

   define ClickOnScrollbox(...)

**Example:**

.. code-block:: pebl

   See ui.pbl in the demo directory for examples of the use of a scrolling text box.  A brief example follows:
   
   
      sb <- MakeScrollBox(Sequence(1,50,1),"The numbers",40,40,gWin,12,150,500,3)
      Draw()
   	  
      resp <- WaitForClickOntarget([sb],[1])
       ClickOnScrollbox(sb,gClick) 
    
       #Alternately:   CallFunction(sb.clickon,[sb,gClick])
   
    
      ##change the selected items
      sb.list <- Sequence(sb.selected,sb.selected+50,1)
      UpdateScrollbox(sb)      
      DrawScrollbox(sb)
      Draw()
    
   

**See Also:**

``MakeScrollingTextBox``
``MakeScrollBox``
``UpdateScrollBox``
``DrawScrollBox``



.. index:: DrawPulldown

DrawPulldown()
--------------

*Redraws a pulldonw if state changes.*

**Description:**

This handles layout/drawing of a pulldown box. This does not actually call Draw() on the window, and so an additional draw command is needed before the output is displayed.  The main use case for this function is if you need to manually change the selected object (by changing .selected). This will redraw the pulldown with the new selection.

**Usage:**

.. code-block:: pebl

   define DrawPulldown(...)

**Example:**

.. code-block:: pebl

   
    options  <- MakePulldownButton(["A",B","C"],400,250,gWin,14,100,1)
    Draw()
    WaitForAnyKeyPress()
    options.selected <- 2
    DrawPulldown(options)
    Draw()
    WaitForAnyKeyPress()
   

**See Also:**

:func:`MakePullDown()`, :func:`Pulldown()`, :func:`UpdatePulldown()`



.. index:: DrawScrollingTextBox

DrawScrollingTextBox()
----------------------

**Description:**

this draws the current state of the scrollbox.
It should be called directly whenever things like the scrollbar,
offset, selected item are changed, but not when the list changes.
the only material side effect it can have is changing selected, which will update
to ensure it stays within bounds.

**Usage:**

.. code-block:: pebl

   define DrawScrollingTextBox(obj)



.. index:: EditScrollboxValue

EditScrollboxValue()
--------------------

**Description:**

make this separate so you can override for more custom edits.
see launcher experiment chain, where the chain is just a set of labels
that link to the 'real' chain.

**Usage:**

.. code-block:: pebl

   define EditScrollboxValue(win,click,default,selected)



.. index:: GetFullLineBreaks

GetFullLineBreaks()
-------------------

**Description:**

this attempts to get the full set of linebreaks from the
text attached to tb

**Usage:**

.. code-block:: pebl

   define GetFullLineBreaks(tb,text)



.. index:: InsideMenu

InsideMenu()
------------

**Description:**

This is offset from upper left corner

**Usage:**

.. code-block:: pebl

   define InsideMenu(xy,object)#+



.. index:: InsideTB

InsideTB()
----------

*Determine inside for a textbox-style object (location is upper left)*

**Description:**

Determines whether an ``[x,y]`` point is inside an object having .x, .y, .width, and .height properties, with .x and .y representing the upper left corner of the object.  This is bound to the .inside property of many custom ui objects.  The ``Inside`` function will use the function bound to the .inside property for any custom object having that property, and so this function's use is mainly hidden from users.

**Usage:**

.. code-block:: pebl

   define InsideTB(...)

**Example:**

.. code-block:: pebl

   
      pulldown <- MakePulldown(["one","two","three","four"],400-75,300,gWin,12,150,1)
   
      if(InsideTB([300,300],pulldown))
       {
         Print("INSIDE")
        }
   

**See Also:**

:func:`Inside()`, :func:`MoveObject()`, :func:`ClickOn()`, :func:`DrawObject()`



.. index:: MakeButton

MakeButton()
------------

*Makes a button for clicking on.*

**Description:**

Creates a button on a window that can be clicked and launches actions. The button is always 20 pixels high (using images in media images), with a rounded grey background.  The label text will be shrunk to fit the width, although this should be avoided as it can look strange. A button is a custom object made from images and text. It has a property 'clickon' that is bound to 'PushButton'  A button will look like this: **Usage:**

.. code-block:: pebl

   define MakeButton(...)

**Example:**

.. code-block:: pebl

   The following creates a button, waits for you to click on it, and animates a button press
   
   
    done <- MakeButton("QUIT",400,250,gWin,150)
    resp <- WaitForClickOntarget([done],[1])
    CallFunction(done.clickon,[done,gClick]) 
   
   

**See Also:**

:func:`PushButton()`, :func:`MakeCheckBox()`



.. index:: MakeMenu

MakeMenu()
----------

*Creates menu with suboptions.*

**Description:**

Creates a menu containing multiple menu items, that automatically call functions specified by the command.  

**Usage:**

.. code-block:: pebl

   define MakeMenu(...)

**Example:**

.. code-block:: pebl

   This creates a menu and awaits clicking on.  More complete examples are available in ui.pbl.  It requires that MyMessage is created somewhere
   
   
      menu1 <- MakeMenu("File",0,0,gWin,14,10,
                 ["Open","Save","Save as","Quit"],
                 ["MYMESSAGE","MYMESSAGE","MYMESSAGE","MYMESSAGE"])
   
   
      menu2<- MakeMenu("Edit",70,0,gWin,14,10,    
                 ["Cut","Copy","Paste","Select"],
                 ["MYMESSAGE","MYMESSAGE","MYMESSAGE","MYMESSAGE"])
   
      menu <- [menu1,menu2]
      opt <- WaitForClickOntarget(menu,[1,2])
      ClickOnMenu(Nth(menu,opt),gClick)
   

**See Also:**

:func:`MakeMenuItem()`, :func:`OpenSubMenus()`, :func:`ClickOnMenu()`



.. index:: MakeMenuItem

MakeMenuItem()
--------------

*Creates menu sub-item.*

**Description:**

Creates a single menu containing a label, whose .clickon property is bound to some other function.

**Usage:**

.. code-block:: pebl

   define MakeMenuItem(...)

**Example:**

.. code-block:: pebl

   This creates a menu and awaits clicking on.  More complete examples are available in ui.pbl.  It requires that MyMessage is created somewhere
   
   
      menu1 <- MakeMenuItem("File",0,0,gWin,14,10,"MYMESSAGE")
   
   
      menu2<- MakeMenu("Edit",70,0,gWin,14,10, "MYMESSAGE")
      
      menus <- [menu1,menu2]
      opt <- WaitForClickOntarget(menu,[1,2])
      ClickOnMenu(Nth(menus,opt),gClick)
   

**See Also:**

:func:`MakeMenu()`, :func:`OpenSubMenus()`, ``ClickOnMenu``



.. index:: MakeScrollBox

MakeScrollBox()
---------------

*Make a scrolling selection box.*

**Description:**

Creates a graphical object that displays and allows selection of a list of items, and scrolls if the text gets too big.   It has a property 'clickon' that is bound to 'ClickOnScrollBox'  A Scrolling textbox looks like this: **Usage:**

.. code-block:: pebl

   define MakeScrollBox(...)

**Example:**

.. code-block:: pebl

   See ui.pbl in the demo directory for examples of the use of a scrolling text box
   
   
   
     sb <- MakeScrollBox(Sequence(1,50,1),"The numbers",40,40,gWin,12,150,500,3)
   
      Draw()
      resp <- WaitForClickOntarget([sb],[1])
      CallFunction(sb.clickon,[sb,gClick])
      #Alternately: ClickOnScrollbox(sb,gClick) 
   

**See Also:**

``SetScrollingText``
``MakeScrollingTextBox``
``UpdateScrollBox``
``DrawScrollBox``
``ClickOnScrollBox``



.. index:: MakeScrollingTextBox

MakeScrollingTextBox()
----------------------

*Make a box for text that can be scrolled if too long.*

**Description:**

Creates a graphical object that displays a block of text, and scrolls if the text gets too big. It uses a ``Scrollbox`` as its base, but handles parsing the text into lines and hides the selection box.  Thus, no 'selection' is displayed (although it actually exists), and a .text property is added to hold the text being displayed.  It has a property 'clickon' that is bound to 'ClickOnScrollBox'  A Scrolling textbox looks like this: **Usage:**

.. code-block:: pebl

   define MakeScrollingTextBox(...)

**Example:**

.. code-block:: pebl

   See ui.pbl in the demo directory for examples of the use of a scrolling text box
   
   
   
     textscroll <- MakeScrollingTextBox("",200,50,gWin,12,
                                           300,150,0)
   
     SetScrollingText(textscroll,FileReadText("Uppercase.txt"))
      Draw()
     resp <- WaitForClickOntarget([textscroll],[1])
      CallFunction(textscroll.clickon,[textscroll,gClick]) 
   

**See Also:**

``SetScrollingText``
``MakeScrollBox``
``UpdateScrollBox``
``DrawScrollBox``
``ClickOnScrollBox``



.. index:: MakeTextList

MakeTextList()
--------------

*Creates a text body from a list.*

**Description:**

This takes a list and creates a block of text with carriage returns, ensuring each item of the list is on its own line; it also requires an offset, skipping the first lines of the list.  It is mostly a helper function used by ``Scrollbox`` objects to help format.  It will make text out of the entire list, so  you should be sure to cut off the end for efficiency if you only want to display some of the lines.

**Usage:**

.. code-block:: pebl

   define MakeTextList(...)

**Example:**

.. code-block:: pebl

   	
   letters <- FileReadList("Uppercase.txt")
   out <- MakeTextList(letters,20,"--")
   
   The above code will create the following:
   
   --u
   --v
   --w
   --x
   --y
   --z
   

**See Also:**

``ListToString``



.. index:: OpenSubMenus

OpenSubMenus()
--------------

*Opens the sub-menus of a menu.*

**Description:**

Used by ClickOnMenu to open, display a submenu and get a click.

**Usage:**

.. code-block:: pebl

   define OpenSubMenus(...)

**Example:**

.. code-block:: pebl

   This creates a menu and awaits clicking on.  More complete examples are available in ui.pbl.  It requires that MyMessage is created somewhere
   
   
      menu1 <- MakeMenuItem("File",0,0,gWin,14,10,"MYMESSAGE")
   
   
      menu2<- MakeMenu("Edit",70,0,gWin,14,10, "MYMESSAGE")
      
      menus <- [menu1,menu2]
      opt <- WaitForClickOntarget(menu,[1,2])
      ClickOnMenu(Nth(menus,opt),gClick)
   

**See Also:**

:func:`MakeMenu()`, :func:`OpenSubMenus()`, :func:`MakeMenuItem()`



.. index:: PopUpEntryBox

PopUpEntryBox()
---------------

**Description:**

Creates a small text-entry box at a specified location..

**Usage:**

.. code-block:: pebl

   define PopUpEntryBox(...)

**Example:**

.. code-block:: pebl

   
    subnum <- PopUpEntryBox("Enter particpant code",gWin,[100,100])
    
   

**See Also:**

``MessageBox`` ``GetEasyInput``, ``PopUpMessageBox``



.. index:: PushButton

PushButton()
------------

*Pushes a button and releases.*

**Description:**

Animates a button-pushing. It takes a button created using the MakeButton function and will animate a downclick when the mouse is down, and release when the mouse is unclicked.  To conform with general object handlers, it requires specifying a mouse click position, which could be [0,0], or gclick. This function is bound to the property 'clickon' of any button, allowing you to handle mouse clicks universally for many different objects.

**Usage:**

.. code-block:: pebl

   define PushButton(...)

**Example:**

.. code-block:: pebl

   The following creates a button, waits for you to click on it, and animates a button press
   
   
    done <- MakeButton("QUIT",400,250,gWin,150)
    resp <- WaitForClickOntarget([done],[1])
    PushButton(done,[0,0])
   
   
   To handle multiple buttons, you can do:
   
   
    done <- MakeButton("QUIT",400,250,gWin,150)
    ok <-   MakeButton("OK",400,250,gWin,150)
   
    resp <- 2
    while (resp != 1)
    {
     Draw()
     resp <- WaitForClickOntarget([done,ok],[1,2])
     obj <- Nth([done,ok],resp)
     CallFunction(obj.clickon,[obj,gClick])
    } 
    
   

**See Also:**

:func:`MakeCheckBox()`



.. index:: SetScrollingText

SetScrollingText()
------------------

*Changes text of a scrolling textbox.*

**Description:**

This updates the text in a ``ScrollingTextBox``. Because text must be parsed to be put into the box, you cannot just update the .text property, but instead should use this function.

**Usage:**

.. code-block:: pebl

   define SetScrollingText(...)

**Example:**

.. code-block:: pebl

   See ui.pbl in the demo directory for examples of the use of a scrolling text box.  A brief example follows:
   
   
     textscroll <- MakeScrollingTextBox("",200,50,gWin,12,
                                           300,150,0)
   
     SetScrollingText(textscroll,FileReadText("Uppercase.txt"))
      Draw()
     resp <- WaitForClickOntarget([textscroll],[1])
      CallFunction(textscroll.clickon,[textscroll,gClick]) 
   

**See Also:**

``MakeScrollingTextBox``
``MakeScrollBox``
``UpdateScrollBox``
``DrawScrollBox``
``ClickOnScrollBox``



.. index:: SetTextBoxCursorFromClick

SetTextBoxCursorFromClick()
---------------------------

**Description:**

this is used directly by a compiled function GetInput0
to reset the cursor position in any getinput thing.

**Usage:**

.. code-block:: pebl

   define SetTextBoxCursorFromClick(box, exit, click)



.. index:: SortDir

SortDir()
---------

**Description:**

This sorts the directory by subdirs
then alphabetically.

**Usage:**

.. code-block:: pebl

   define SortDir(inlist,path)



.. index:: UpdatePulldown

UpdatePulldown()
----------------

*Updates the list of a pulldown.*

**Description:**

This changes the list being used in a Pulldown object.  It tries to maintain the same selected option (matching the text of the previous selection), but if not found will select index 1. It calls ``DrawPullDown`` when complete, but a ``Draw()`` command must be issued before the pulldown changes will appear.

**Usage:**

.. code-block:: pebl

   define UpdatePulldown(...)

**Example:**

.. code-block:: pebl

   
    options  <- MakePulldownButton(["A",B","C"],400,250,gWin,14,100,3)
    Draw()
    WaitForAnyKeyPress()
    
    ##This should add a fourth option but C should still be selected.
    UpdatePullDown(options,["A","B","C","D"])
    Draw()
    WaitForAnyKeyPress()
   
   

**See Also:**

:func:`MakePullDown()`, :func:`Pulldown()`, :func:`DrawPulldown()`



.. index:: UpdateScrollbox

UpdateScrollbox()
-----------------

*Recalculates scrollbox layout.*

**Description:**

This updates the layout of a ``ScrollBox``. It should be used if you manually change the .list or .listoffset properties.  It won't actually redraw the scrollbox (which is done by DrawScrollbox).

**Usage:**

.. code-block:: pebl

   define UpdateScrollbox(...)

**Example:**

.. code-block:: pebl

   See ui.pbl in the demo directory for examples of the use of a scrolling text box.  A brief example follows:
   
   
      sb <- MakeScrollBox(Sequence(1,50,1),"The numbers",40,40,gWin,12,150,500,3)
      Draw()
   	  
      resp <- WaitForClickOntarget([sb],[1])
      CallFunction(sb.clickon,[sb,gClick])
      #Alternately: ClickOnScrollbox(sb,gClick) 
    
      ##change the selected items
      sb.list <- Sequence(sb.selected,sb.selected+50,1)
      UpdateScrollbox(sb)      
      DrawScrollbox(sb)
      Draw()
    
   

**See Also:**

``MakeScrollingTextBox``
``MakeScrollBox``
``DrawScrollBox``
``ClickOnScrollBox``
