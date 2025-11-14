================================================================================
Design Library - Experimental Design
================================================================================

This library contains functions for experimental design, including Latin squares, counterbalancing, and design matrices.

.. contents:: Function Index
   :local:
   :depth: 0



.. index:: ChooseN

ChooseN()
---------

**Description:**

Samples ``<number>`` items from list, returning   a list in the original order. Items are sampled without replacement, so   once an item is chosen it will not be chosen again. If   ``<number>`` is larger than the length of the list, the entire   list is returned in order.  It differs from ``SampleN`` in that   ``ChooseN`` returns items in the order they appeared in the   originial list, but ``SampleN`` is shuffled.

**Usage:**

.. code-block:: pebl

   define ChooseN(...)

**Example:**

.. code-block:: pebl

   
   
   # Returns 5 numbers
   ChooseN([1,1,1,2,2], 5)     
   
   # Returns 3 numbers from 1 and 7:
   ChooseN([1,2,3,4,5,6,7], 3) 
   
   

**See Also:**

:func:`SampleN()`, :func:`SampleNWithReplacement()`, :func:`Subset()`



.. index:: DesignBalancedSampling

DesignBalancedSampling()
------------------------

**Description:**

Samples elements ``roughly'' equally.   		This function returns a list of repeated samples from  		``<treatment_list>``, such that each element in ``<treatment_list>``  		appears approximately equally.  Each element from  		``<treatment_list>`` is sampled once without replacement before  		all elements are returned to the mix and sampling is repeated.   		If there are no repeated items in ``<list>``, there will be no  		consecutive repeats in the output.  The last repeat-sampling  		will be truncated so that a ``<length>``-size list is returned.   		If you don't want the repeated epochs this function provides,  		Shuffle() the results.

**Usage:**

.. code-block:: pebl

   define DesignBalancedSampling(...)

**Example:**

.. code-block:: pebl

   
   DesignBalancedSampling([1,2,3,4,5],12)
   ## e.g., produces something like:
   ##    [5,3,1,4,2, 3,1,5,2,4, 3,1 ]
   

**See Also:**

:func:`CrossFactorWithoutDuplicates()`,
  :func:`Shuffle()`, :func:`DesignFullCounterBalance()`,
  		:func:`DesignGrecoLatinSquare()`, :func:`DesignLatinSquare()`, :func:`Repeat()`, 
		:func:`RepeatList()`, :func:`LatinSquare()`



.. index:: DesignGrecoLatinSquare

DesignGrecoLatinSquare()
------------------------

**Description:**

This will return a list of lists formed by rotating   through each element of the ``<treatment_list>``s, making a list   containing all element of the list, according to a greco-latin   square.  All lists must be of the same length.

**Usage:**

.. code-block:: pebl

   define DesignGrecoLatinSquare(...)

**Example:**

.. code-block:: pebl

   
   x <- ["a","b","c"]
   y <- ["p","q","r"]
   z <- ["x","y","z"]
   Print(DesignGrecoLatinSquare(x,y,z))
   # produces:   	[[[a, p, x], [b, q, y], [c, r, z]], 
   #               [[a, q, z], [b, r, x], [c, p, y]], 
   #               [[a, r, y], [b, p, z], [c, q, x]]]
   

**See Also:**

:func:`CrossFactorWithoutDuplicates()`, :func:`LatinSquare()`, :func:`DesignFullCounterBalance()`, :func:`DesignBalancedSampling()`, :func:`DesignLatinSquare()`, :func:`Repeat()`, :func:`RepeatList()`,
  :func:`Shuffle()`



.. index:: DesignLatinSquare

DesignLatinSquare()
-------------------

*Simple latin square*

**Description:**

This returns return a list of lists formed by   rotating through each element of ``<treatment_list>``, making a   list containing all element of the list. Has no side effect on input   lists.

**Usage:**

.. code-block:: pebl

   define DesignLatinSquare(...)

