================================================================================
PEBLList - List Manipulation
================================================================================

This module contains functions for creating, manipulating, and querying lists.

.. contents:: Function Index
   :local:
   :depth: 0


.. index:: Append

Append()
--------

**Description:**

Appends an item to a list.  Useful for constructing lists in conjunction with the loop statement.  Note: ``Append()`` is useful, but inefficent for large data structures, because it requires making a copy of the entire data list and then overwriting it, if you use ``list <- Append(list, item)``.  The overhead will be hardly noticeable unless you are building lists hundreds of elements long.  In that case you shuold either create the list upfront and use ``SetElement``, or you ``PushOnEnd`` to modify the list directly.

**Usage:**

.. code-block:: pebl

   Append(<list>, <item>)

**Example:**

.. code-block:: pebl

   list <- Sequence(1,5,1)
   double  <- []
   loop(i, list)
   {
    double <- Append(double, [i,i])
   }
   Print(double)
   # Produces [[1,1],[2,2],[3,3],[4,4],[5,5]]

**See Also:**

:func:`SetElement()` :func:`List()`, :func:`[ ]`, :func:`Merge()`, :func:`PushOnEnd`


.. index:: CrossFactorWithoutDuplicates

CrossFactorWithoutDuplicates()
------------------------------

**Description:**

This function takes a single list, and returns a list of all  			pairs, excluding the pairs that have two of the same item.  			To achieve the same effect but include the duplicates, use: ``DesignFullCounterBalance(x,x)``.

**Usage:**

.. code-block:: pebl

   CrossFactorWithoutDuplicates(<list>)

**Example:**

.. code-block:: pebl

   CrossFactorWithoutDuplicates([a,b,c]) 
   # == [[a,b],[a,c],[b,a],[b,c],[c,a],[c,b]]

**See Also:**

:func:`DesignFullCounterBalance()`, :func:`Repeat()`, :func:`DesignBalancedSampling()`,  :func:`DesignGrecoLatinSquare()`, :func:`DesignLatinSquare()`,  :func:`RepeatList()`,    :func:`LatinSquare()`, :func:`Shuffle()`


.. index:: DesignFullCounterbalance

DesignFullCounterbalance()
--------------------------

**Description:**

This takes two lists as parameters, and returns a nested list  		of lists that includes the full counterbalancing of both  		parameter lists.  Use cautiously; this gets very large.

**Usage:**

.. code-block:: pebl

   DesignFullCounterbalance(<lista>, <listb>)

**Example:**

.. code-block:: pebl

   a <- [1,2,3]
   b <- [9,8,7]
   DesignFullCounterbalance(a,b)	# == [[1,9],[1,8],[1,7],
   				#     [2,9],[2,8],[2,7],
   				#     [3,9],[3,8],[3,7]]

**See Also:**

:func:`CrossFactorWithoutDuplicates()`,   :func:`LatinSquare()`, :func:`Shuffle()`,   :func:`DesignBalancedSampling()`, :func:`DesignGrecoLatinSquare()`,    :func:`DesignLatinSquare()`, :func:`Repeat()`, :func:`RepeatList()`,


.. index:: First

First()
-------

*Returns the first item in a list.*

**Description:**

Returns the first item of a list.

**Usage:**

.. code-block:: pebl

   First(<list>)

**Example:**

.. code-block:: pebl

   First([3,33,132])		# == 3

**See Also:**

:func:`Nth()`, :func:`Last()`


.. index:: IsMember

IsMember()
----------

**Description:**

Returns true if ``<element>`` is a member of ``<list>``.

**Usage:**

.. code-block:: pebl

   IsMember(<element>,<list>)

**Example:**

.. code-block:: pebl

   IsMember(2,[1,4,6,7,7,7,7])		# false
   IsMember(2,[1,4,6,7,2,7,7,7]) 		# true


.. index:: Last

Last()
------

*Returns the last item in a list.*

**Description:**

Returns the last item in a list. Provides faster  		access to the last item of a list than does Nth().

**Usage:**

.. code-block:: pebl

   Last(<list>)

