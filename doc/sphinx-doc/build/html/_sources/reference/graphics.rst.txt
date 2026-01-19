================================================================================
Graphics Library - Advanced Graphics
================================================================================

This library contains advanced graphics functions for creating complex visual stimuli and shapes.

.. contents:: Function Index
   :local:
   :depth: 0



.. index:: BlockE

BlockE()
--------

*Creates a block E as a useable polygon which can be added to a window directly.*

**Description:**

Creates a polygon in the shape of a  block E, pointing in one of four directions. Arguments include position in window. 

- ``<x>`` and ``<y>`` is the position of the center
- ``<h>`` and ``<w>`` or the size of the E in pixels
- ``<thickness>`` thickness of the E
- ``<direction>`` specifies which way the E points:  1=right,   2=down, 3=left, 4=up.
- ``<color>`` is a color object (not just the name)

  Like other drawn objects, the Block E must then be added to the window to appear.

**Usage:**

.. code-block:: pebl

   define BlockE(...)

**Example:**

.. code-block:: pebl

   
     win <- MakeWindow()
     e1 <- BlockE(100,100,40,80,10,1,MakeColor("black"))
     AddObject(e1,win)
     Draw()
   

**See Also:**

:func:`Plus()`, :func:`Polygon()`, :func:`MakeStarPoints()`,
:func:`MakeNGonPoints()`



.. index:: ConvexHull

ConvexHull()
------------

*Returns a convex subset of points for a set*

**Description:**

Computes the convex hull of a set of [x,y]   points. It returns a set of points that forms the convex hull, with   the first and last point identical.  A convex hull is the set of   outermost points, such that a polygon connecting just those points   will encompass all other points, and such that no angle is acute.   It is used in MakeAttneave.

**Usage:**

.. code-block:: pebl

   define ConvexHull(...)

**Example:**