**Example:**

.. code-block:: pebl

   
   order <- [1,2,3]
   treatment <- ["A","B","C"]
   design <- DesignLatinSquare(order,treatment)
   # produces: [[[1, A], [2, B], [3, C]],
   #            [[1, B], [2, C], [3, A]],
   #            [[1, C], [2, A], [3, B]]]
   

**See Also:**

:func:`CrossFactorWithoutDuplicates()`,
  :func:`DesignFullCounterBalance()`, :func:`DesignBalancedSampling()`,
  :func:`DesignGrecoLatinSquare()`, :func:`Repeat()`, :func:`LatinSquare()`
  :func:`RepeatList()`, :func:`Shuffle()`, :func:`Rotate()`



.. index:: ExtractListItems

ExtractListItems()
------------------

*Gets a subset of items from a list*

**Description:**

Extracts items from a list, forming a new list.  		The list ``<items>`` are the integers representing the 		indices that should be extracted.

**Usage:**

.. code-block:: pebl

   define ExtractListItems(...)

**Example:**

.. code-block:: pebl

   
   myList <- Sequence(101, 110, 1)
   ExtractListItems(myList, [2,4,5,1,4])
   # produces [102, 104, 105, 101, 104]
   

**See Also:**

:func:`Subset()`, :func:`SubList()`, :func:`SampleN()`, :func:`Filter()`



.. index:: Flatten

Flatten()
---------

*Flattens a nested list completely*

**Description:**

Flattens nested list ``<list>`` to a single flat list.

**Usage:**

.. code-block:: pebl

   define Flatten(...)

**Example:**

.. code-block:: pebl

   
   Flatten([1,2,[3,4],[5,[6,7],8],[9]])	# == [1,2,3,4,5,6,7,8,9]
   Flatten([1,2,[3,4],[5,[6,7],8],[9]])	# == [1,2,3,4,5,6,7,8,9]
   

**See Also:**

:func:`FlattenN()`, :func:`FoldList()`



.. index:: FlattenN

FlattenN()
----------

*Flattens n levels of a nested list*

**Description:**

Flattens ``<n>`` levels of nested list ``<list>``.

**Usage:**

.. code-block:: pebl

   define FlattenN(...)

**Example:**

.. code-block:: pebl

   
   Flatten([1,2,[3,4],[5,[6,7],8],[9]],1) 
   # == [1,2,3,4,5,[6,7],8,9]
   

**See Also:**

:func:`Flatten()`, :func:`FoldList()`



.. index:: FoldList

FoldList()
----------

*Folds list into length-n sublists.*

**Description:**

Folds a list into equal-length sublists.

**Usage:**

.. code-block:: pebl

   define FoldList(...)

**Example:**

.. code-block:: pebl

   
   FoldList([1,2,3,4,5,6,7,8],2)	# == [[1,2],[3,4],[5,6],[7,8]]
   

**See Also:**

:func:`FlattenN()`, :func:`Flatten()`



.. index:: Insert

Insert()
--------

**Description:**

Inserts an element into a list at a specified   position, returning the new list. The original list in unchanged.

**Usage:**

.. code-block:: pebl

   define Insert(...)

**Example:**

.. code-block:: pebl

   
     x <- [1,2,3,5]
     y <- Insert(x,1,4)  
     ##y== [1,2,3,1,5]  
   

**See Also:**

:func:`List()`, ``Merge``, ``Append``



.. index:: LatinSquare

LatinSquare()
-------------

*A simple latin square constructor*

**Description:**

Quick and dirty latin square, taking on just one   list argument.

**Usage:**

.. code-block:: pebl

   define LatinSquare(...)

**Example:**