**Example:**

.. code-block:: pebl

   Last([1,2,3,444])	# == 444

**See Also:**

:func:`Nth()`, :func:`First()`


.. index:: Length

Length()
--------

*Returns the number of elements in a list.*

**Description:**

Returns the number of items in a list.

**Usage:**

.. code-block:: pebl

   Length(<list>)

**Example:**

.. code-block:: pebl

   Length([1,3,55,1515])	# == 4

**See Also:**

:func:`StringLength()`


.. index:: List

List()
------

*Makes a list out of items*

**Description:**

Creates a list of items. Functional version of ``[]``.

**Usage:**

.. code-block:: pebl

   List(<item1>, <item2>, ....)

**Example:**

.. code-block:: pebl

   List(1,2,3,444)		# == [1,2,3,444]

**See Also:**

:func:`[ ]`, :func:`Merge()`, :func:`Append()`


.. index:: ListToString

ListToString()
--------------

**Description:**

Converts a list of things to a single string

**Usage:**

.. code-block:: pebl

   ListToString(<list>)

**Example:**

.. code-block:: pebl

   ListToString([1,2,3,444])		# == "123444"
   ListToString(["a","b","c","d","e"])		# == "abcde"

**See Also:**

:func:`SubString`, :func:`StringLength`, ``ConcatenateList``


.. index:: Merge

Merge()
-------

*Combines two lists.*

**Description:**

Combines two lists, ``<lista>`` and ``<listb>``, into a single list.

**Usage:**

.. code-block:: pebl

   Merge(<lista>,<listb>)

**Example:**

.. code-block:: pebl

   Merge([1,2,3],[8,9]) 	# == [1,2,3,8,9]

**See Also:**

:func:`[ ]`, :func:`Append()`, :func:`List()`


.. index:: ModList

ModList()
---------

*Adds pre- and post- elements to each list member*

**Description:**

Modifies each element of a list with a pre- and post- string. If the list item is not a string, it will use whatever string it turns into.  This creates a new list, so it could be used to make a copy of a string-based list.

**Usage:**

.. code-block:: pebl

   ModList(<list>,<pre>,<post>)
   	ModList(list,"<",">")  ##encloses each list item in brackets

**Example:**

.. code-block:: pebl

   ModList([1,2,3,444]," ","")	        
   	ModList(["a","b","c","d","e"],",","-")

**See Also:**

:func:`SubString`, :func:`StringLength`, ``FoldList``, 	``ConcatenateList``,


.. index:: Nth

Nth()
-----

*Returns the nth item in a list.*

**Description:**

Extracts the Nth item from a list.  Indexes from 1 upwards. 		``Last()`` provides faster access than ``Nth()`` to the end of a list,  		which must walk along the list to the desired position.

**Usage:**

.. code-block:: pebl

   Nth(<list>, <index>)

**Example:**

.. code-block:: pebl

   a <- ["a","b","c","d"]
   Print(Nth(a,3)) 		# == 'c'

**See Also:**

:func:`First()`, :func:`Last()`, :func:`Second()`, :func:`Third()`, :func:`Fourth()`, :func:`Fifth()`


.. index:: Second

Second()
--------

*Returns the second item in a list.*

**Description:**

Returns the second item of a list. Provides convenient access to the second element without using Nth().

**Usage:**

.. code-block:: pebl

   Second(<list>)

**Example:**

.. code-block:: pebl

   Second([3,33,132,200])		# == 33

**See Also:**

:func:`First()`, :func:`Third()`, :func:`Fourth()`, :func:`Fifth()`, :func:`Nth()`, :func:`Last()`


.. index:: Third

Third()
-------

*Returns the third item in a list.*

**Description:**

Returns the third item of a list. Provides convenient access to the third element without using Nth().

**Usage:**

.. code-block:: pebl

   Third(<list>)

**Example:**

.. code-block:: pebl

   Third([3,33,132,200])		# == 132

**See Also:**

:func:`First()`, :func:`Second()`, :func:`Fourth()`, :func:`Fifth()`, :func:`Nth()`, :func:`Last()`


