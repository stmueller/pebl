================================================================================
PEBL Example Syntax
================================================================================

This page demonstrates the PEBL syntax highlighter.

Basic Example
=============

.. code-block:: pebl

   ## Simple PEBL program
   define Start(p)
   {
       # Create a window
       gWin <- MakeWindow()

       # Variables
       numTrials <- 10
       accuracy <- 0.95
       gSubNum <- GetSubNum(gWin)

       # Conditionals (must use parentheses)
       if (gSubNum > 0)
       {
           Print("Starting experiment for subject " + gSubNum)
       }

       # Loops
       loop(i, Sequence(1, numTrials, 1))
       {
           rt <- RunTrial(i)
           FilePrint(gFileOut, gSubNum + "," + i + "," + rt)
       }
   }

Function Definitions
====================

.. code-block:: pebl

   define CalculateAccuracy(hits, total)
   {
       accuracy <- hits / total
       return accuracy
   }

Built-in Functions
==================

.. code-block:: pebl

   # Math functions
   x <- Sqrt(100)
   y <- Round(3.14159, 2)
   z <- Random()

   # List operations
   myList <- [1, 2, 3, 4, 5]
   shuffled <- Shuffle(myList)
   mean <- Mean(myList)

   # String operations
   text <- "Hello World"
   lower <- Lowercase(text)
   parts <- SplitString(text, " ")

Comparison Operators
====================

.. code-block:: pebl

   if (x == 10)
   {
       Print("Equal")
   }

   if (y >= 5 and z < 100)
   {
       Print("In range")
   }

   if (not IsKeyDown("<space>"))
   {
       Wait(100)
   }

Assignment and Arithmetic
==========================

.. code-block:: pebl

   # Assignment
   result <- 0

   # Arithmetic
   sum <- 10 + 20
   diff <- 100 - 25
   product <- 5 * 6
   quotient <- 100 / 4
   power <- 2 ^ 8