.. code-block:: pebl

   
   Print(LatinSquare([11,12,13,14,15,16]))
   # Output:
   #[[11, 12, 13, 14, 15, 16]
   #, [12, 13, 14, 15, 16, 11]
   #, [13, 14, 15, 16, 11, 12]
   #, [14, 15, 16, 11, 12, 13]
   #, [15, 16, 11, 12, 13, 14]
   #, [16, 11, 12, 13, 14, 15]
   #]
   
   

**See Also:**

:func:`DesignFullCounterBalance()`,
  :func:`DesignBalancedSampling()`, :func:`DesignGrecoLatinSquare()`,
  :func:`DesignLatinSquare()`, :func:`Repeat()`, :func:`RepeatList()`,
  :func:`Shuffle()`



.. index:: Levels

Levels()
--------

*Returns a sorted list of unique elements in list.*

**Description:**

Returns sorted list of unique elements of a list.

**Usage:**

.. code-block:: pebl

   define Levels(...)

**Example:**

.. code-block:: pebl

   
   Levels([1,3,55,1,5,1,5])	# == [1,3,5,55]
   

**See Also:**

:func:`Match()`, :func:`Filter()`, :func:`Sort()`



.. index:: ListBy

ListBy()
--------

*Segments a list into sublist by the values of a second list*

**Description:**

organizes a list into sublists, based on the   elements of a second list.  It returns a list of two entities: (1) a   condition list, describing what values were aggregated across; (2)   the nested list elements.  The length of each element should be the same.  Together with Match and Filter, ListBy is useful for aggregating data across blocks and conditions for immediate feedback.

**Usage:**

.. code-block:: pebl

   define ListBy(...)

**Example:**

.. code-block:: pebl

   
   
   	a <- Sequence(1,10,1)
       b <- RepeatList([1,2],5)
       x <- ListBy(a,b)
       Print(x)
   #[[1, 2],
   #  [[1, 3, 5, 7, 9],
   #   [2, 4, 6, 8, 10]]
   #]
   
       Print(ListBy(b,a))
   #[[1, 2, 3, 4, 5, 6, 7, 8, 9, 10],
   # [[1], [2], [1], [2], [1], [2], [1], [2], [1], [2]]]
   
   

**See Also:**

:func:`List()`, ``[ ]``, :func:`Merge()`, :func:`Append()`



.. index:: RemoveSubset

RemoveSubset()
--------------

**Description:**

Removes a subset of elements from a list. Creates a new list, and does not affect the original

**Usage:**

.. code-block:: pebl

   define RemoveSubset(...)

**Example:**

.. code-block:: pebl

   
    list1 <- [1,2,2,4,5]
    list2 <- RemoveSubset(list1,[2,3])
    Print(list1) #[1,2,2,4,5]
    Print(list2) #[1,4,5]
   

**See Also:**

:func:`Merge()`, :func:`Insert()`, :func:`Rest()`



.. index:: Replace

Replace()
---------

*Replaces items in a data structure*

**Description:**

Creates a copy of a (possibly nested) list in which 		items matching some list are replaced for other items.   		``<template>`` can be any data structure, and can be nested.   		``<replacementList>`` is a list containing two-item list pairs: 		the to-be-replaced item and to what it should be transformed. Note: replacement searches the entire ``<replacementList>`` for  		matches.  If multiple keys are identical, the item will be  		replaced with the last item that matches.

**Usage:**

.. code-block:: pebl

   define Replace(...)

**Example:**

.. code-block:: pebl

   
   
   x <- ["a","b","c","x"]
   rep <- [["a","A"],["b","B"],["x","D"]]
   Print(Replace(x,rep))
   # Result:  [A, B, c, D] 
   

**See Also:**

:func:`ReplaceChar()`



.. index:: Rest

Rest()
------

*Returns a list minus its first element*

**Description:**

Returns the 'rest' of a list; a list minus its   first element.  If the list is empty or has a single member, it will   return an empty list [].  This is a very common function in LISP.

**Usage:**

.. code-block:: pebl

   define Rest(...)

**Example:**