.. index:: Fourth

Fourth()
--------

*Returns the fourth item in a list.*

**Description:**

Returns the fourth item of a list. Provides convenient access to the fourth element without using Nth().

**Usage:**

.. code-block:: pebl

   Fourth(<list>)

**Example:**

.. code-block:: pebl

   Fourth([3,33,132,200])		# == 200

**See Also:**

:func:`First()`, :func:`Second()`, :func:`Third()`, :func:`Fifth()`, :func:`Nth()`, :func:`Last()`


.. index:: Fifth

Fifth()
-------

*Returns the fifth item in a list.*

**Description:**

Returns the fifth item of a list. Provides convenient access to the fifth element without using Nth().

**Usage:**

.. code-block:: pebl

   Fifth(<list>)

**Example:**

.. code-block:: pebl

   Fifth([3,33,132,200,999])		# == 999

**See Also:**

:func:`First()`, :func:`Second()`, :func:`Third()`, :func:`Fourth()`, :func:`Nth()`, :func:`Last()`


.. index:: PushOnEnd

PushOnEnd()
-----------

**Description:**

Pushes an item onto the end of a list, modifying the list itself.  Note: ``PushOnEnd`` is a more efficient replacement for ``Append()``. Unlike ``Append``, it will modify the original list as a side effect, so the following works:  

.. code-block:: text

   PushOnEnd(list, item)

 There is no need to set the original list to the result of PushOnEnd, like you must do with Append.  However, it does in fact work, and incurs only a slight overhead, so that Append can often be replaced with PushOnEnd without worry. 

.. code-block:: text

   list <-  PushOnEnd(list, item)


**Usage:**

.. code-block:: pebl

   PushOnEnd(<list>, <item>)

**Example:**

.. code-block:: pebl

   list <- Sequence(1,5,1)
   double  <- []
   loop(i, list)
   {
     PushOnEnd(double, [i,i])
   }
   Print(double)
   # Produces [[1,1],[2,2],[3,3],[4,4],[5,5]]

**See Also:**

:func:`SetElement()` :func:`List()`, :func:`[ ]`, :func:`Merge()`, :func:`PushOnEnd`


.. index:: Repeat

Repeat()
--------

**Description:**

Makes and returns a list by repeating ``<object>`` ``<n>`` times.  		Has no effect on the object. Repeat will not make new copies  		of the object. If you later change the object,  		you will change every object in the list.

**Usage:**

.. code-block:: pebl

   Repeat(<object>, <n>)

**Example:**

.. code-block:: pebl

   x <- "potato"
   y <- repeat(x, 10)
   Print(y)
   # produces ["potato","potato","potato",
               "potato","potato", "potato",
               "potato","potato","potato","potato"]

**See Also:**

:func:`RepeatList()`


.. index:: RepeatList

RepeatList()
------------

**Description:**

Makes a longer list by repeating a shorter list ``<n>`` times.  	Has no effect on the list itself, but changes made to objects  	in the new list will also affect the old list.

**Usage:**

.. code-block:: pebl

   RepeatList(<list>, <n>)

**Example:**

.. code-block:: pebl

   RepeatList([1,2],3) # == [1,2,1,2,1,2]

**See Also:**

:func:`Repeat()`, :func:`Merge()`, :func:`[ ]`


.. index:: Rotate

Rotate()
--------

**Description:**

Returns a list created by rotating a list by ``<n>`` items.   		The new list will begin with the ``<n+1>``th item of the old  		list (modulo its length), and contain all of its items in  		order, jumping back to the beginning and ending with the ``<n>``th 		item. Rotate(``<list>``,0) has no effect.  Rotate does not modify  		the original list.

**Usage:**

.. code-block:: pebl

   Rotate(<list-of-items>, <n>)

**Example:**

.. code-block:: pebl

   Rotate([1,11,111],1)  # == [11,111,1]

**See Also:**

:func:`Transpose()`


.. index:: Sequence

Sequence()
----------

**Description:**