.. code-block:: pebl

    
   pts <- [[0.579081, 0.0327737], 
            [0.0536094, 0.378258], 
            [0.239628, 0.187751], 
            [0.940625, 0.26526], 
            [0.508748, 0.840846],
            [0.352604, 0.200193], 
            [0.38684, 0.212413],
            [0.00114761, 0.768165],
            [0.432963, 0.629412]]
     Print(ConvexHull(pts))
   
   
   
   output:
   
   [[0.940625, 0.26526]
   , [0.508748, 0.840846]
   , [0.00114761, 0.768165]
   , [0.0536094, 0.378258]
   , [0.239628, 0.187751]
   , [0.579081, 0.0327737]
   , [0.940625, 0.26526]
   
   

**See Also:**

``MakeAttneave``,



.. index:: GetAngle

GetAngle()
----------

*Returns the angle in degrees of a vector.*

**Description:**

Gets  an angle (in degrees) from (0,0) of an x,y coordinate

**Usage:**

.. code-block:: pebl

   define GetAngle(...)

**Example:**

.. code-block:: pebl

   
     ##point sprite in the direction of a click   
     sprite <- LoadImage("car.png")
     AddObject(sprite,gWin)
     Move(sprite,300,300)
     xy <- WaitForDownClick()
     newangle <- GetAngle(First(xy)-300,Second(xy)-300)
     sprite.rotation <- newangle
     Draw()
   

**See Also:**

``DegtoRad``, ``RadToDeg``



.. index:: GetAngle3

GetAngle3()
-----------

*Gets angle abc.*

**Description:**

Gets  an angle (in radians) of abc.

**Usage:**

.. code-block:: pebl

   define GetAngle3(...)

**Example:**

.. code-block:: pebl

   
      a <- [0.579081, 0.0327737]
      b <- [0.0536094, 0.378258]
      c <- [0.239628, 0.187751]
   
     Print(GetAngle3(a,b,c)) ## .2157
   
   

**See Also:**

``DegtoRad``, ``RadToDeg``, ``GetAngle``, ``ToRight``



.. index:: KaniszaPolygon

KaniszaPolygon()
----------------

**Description:**

Creates generic polygon, defined only by with ``pac-man`` circles at specified vertices.

**Usage:**

.. code-block:: pebl

   define KaniszaPolygon(...)

**Example:**

.. code-block:: pebl

   For detailed usage example, see: `http://peblblog.blogspot.com/2010/11/kanizsa-shapes.html`
   Part of a script using KaniszaPolygon:	
   
      #Specify the xy points
      xys <- [[10,10],[10,50],[130,60],[100,100],[150,100],
              [150,20],[80,-10],[45,10]]
       
       #Specify which vertices to show (do all)
       show <- [1,1,1,1,1,1,1,1]
        
       #Make one, showing the line
       x <-  KaniszaPolygon(xys,show,10,fg,bg,1)
       AddObject(x,gWin);   Move(x,200,200)
   
       #Make a second, not showing the line
       x2 <-  KaniszaPolygon(xys,show,10,fg,bg,0)
       AddObject(x2,gWin);   Move(x2,400,200)
   
       #Make a third, only showing some vertices:
       x3 <-  KaniszaPolygon(xys,[1,1,1,1,1,0,0,1],10,fg,bg,0)
       AddObject(x3,gWin);  Move(x3,600,200)
           
   

**See Also:**

:func:`Polygon()`, :func:`KaneszaSquare()`



.. index:: KaniszaSquare

KaniszaSquare()
---------------

**Description:**

Creates generic Kanesza Square, one defined only by with ``pac-man`` circles at its vertices:    

**Usage:**

.. code-block:: pebl

   define KaniszaSquare(...)

**Example:**

.. code-block:: pebl

   For detailed usage example, see
   `http://peblblog.blogspot.com/2010/11/kanizsa-shapes.html`
   	
   
   
      gWin <- MakeWindow()
      square <- KaniszaSquare(150,20,MakeColor("red"),
                                     MakeColor("green"))
      AddObject(square,gWin)
      Move(square,200,200)
      Draw()
      WaitForAnyKeyPress()
   
   

**See Also:**

:func:`Polygon()`, :func:`KaneszaPolygon()`



.. index:: LayoutGrid

LayoutGrid()
------------

*Creates [x,y] pairs in a grid for graphical layout*

**Description:**

Creates a grid of x,y points in a range, that are  spaced in a specified number of rows and columns.  Furthermore, you can specify whether they are vertical or horizontally laid out.

**Usage:**

.. code-block:: pebl

   define LayoutGrid(...)

**Example:**

.. code-block:: pebl

   Example PEBL Program using NonoverlapLayout:
   
   define Start(p)
   {
      gWin <- MakeWindow()
      gVideoWidth <- 800
      gVideoHeight <- 300
   
      lab1 <- EasyLabel("LayoutGrid, horizontal",
                        200,25,gWin,24)
      lab2 <- EasyLabel("LayoutGrid, vertical",
                        600,25,gWin,24)
      nums <- Sequence(1,20,1)
      stim1 <- []
      stim2 <- []
   
      font <- MakeFont(gPeblBaseFont,0,25,
                 MakeColor("black"),MakeColor("white"),0)
      loop(i,nums)
      {
        stim1 <- Append(stim1,MakeLabel(i+"",font))
        stim2 <- Append(stim2,MakeLabel(i+"",font))
       }
   
     layout1 <- LayoutGrid(50,gVideoWidth/2-50,
                          50,gVideoHeight-50,5,4,0)
     layout2 <- LayoutGrid(gVideoWidth/2+50,gVideoWidth-50,
                          50,gVideoHeight-50,5,4,1)
   
   
     ##Now, layout the stuff.
   
     loop(i,Transpose([stim1,layout1]))
      {	
         obj <- First(i)
         xy <- Second(i)
         AddObject(obj,gWin)
         Move(obj, First(xy),Second(xy))
      }
   
     loop(i,Transpose([stim2,layout2]))
      {	
         obj <- First(i)
         xy <- Second(i)
         AddObject(obj,gWin)
         Move(obj, First(xy),Second(xy))
      }
   
     Draw()
     WaitForAnyKeyPress()
   }
   
   
   The output of the above program is shown below.  Even for the left configuration, which is too compact (and which takes a couple seconds to run), the targets are fairly well distributed.
   

**See Also:**

:func:`NonOverlapLayout()`



.. index:: MakeAttneave

MakeAttneave()
--------------

*Makes a complex ``Attneave'' polygon*

**Description:**

Makes a random 'Attneave' figure((Collin, C. A., \& Mcmullen, P. A. (2002). Using Matlab to generate  families of similar Attneave shapes. Behavior Research Methods  Instruments and Computers, 34(1), 55-68.).). An Attneave figure is a complex polygon that can be used as a stimulus in a number of situations.  It returns a sequence of points for use in Polygon().  {} MakeAttneave uses ConvexHull,  InsertAttneavePointRandom() and ValidateAttneaveShape(), found in Graphics.pbl.  Override these to change constraints such as  minimum/maximum side lengths, angles, complexity, etc.  MakeAttneave uses a sampling-and-rejection scheme to create in-bounds shapes.  Thus, if you specify impossible or nearly-impossible constraints, the time necessary to create shapes may be very long or infinite.   The arguments to MakeAttneave are: 

- size: size, in pixels, of a circle from which points are   sampled in a uniform distribution.
- numpoints: number of points in the polygon.
- minangle: smallest angle acceptable (in degrees).
- maxangle: largest angle acceptable  (in degrees).


**Usage:**

.. code-block:: pebl

   define MakeAttneave(...)

**Example:**

.. code-block:: pebl

   
     gWin <- MakeWindow()
     shape <- MakeAttneave(100,5+RandomDiscrete(5),5,170)
     pts <- Transpose(shape)
     poly <- Polygon(200,200,First(pts),Second(pts),
                     MakeColor("blue"),1)
     AddObject(poly,gWin)
     Draw()
     WaitForAnyKeyPress()
   

**See Also:**

:func:`MakeImage()`, :func:`Polygon()`, :func:`Square()`



.. index:: MakeGabor

MakeGabor()
-----------

*Creates a 'gabor patch' with specified parameters*

**Description:**

Creates a greyscale gabor patch, with seven variables: 

- size (in pixels) of square the patch is drawn on
- freq: frequency of grating (number of wavelengths in size)
- sd: standard deviation, in pixels, of gaussian window
- angle: angle of rotation of grating, in radians
- phase: phase offset of grating (in radians)
- bglev: number between 0 and 255 indicating background color in greyscale.

  { }

**Usage:**

.. code-block:: pebl

   define MakeGabor(...)

**Example:**

.. code-block:: pebl

   
      win <- MakeWindow()
      patch <- MakeGabor(80, 0,10,0,0,100)
      AddObject(patch,win)
      Move(patch,200,200)
      Draw()
      
   

**See Also:**

:func:`MakeAttneave()`, :func:`SetPixel()`, :func:`MakeCanvas()`



.. index:: MakeGraph

MakeGraph()
-----------

**Description:**

Creates a simple bargraph that can be added to/moved on a window..

**Usage:**

.. code-block:: pebl

   define MakeGraph(...)



.. index:: MakeNGonPoints

MakeNGonPoints()
----------------

*Creates points for a polygon, which can then be fed to Polygon*

**Description:**

Creates a set of points that form a regular n-gon.  It can be transformed with functions like ``RotatePoints``, or it can be  used to create a graphical object with ``Polygon``.  Note: ``MakeNGonPoints`` returns a list like: 

.. code-block:: text

   [[x1, x2, x3,...],[y1,y2,y3,...]],

 while Polygon() takes the X and Y lists independently.

**Usage:**

.. code-block:: pebl

   define MakeNGonPoints(...)

**Example:**

.. code-block:: pebl

   
      window <- MakeWindow()
      ngonp <- MakeNGonPoints(50,10)
      ngon <- Polygon(200,200,First(ngonp),Nth(ngonp,2),
                      MakeColor("red"),1)
      AddObject(ngon,window)
      Draw()
   

**See Also:**

``MakeStarPoints``, ``Polygon``, ``RotatePoints``, ``ZoomPoints``



.. index:: MakeStarPoints

MakeStarPoints()
----------------

*Creates points for a star, which can then be fed to Polygon*

**Description:**

Creates a set of points that form a regular star.  It can be transformed with functions like ``RotatePoints``, or it can be  used to create a graphical object with ``Polygon``.  Note: ``MakeStarPoints`` returns a list: 

.. code-block:: text

   [[x1, x2, x3,...],[y1,y2,y3,...]],

 while ``Polygon()`` takes the X and Y lists independently.

**Usage:**

.. code-block:: pebl

   define MakeStarPoints(...)

**Example:**

.. code-block:: pebl

   
      window <- MakeWindow()
      sp <- MakeStarPoints(50,20,10)
      star <- Polygon(200,200,First(sp),Nth(sp,2),
                      MakeColor("red"),1)
      AddObject(star,window)
      Draw()
   

**See Also:**

``MakeNGonPoints``, ``Polygon``, ``RotatePoints``, ``ZoomPoints``



.. index:: NonOverlapLayout

NonOverlapLayout()
------------------

*Creates a set of num points that don't overlap, but fails gracefully*

**Description:**

Creates a set of num points in a xy range, that have a (soft) minimum tolerance of 'tol' between points.  That is, to the extent possible, the returned points will have a minumum distance between them of ``<tol>``.  This may not be possible or be very difficult, and so after a limited number of attempts (by default, 100), the algorithm will return the current configuration, which may have some violations of the minimum tolerance rule, but it will usually be fairly good.    The algorithm works by initializing with a random set of points, then computing a pairwise distance matrix between all points, finding the closest two points, and resampling one of them until its minumum distance is larger than the current.  Thus, each internal iteration uniformly improves (or keeps the configuration the same), and the worst points are reconfigured first, so that even if a configuration that does not satisfy the constraints, it will usually be very close.  Internally, the function (located in pebl-lib/Graphics.pbl) has a variable that controls how many steps are taken, called ``limit``, which is set to 100.  For very compacted or very large iterations, this limit can be increased by editing the file or making a copy of the function.    The function usually returns fairly quickly, so it can often be used real-time between trials.  However, for complex enough configurations, it can take on the order of seconds; furthermore, more  complex configurations might take longer than less complex configurations, which could represent a potential confound (if more complex stimuli have longer ISIs).  Users should thus consider creating the configurations when the test is initialized, or created prior to the study and then saved out to a file for later use.   \newpage

**Usage:**

.. code-block:: pebl

   define NonOverlapLayout(...)

**Example:**

.. code-block:: pebl

   Example PEBL Program using NonoverlapLayout:
   
   define Start(p)
    {
      win <- MakeWindow()  
      ## Make 25 points in a square in the middle 
      ## of the screen, a minimum of 50 pixels apart.  
      ## This is too compact, but it will be OK.
   
      points <- NonOverlapLayout(100,300,200,400,50,25)
      circs <- []
      ##This should non-overlapping circles of radius 25
      loop(i,points)
       {
          tmp <- Circle(First(i),Second(i),25,
                        MakeColor("blue"),0) 
          AddObject(tmp,win)
          circs <- Append(circs,tmp)
       }
   
   
      rect1 <- Square(200,300,200,MakeColor("black"),0)
      rect2 <- Square(600,300,200,MakeColor("black"),0)
   
      AddObject(rect1,win)
      AddObject(rect2,win)
      ##Reduce the tolerance: this one should be bettter
      points <- NonOverlapLayout(500,700,200,400,50,15)
   
   
      ##This should non-overlapping circles of radius 15
      loop(i,points)
       {
          tmp <- Circle(First(i),Second(i),
                        15,MakeColor("blue"),0) 
          AddObject(tmp,win)
   	   circs <- Append(circs,tmp)
       }
      Draw()
      WaitForAnyKeyPress()
   
   }
   
   The output of the above program is shown below.  Even for the left configuration, which is too compact (and which takes a couple seconds to run), the targets are fairly well distributed.
   

**See Also:**

:func:`LayoutGrid()`



.. index:: Plus

Plus()
------

*Creates a plus sign as a useable polygon which can be added to a window directly.*

**Description:**

Creates a polygon in the shape of a  plus sign. Arguments include position in window. 

- ``<x>`` and ``<y>`` is the position of the center
- ``<size>`` or the size of the plus sign in pixels
- ``<width>`` thickness of the plus
- ``<color>`` is a color object (not just the name)

  Like other drawn objects, the plus must then be added to the window to appear.

**Usage:**

.. code-block:: pebl

   define Plus(...)

**Example:**

.. code-block:: pebl

   
     win <- MakeWindow()
     p1 <- Plus(100,100,80,15,MakeColor("red"))
     AddObject(p1,win)
     Draw()
   

**See Also:**

:func:`BlockE()`, :func:`Polygon()`, :func:`MakeStarPoints()`,
:func:`MakeNGonPoints()`



.. index:: ReflectPoints

ReflectPoints()
---------------

*Reflects points on vertical axis*

**Description:**

Takes a set of points (defined in a joined list  [[x1,x2,x3,...],[y1,y2,y3,...]] and reflects them around the vertical axis x=0, returning a similar [[x],[y]] list.  Identical to ``ZoomPoints(pts,-1,1)``

**Usage:**

.. code-block:: pebl

   define ReflectPoints(...)

**Example:**

.. code-block:: pebl

   
     points <- [[1,2,3,4],[20,21,22,23]]
     newpoints <- ReflectPoints(points)
   

**See Also:**

:func:`ZoomPoints()`, :func:`RotatePoints()`



.. index:: ResetCanvas

ResetCanvas()
-------------

*Resets a canvas to its background, erasing anything drawn on the canvas*

**Description:**

Resets a canvas, so that anything drawn onto it is   erased and returned to its background color.  Implemented by   resetting the background color to itself:  

.. code-block:: text

   canvas.color <- canvas.

  The function does not return the canvas,   but has the side effect of resetting it.

**Usage:**

.. code-block:: pebl

   define ResetCanvas(...)

**Example:**

.. code-block:: pebl

   
   
   #create a canvas, add pixel noise, then reset and repeat.
   define Start(p)
   {
     gWin <- MakeWindow()
     canvas <- MakeCanvas(100,100,MakeColor("black"))
     AddObject(canvas,gWin); Move(canvas,300,300)
     Draw()
     white <- MakeColor("white")
     ##add pixel noise
     j <- 1
     while(j < 5)
      {
     i <- 1
     while(i < 200)
      {
        SetPixel(canvas,Round(Random()*100),
                 Round(Random()*100),white)
        i <- i +1 
      }
     Draw()
     WaitForAnyKeyPress()
     ResetCanvas(canvas)
     Draw()
      j <- j + 1
     }
     WaitForAnyKeyPress()
   
   }
   

**See Also:**

:func:`SetPixel()`, :func:`MakeCanvas()`, :func:`Draw()`



.. index:: RGBtoHSV

RGBtoHSV()
----------

*Converts a color to HSV triple*

**Description:**

Converts a color object to HSV values.  May be useful for computing color-space distances an so on.  No HSVtoRGB is currently implemented.

**Usage:**

.. code-block:: pebl

   define RGBtoHSV(...)

**Example:**

.. code-block:: pebl

   
   
   x <- RGBtoHSV(MakeColor("red))
   
   

**See Also:**

:func:`MakeColor()`, :func:`MakeColorRGB()`



.. index:: RotatePoints

RotatePoints()
--------------

**Description:**

Takes a set of points (defined in a joined list  ``[[x1,x2,x3,...],`` ``[y1,y2,y3,...]]`` and rotates them ``<angle>`` degrees around the point ``[0,0]``,  returning a similar ``[[x],[y]]`` list.

**Usage:**

.. code-block:: pebl

   define RotatePoints(...)

**Example:**

.. code-block:: pebl

   
     points <- [[1,2,3,4],[20,21,22,23]]
     newpoints <- RotatePoints(points,10)
   

**See Also:**

:func:`ZoomPoints()`, :func:`ReflectPoints()`



.. index:: SegmentsIntersect

SegmentsIntersect()
-------------------

*Determines whether line segment ab intersects cd.*

**Description:**

Determines whether two line segments, defined by   four xy point pairs, intersect. Two line segments that share a   corner return 0, although they could be considered to intersect.  This function is defined in pebl-lib/Graphics.pbl

**Usage:**

.. code-block:: pebl

   define SegmentsIntersect(...)

**Example:**

.. code-block:: pebl

   
    SegmentsIntersect(1,0,2,0, 
                      1,2,2,2)  #0
   
    #returns 0, though they share (1,0)
    SegmentsIntersect(1,0,2,0,
                       1,0,2,2)  
    SegmentsIntersect(1,1,3,1,
                      2,2,2,0)  #1
   
   
   

**See Also:**

``GetAngle3``, ``ToRight``



.. index:: ToRight

ToRight()
---------

*Determines whether p3 is te the right of line p1p2*

**Description:**

Determines whether a point p3 is 'to the right'   of a line segment defined by p1  to p2.  Works essentially by   computing the determinant.

**Usage:**

.. code-block:: pebl

   define ToRight(...)

**Example:**

.. code-block:: pebl

   
     a <- [100,0]
     b <- [100,100]
     c <- [150,50]
     ToRight(a,b,c) # returns 1; true
     ToRight(b,a,c) # returns 0; false
   
   

**See Also:**

:func:`GetAngle()`, :func:`GetAngle3()`, :func:`SegmentsIntersect()`



.. index:: ZoomPoints

ZoomPoints()
------------

*Zooms a set of points in 2 directions*

**Description:**

Takes a set of points (defined in a joined list  [[x1,x2,x3,...],[y1,y2,y3,...]] and adjusts them in the x and y direction independently, returning a similar [[x],[y]] list.  Note: The original points should be centered at zero, because the get adjusted relative to zero, not relative to their center.

**Usage:**

.. code-block:: pebl

   define ZoomPoints(...)

**Example:**

.. code-block:: pebl

   
     points <- [[1,2,3,4],[20,21,22,23]]
     newpoints <- ZoomPoints(points,2,.5)
     ##Produces [[2,4,6,8],[10,11.5,11,11.5]]
   

**See Also:**

:func:`RotatePoints()`, :func:`ReflectPoints()`


Functions Pending Documentation
--------------------------------



.. index:: GetMinDist

GetMinDist()
------------

*Finds the minimum distance between any two points in a list*

**Description:**

Computes and returns the minimum distance between any pair of points in a list of [x,y] coordinate pairs. This function examines all possible pairs of points and returns the smallest distance found. Useful for validating point distributions, checking spacing constraints, or analyzing geometric layouts.

**Usage:**

.. code-block:: pebl

   define GetMinDist(pts)

**Example:**

.. code-block:: pebl


   points <- [[0, 0], [10, 0], [5, 5], [100, 100]]
   minDist <- GetMinDist(points)
   Print(minDist)
   # Result: 7.07107  (distance between [5,5] and [10,0])

   # Check if points are spaced far enough apart
   layout <- NonOverlapLayout(100, 400, 100, 400, 50, 20)
   spacing <- GetMinDist(layout)
   if(spacing < 50)
   {
      Print("Warning: some points are too close together")
   }


**See Also:**

:func:`NonOverlapLayout()`, :func:`Dist()`



.. index:: HideObject

HideObject()
------------

*Hides a custom graphical object*

**Description:**

Hides a custom graphical object by hiding its canvas. This function is designed for use with complex custom objects that have a ``.canv`` property containing their graphical representation. The object remains in memory but is not displayed until shown again with ``ShowObject()``. This is useful for temporarily removing custom stimuli from view without destroying them.

**Usage:**

.. code-block:: pebl

   define HideObject(obj)

**Example:**

.. code-block:: pebl


   # Create a custom object with a canvas
   myObject <- MakeCustomShape()  # hypothetical function

   # Display the object
   ShowObject(myObject)
   Draw()
   Wait(1000)

   # Hide the object temporarily
   HideObject(myObject)
   Draw()
   Wait(1000)

   # Show it again
   ShowObject(myObject)
   Draw()


**See Also:**

:func:`ShowObject()`, :func:`Hide()`, :func:`Show()`



.. index:: LandoltRing

LandoltRing()
-------------

*Creates a Landolt C ring stimulus for visual acuity testing*

**Description:**

Creates a Landolt C (also known as Landolt ring), a classic visual acuity stimulus used in optometry and vision research. The Landolt C is a ring with a gap at a specified angular position. Subjects identify the location of the gap to assess visual acuity. The function returns a canvas object containing the rendered stimulus.

The Landolt C was introduced by Edmund Landolt in 1888 (Landolt, E. Methode optométrique simple. *Bull Mem Soc Fran Ophtalmol.* 1888;6:213–4) and remains a standard stimulus in visual psychophysics and clinical vision testing.

Parameters:

- ``outer``: Outer diameter of the ring in pixels
- ``inner``: Inner diameter (size of the central hole) in pixels
- ``angle``: Angular position of the gap in degrees (0=right, 90=down, 180=left, 270=up)
- ``lgap``: Width of the gap in pixels
- ``color``: Color object for the ring
- ``bgcolor``: Color object for the background and gap

**Usage:**

.. code-block:: pebl

   define LandoltRing(outer, inner, angle, lgap, color, bgcolor)

**Example:**

.. code-block:: pebl


   gWin <- MakeWindow()
   black <- MakeColor("black")
   white <- MakeColor("white")

   # Create a Landolt C with gap on the right (0 degrees)
   ring1 <- LandoltRing(100, 60, 0, 20, black, white)
   AddObject(ring1, gWin)
   Move(ring1, 200, 200)

   # Create a smaller ring with gap at bottom (90 degrees)
   ring2 <- LandoltRing(60, 40, 90, 12, black, white)
   AddObject(ring2, gWin)
   Move(ring2, 400, 200)

   # Create rings at different orientations for acuity test
   angles <- [0, 45, 90, 135, 180, 225, 270, 315]
   loop(i, Sequence(1, Length(angles), 1))
   {
      ring <- LandoltRing(80, 50, Nth(angles, i), 15, black, white)
      AddObject(ring, gWin)
      Move(ring, 100 + i*80, 300)
   }

   Draw()
   WaitForAnyKeyPress()


**See Also:**

:func:`Circle()`, :func:`BlockE()`, :func:`MakeCanvas()`



.. index:: MakeTable

MakeTable()
-----------

*Creates a formatted table display with headers and data*

**Description:**

Creates a visual table object containing data arranged in rows and columns with headers. Returns a custom table object drawn on a canvas that can be added to a window and positioned like other graphical objects. The table includes horizontal rules separating the header from data and bordering the table. Headers can be multi-row (nested lists). Useful for displaying results, feedback, or structured information to participants.

Parameters:

- ``data``: Nested list of data values (rows × columns)
- ``header``: List of column headers (can be nested for multi-row headers)
- ``width``: Total width of the table in pixels
- ``height``: Total height of the table in pixels
- ``fontsize``: Base font size for table text
- ``fgcol``: Foreground color object for text and lines
- ``bgcolor``: Background color object for the table
- ``layout``: Layout mode (default: 1)

**Usage:**

.. code-block:: pebl

   define MakeTable(data, header, width, height, fontsize, fgcol, bgcol, layout: 1)

**Example:**

.. code-block:: pebl


   gWin <- MakeWindow()
   black <- MakeColor("black")
   white <- MakeColor("white")

   # Create a simple results table
   headers <- ["Trial", "RT (ms)", "Accuracy"]
   data <- [[1, 523, "Correct"],
            [2, 456, "Correct"],
            [3, 678, "Error"],
            [4, 501, "Correct"]]

   table <- MakeTable(data, headers, 400, 300, 16, black, white)
   AddObject(table.canv, gWin)
   Move(table.canv, 400, 300)
   Draw()
   WaitForAnyKeyPress()


**See Also:**

:func:`MakeCanvas()`, :func:`MakeLabel()`, :func:`EasyLabel()`



.. index:: ShowObject

ShowObject()
------------

*Shows a custom graphical object*

**Description:**

Shows a custom graphical object by displaying its canvas. This function is designed for use with complex custom objects that have a ``.canv`` property containing their graphical representation. Use this to display objects that were previously hidden with ``HideObject()`` or to initially display custom objects after creation. The object must have been created with a canvas property for this function to work properly.

**Usage:**

.. code-block:: pebl

   define ShowObject(obj)

**Example:**

.. code-block:: pebl


   # Create a custom object with a canvas
   myObject <- MakeCustomShape()  # hypothetical function

   # Initially hide the object
   HideObject(myObject)
   Draw()

   # Wait for user input
   WaitForAnyKeyPress()

   # Now show the object
   ShowObject(myObject)
   Draw()

   # Toggle visibility
   if(someCondition)
   {
      ShowObject(myObject)
   } else {
      HideObject(myObject)
   }


**See Also:**

:func:`HideObject()`, :func:`Show()`, :func:`Hide()`


Functions Under Investigation
------------------------------



.. index:: ThickLine2

ThickLine2()
------------

.. warning::
   **Under investigation.** This function's status is being reviewed.

**Usage:**

.. code-block:: pebl

   define ThickLine2(x1,y1,x2,y2,size,color)