.. code-block:: pebl

   
   x <- Sequence(1,5,1)
   y <- Rest(x)
   Print(rep)
   # Result:  [2,3,4,5]
   

**See Also:**

:func:`Insert()`



.. index:: Sample

Sample()
--------

**Description:**

Samples a single item from a list, returning it.   It is a bit more convenient at times than ShuffleN(list,1), which   returns a list of length 1.  Implemented as First(ShuffleN(list,1))

**Usage:**

.. code-block:: pebl

   define Sample(...)

**Example:**

.. code-block:: pebl

   
   Sample([1,1,1,2,2])     # Returns a single number
   Sample([1,2,3,4,5,6,7]) # Returns a single number
   

**See Also:**

:func:`SeedRNG()`, :func:`Sample()`, :func:`ChooseN()`, :func:`SampleNWithReplacement()`, :func:`Subset()`



.. index:: SampleN

SampleN()
---------

**Description:**

Samples ``<number>`` items from list, returning   a randomly- ordered list. Items are sampled without replacement, so   once an item is chosen it will not be chosen again. If   ``<number>`` is larger than the length of the list, the entire   list is returned shuffled.  It differs from ``ChooseN`` in that   ``ChooseN`` returns items in the order they appeared in the   originial list.  It is implemented as ``Shuffle(ChooseN())``.

**Usage:**

.. code-block:: pebl

   define SampleN(...)

**Example:**

.. code-block:: pebl

   
   SampleN([1,1,1,2,2], 5)     # Returns 5 numbers
   SampleN([1,2,3,4,5,6,7], 3) # Returns 3 numbers 
   

**See Also:**

:func:`ChooseN()`, :func:`SampleNWithReplacement()`, :func:`Subset()`



.. index:: SampleNWithReplacement

SampleNWithReplacement()
------------------------

**Description:**

``SampleNWithReplacement`` samples   ``<number>`` items from ``<list>``, replacing after each draw   so that items can be sampled again.  ``<number>`` can be larger   than the length of the list. It has no side effects on its   arguments.

**Usage:**

.. code-block:: pebl

   define SampleNWithReplacement(...)

**Example:**

.. code-block:: pebl

   
   x <- Sequence(1:100,1)
   SampleNWithReplacement(x, 10)
   # Produces 10 numbers between 1 and 100, possibly 
   # repeating some.
   

**See Also:**

:func:`SampleN()`, :func:`ChooseN()`, :func:`Subset()`



.. index:: ShuffleRepeat

ShuffleRepeat()
---------------

**Description:**

Randomly shuffles  ``<list>``, repeating ``<n>`` times.  Shuffles  each iteration of the list separately, so you are guaranteed to go  through all elements of the list before you get another.  Returns a nested list.

**Usage:**

.. code-block:: pebl

   define ShuffleRepeat(...)

**Example:**

.. code-block:: pebl

   
   Print(ShuffleRepeat([1,2,3,4,5]),3)
   ##  Results might be anything, like:
   ## [[5,3,2,1,4], [3,2,5,1,4], [1,4,5,3,2]]
   
   
   Typically, you will want to flatten before using:
   
   list <-  Flatten(ShuffleRepeat([1,2,3], 5))
   

**See Also:**

:func:`Sort()`, :func:`SortBy()`, :func:`ShuffleRepeat()`,
                    :func:`ShuffleWithoutAdjacents()`



.. index:: ShuffleWithoutAdjacents

ShuffleWithoutAdjacents()
-------------------------

**Description:**

Randomly shuffles  ``<nested-list>``, attempting to   create a list where the nested elements do not appear adjacently in   the new list. Returns a list that is flattened one level. It will   always return a shuffled list, but it is not guaranteed to return   one that has the non-adjecent structure specified, because this is   sometimes impossible or very difficult to do randomly.  Given small   enough non-adjacent constraints with enough fillers, it should be   able to find something satisfactory.