Makes a sequence of numbers from ``<start>`` to   ``<end>`` at ``<step>``-sized increments. If ``<step>`` is   positive, ``<end>`` must be larger than ``<start>``, and if   ``<step>`` is negative, ``<end>`` must be smaller than   ``<start>``. If ``<start> + n*<step>`` does not exactly equal   ``<end>``, the last item in the sequence will be the number   closest number to ``<end>`` in the direction of ``<start>``   (and thus ``<step>``).

**Usage:**

.. code-block:: pebl

   Sequence(<start>, <end>, <step>)

**Example:**

.. code-block:: pebl

   Sequence(0,10,3)    # == [0,3,6,9]
   Sequence(0,10,1.5)  # == [0,1.5,3,4.5, 6, 7.5, 9]
   Sequence(10,1,3)    # error
   Sequence(10,0,-1)   # == [10,9,8,7,6,5,4,3,2,1]

**See Also:**

:func:`Repeat()`, :func:`RepeatList()`


.. index:: SetElement

SetElement()
------------

*Sets an element of list to value*

**Description:**

Efficiently alter a specific item from a list.  ``SetElement`` has  length-constant access time, and so it can be efficient to pre-create a list structure and then populate it one-by-one.

**Usage:**

.. code-block:: pebl

   SetElement(<list>, <index>, <value>)

**Example:**

.. code-block:: pebl

   ##Set a random subset of elements to their index:
    list <- Repeat(0,10)
     index <- 1
     while(index <= 10)
     {
       if(Random()<.2)
        {
           SetElement(list,index,index)
         }
       index <- index + 1
      }

**See Also:**

:func:`Nth()`, :func:`Append()`, :func:`PushOnEnd()`


.. index:: Shuffle

Shuffle()
---------

*Returns a new list with the items in list shuffled randomly.*

**Description:**

Randomly shuffles a list.

**Usage:**

.. code-block:: pebl

   Shuffle(list)

**Example:**

.. code-block:: pebl

   Print(Shuffle([1,2,3,4,5]))
   # Results might be anything, like [5,3,2,1,4]

**See Also:**

:func:`Sort()`, :func:`SortBy()`, :func:`ShuffleRepeat()`,                     :func:`ShuffleWithoutAdjacents()`


.. index:: Sort

Sort()
------

*Sorts a list by its values.*

**Description:**

Sorts a list by its values from smallest to largest.

**Usage:**

.. code-block:: pebl

   Sort(<list>)

**Example:**

.. code-block:: pebl

   Sort([3,4,2,1,5]) # == [1,2,3,4,5]

**See Also:**

:func:`SortBy()`, :func:`Shuffle()`


.. index:: SortBy

SortBy()
--------

**Description:**

Sorts a list by the values in another list, in ascending 		order.

**Usage:**

.. code-block:: pebl

   SortBy(<value-list>, <key-list>)

**Example:**

.. code-block:: pebl

   SortBy(["Bobby","Greg","Peter"], [3,1,2]) 
   # == ["Greg","Peter","Bobby"]

**See Also:**

:func:`Shuffle()`, :func:`Sort()`


.. index:: SubList

SubList()
---------

*Returns a sublist of a list.*

**Description:**

Extracts a list from another list, by specifying  	     	beginning and end points of new sublist.

**Usage:**

.. code-block:: pebl

   SubList(<list>, <begin>, <end>)

**Example:**

.. code-block:: pebl

   SubList([1,2,3,4,5,6],3,5)	# == [3,4,5]

**See Also:**

:func:`SubSet()`, :func:`ExtractListItems()`


.. index:: Transpose

Transpose()
-----------

*Transposes a list of equal-length lists.*

**Description:**

Transposes or ``rotates'' a list of lists.  Each   sublist must be of the same length.

**Usage:**

.. code-block:: pebl

   Transpose(<list-of-lists>)

**Example:**

.. code-block:: pebl

   Transpose([[1,11,111],[2,22,222],
              [3,33,333], [4,44,444]])
   # == [[1,2,3,4],[11,22,33,44],
   #      [111,222,333,444]]

**See Also:**

:func:`Rotate()`

================================================================================