**Usage:**

.. code-block:: pebl

   define ShuffleWithoutAdjacents(...)

**Example:**

.. code-block:: pebl

   
   Print(ShuffleWithoutAdjacents([[1,2,3],
                                  [4,5,6],
                                  [7,8,9]])
   ## Example Output: 
   ## [8, 5, 2, 7, 4, 1, 6, 9, 3]
   ## [7, 4, 8, 1, 9, 2, 5, 3, 6]
   
   ## Non-nested items are shuffled without constraint
   Print(ShuffleWithoutAdjacents([[1,2,3], 
                                 11,12,13,14,15,16]))
   ## output: [13, 11, 2, 14, 3, 15, 1, 16, 12]
   ##         [13, 12, 2, 16, 15, 11, 1, 14, 3]
   ##         [11, 1, 15, 2, 12, 16, 14, 13, 3]
   
   ## Sometimes the constraints cannot be satisfied.  
   ## 9 will always appear in position 2
   Print(ShuffleWithoutAdjacents([[1,2,3], 9])
   ## output: [3, 9, 1, 2]
   ##         [2, 9, 3, 1]
   ##         [3, 9, 2, 1]
   

**See Also:**

:func:`Shuffle()`, :func:`Sort()`, :func:`SortBy()`,
        :func:`ShuffleRepeat()`, :func:`ShuffleWithoutAdjacents()`



.. index:: Subset

Subset()
--------

*returns a subset of items from a list*

**Description:**

Extracts a subset of items from another list,   returning a new list that includes items from the original list only   once and in their original orders.  Item indices in the second   argument that do not exist in the first argument are ignored.  It   has no side effects on its arguments.

**Usage:**

.. code-block:: pebl

   define Subset(...)

**Example:**

.. code-block:: pebl

   
   Subset([1,2,3,4,5,6],[5,3,1,1])	# == [1,3,5]
   Subset([1,2,3,4,5], [23,4,2])		# == [2,4]
   

**See Also:**

:func:`SubList()`, :func:`ExtractItems()`, :func:`SampleN()`


Functions Pending Documentation
--------------------------------



.. index:: FindToken

FindToken()
-----------

*Recursively searches for a token in a nested list structure*

**Description:**

Searches recursively through a possibly nested list to find a specific token (value). Returns the index (1-based) of the first occurrence of the token found. If the token is in a nested sublist, it searches that sublist recursively. Returns 0 if the token is not found. Useful for searching complex nested data structures.

**Usage:**

.. code-block:: pebl

   define FindToken(token, nestedlist)

**Example:**

.. code-block:: pebl


   # Search in a flat list
   data <- ["apple", "banana", "cherry"]
   index <- FindToken("banana", data)
   Print(index)
   # Result: 2

   # Search in nested list
   nested <- [["a", "b"], ["c", "d"], ["e", "f"]]
   index <- FindToken("d", nested)
   Print(index)
   # Result: 2 (found in second sublist)

   # Token not found
   index <- FindToken("z", data)
   Print(index)
   # Result: 0


**See Also:**

:func:`IsMember()`, :func:`Match()`, :func:`Filter()`



.. index:: Reverse

Reverse()
---------

*Reverses the order of elements in a list*

**Description:**

Returns a new list containing all elements from the input list in reverse order. The original list is unchanged. This function is useful for reversing presentation order, creating backwards sequences, or implementing stack-like data structures.

**Usage:**

.. code-block:: pebl

   define Reverse(list)

**Example:**

.. code-block:: pebl


   x <- [1, 2, 3, 4, 5]
   y <- Reverse(x)
   Print(y)
   # Result: [5, 4, 3, 2, 1]

   words <- ["first", "second", "third"]
   Print(Reverse(words))
   # Result: [third, second, first]


**See Also:**

:func:`Rotate()`, :func:`Shuffle()`, :func:`Sort()`
