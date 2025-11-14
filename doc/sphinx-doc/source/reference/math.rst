================================================================================
Math Library - Extended Mathematical
================================================================================

This library contains extended mathematical functions beyond the core PEBLMath namespace.

.. contents:: Function Index
   :local:
   :depth: 0



.. index:: Bound

Bound()
-------

*Returns val, bounded by min and max.*

**Description:**

This makes sure
number is between min and max;
if min>max, it will return max, soyou need to check if that
isn't the right behavior.

**Usage:**

.. code-block:: pebl

   define Bound(number,min,max)



.. index:: CumNormInv

CumNormInv()
------------

*Returns accurate numerical approximation of cumulative normal inverse.*

**Description:**

This function takes a probability and returns the    corresponding z-score for the cumulative standard normal distribution.   It uses an accurate numerical approximation from: ``http://home.online.no/~pjacklam/notes/invnorm``

**Usage:**

.. code-block:: pebl

   define CumNormInv(...)

**Example:**

.. code-block:: pebl

   
   
    Print(CumNormInv(0))    #= NA
     Print(CumNormInv(.01)) #= -2.32634
     Print(CumNormInv(.5))  #= 0
     Print(CumNormInv(.9))  #= 1.28
     Print(CumNormInv(1))   #= NA
   

**See Also:**

:func:`NormalDensity()`, :func:`RandomNormal()`



.. index:: CumSum

CumSum()
--------

*Returns the cumulative sums of a set of numbers*

**Description:**

Returns the cumulative sum  of ``<list>``.

**Usage:**

.. code-block:: pebl

   define CumSum(...)

**Example:**

.. code-block:: pebl

   
      sum <- CumSum([1,2,3,3,4,7])
      # == [1,3,6,9,13,20]
   

**See Also:**

:func:`Min()`, :func:`Max()`, :func:`Mean()`, :func:`Median()`, :func:`Quantile()`, :func:`StDev()`



.. index:: Dist

Dist()
------

*Returns distance between two points.*

**Description:**

Returns Euclidean distance between two points.   Each point should be [x,y], and any additional items in the list are   ignored.

**Usage:**

.. code-block:: pebl

   define Dist(...)

**Example:**

.. code-block:: pebl

   
     p1 <- [0,0]
     p2 <- [3,4]
     d <- Dist(p1,p2)  #d is 5
   



.. index:: Filter

Filter()
--------

*Filters a list based on a 0/1 list produced by Match.*

**Description:**

Returns a subset of ``<list>``, depending on whether the ``<filter>`` list is zero or nonzero.  Both arguments must be lists of the same length.

**Usage:**

.. code-block:: pebl

   define Filter(...)

**Example:**

.. code-block:: pebl

    
     x <- [1,2,3,3,2,2,1]
     Print(Filter(x,[1,1,1,0,0,0,0])) ##==[1,2,3]
     Print(Filter(x,Match(x,1)))      ##== [1,1]
   

**See Also:**

:func:`Match()`, :func:`Subset()`, :func:`Lookup()`



.. index:: Match

Match()
-------

*Returns a list of 0/1s, indicating which elements of list match item.*

**Description:**

Returns a list of 0/1, indicating which elements of  ``<list>`` match ``<target>``

**Usage:**

.. code-block:: pebl

   define Match(...)

**Example:**

.. code-block:: pebl

    
     x <- [1,2,3,3,2,2,1]
     Print(Match(x,1))  ##== [1,0,0,0,0,0,1]
     Print(Match(x,2))  ##== [0,1,0,0,1,1,0]
     Print( Match(x,3)  ##== [0,0,1,1,0,0,0]
   
   

**See Also:**

:func:`Filter()`, :func:`Subset()`, :func:`Lookup()`



.. index:: NormalDensity

NormalDensity()
---------------

*Returns density of standard normal distribution.*

**Description:**

Computes density of normal standard distribution

**Usage:**

.. code-block:: pebl

   define NormalDensity(...)

**Example:**

.. code-block:: pebl

   
   
   
     Print(NormalDensity(-100))     # 1.8391e-2171
     Print(NormalDensity(-2.32635)) #5.97
     Print(NormalDensity(0))        #0.398942
     Print(NormalDensity(1.28155))  #.90687
     Print(NormalDensity(1000))     #inf
   
   

**See Also:**

:func:`RandomNormal()`, :func:`CumNormInv()`



.. index:: Order

Order()
-------

**Description:**

Returns a list of indices describing the order of values by position, from min to max.

**Usage:**

.. code-block:: pebl

   define Order(...)

**Example:**

.. code-block:: pebl

   
   	n <- [33,12,1,5,9]
     	o <- Order(n)
       Print(o) #should print [3,4,5,2,1]
   

**See Also:**

:func:`Rank()`



.. index:: Rank

Rank()
------

**Description:**

Returns a list of numbers describing the rank of   each position, from min to max.  The same as calling ``Order(Order(x))``.

**Usage:**

.. code-block:: pebl

   define Rank(...)

**Example:**

.. code-block:: pebl

   
   	n <- [33,12,1,5,9]
     	o <- Rank(n)
       Print(o) #should print [5,4,1,2,3]
   

**See Also:**

:func:`Order()`



.. index:: SDTBeta

SDTBeta()
---------

*Computes SDT beta.*

**Description:**

``SDTBeta`` computes beta, as defined by signal detection theory.  This is a measure of decision bias based on hit rate and false alarm rate.

**Usage:**

.. code-block:: pebl

   define SDTBeta(...)

**Example:**

.. code-block:: pebl

   
   
     Print(SDTBeta(.1,.9))  
     Print(SDTBeta(.1,.5))  
     Print(SDTBeta(.5,.5))  
     Print(SDTBeta(.8,.9))  
     Print(SDTbeta(.9,.95)) 
   
   

**See Also:**

:func:`SDTDPrime()`



.. index:: SDTDPrime

SDTDPrime()
-----------

*Computes SDT dprime.*

**Description:**

``SDTDPrime`` computes d-prime, as defined by signal detection theory.  This is a measure of sensitivy based jointly on hit rate and false alarm rate.

**Usage:**

.. code-block:: pebl

   define SDTDPrime(...)

**Example:**

.. code-block:: pebl

   
   
     Print(SDTDPrime(.1,.9))  #2.56431
     Print(SDTDPrime(.1,.5))  #1.28155
     Print(SDTDPrime(.5,.5))  #0
     Print(SDTDPrime(.8,.9))  #.43993
     Print(SDTDPrime(.9,.95)) #.363302
   
   

**See Also:**

:func:`SDTBeta()`,



.. index:: Sum

Sum()
-----

**Description:**

Returns the sum  of ``<list>``.

**Usage:**

.. code-block:: pebl

   define Sum(...)

**Example:**

.. code-block:: pebl

   
      sum <- Sum([3,5,99,12,1.3,15])      # == 135.3
   

**See Also:**

:func:`Min()`, :func:`Max()`, :func:`Mean()`, :func:`Median()`, :func:`Quantile()`, :func:`StDev()`



.. index:: SummaryStats

SummaryStats()
--------------

**Description:**

Computes summary statistics for a data list,   aggregated by labels in a condition list. For each condition (distinct label in the ``<cond>`` list), it will  return a list with the following entries: ``<cond>`` ``<N>`` ``<median>`` ``<mean>`` ``<sd>``

**Usage:**

.. code-block:: pebl

   define SummaryStats(...)

**Example:**

.. code-block:: pebl

   
     dat <- [1.1,1.2,1.3,2.1,2.2,2.3]
     cond <- [1,1,1,2,2,2]
     Print(SummaryStats(dat,cond))
   
   Result:
   
   [[1, 3, 1.1, 1.2, 0.0816497]
   , [2, 3, 2.1, 2.2, 0.0816497]
   ]
   

**See Also:**

:func:`StDev()`, :func:`Min()`, :func:`Max()`, :func:`Mean()`, :func:`Median()`, :func:`Quantile()`, :func:`Sum()`



.. index:: VecSum

VecSum()
--------

*Returns the pairwise sums of two lists of numbers*

**Description:**

Returns the pairwise sums of ``<list1>`` and ``<list2>``.

**Usage:**

.. code-block:: pebl

   define VecSum(...)

**Example:**

.. code-block:: pebl

   
      sum <- VecSum([1,1,1,1,2],[2,3,4,3,2])
      ## == [3,4,5,4,4]
   

**See Also:**

:func:`VecTimes()`, :func:`CumSum()`, :func:`Median()`, :func:`Quantile()`



.. index:: VecTimes

VecTimes()
----------

*Returns the pairwise products of two lists of numbers*

**Description:**

Returns the pairwise sums of ``<list1>`` and ``<list2>``.

**Usage:**

.. code-block:: pebl

   define VecTimes(...)

**Example:**

.. code-block:: pebl

   
      prod <- VecTimes([1,1,2,2,3],[2,3,4,3,2])
      ## == [2,3,8,6,6]
   

**See Also:**

:func:`VecSum()`, :func:`Mean()`, :func:`CumSum()`



.. index:: Max

Max()
-----

*Returns the largest value in a list*

**Description:**

Max returns the largest value in a list. This is a PEBL function that wraps the compiled Max function, adding error checking to ensure the argument is a list.

**Usage:**

.. code-block:: pebl

   define Max(list)

**Example:**

.. code-block:: pebl

   numbers <- [3, 7, 2, 9, 4]
   max_value <- Max(numbers)  # Returns 9

**See Also:**

:func:`Min()`, :func:`Mean()`, :func:`Median()`, :func:`StdDev()`



.. index:: Median

Median()
--------

*Returns the median value of a list*

**Description:**

Median returns the median value of a list. If the list has an even number of elements, it returns the average of the two middle values. This is a PEBL function that provides error checking and handles edge cases.

**Usage:**

.. code-block:: pebl

   define Median(list)

**Example:**

.. code-block:: pebl

   numbers1 <- [3, 7, 2, 9, 4]
   med1 <- Median(numbers1)  # Returns 4

   numbers2 <- [1, 2, 3, 4]
   med2 <- Median(numbers2)  # Returns 2.5

**See Also:**

:func:`Mean()`, :func:`Min()`, :func:`Max()`, :func:`Quantile()`, :func:`StdDev()`



.. index:: Min

Min()
-----

*Returns the smallest value in a list*

**Description:**

Min returns the smallest value in a list. This is a PEBL function that wraps the compiled Min function, adding error checking to ensure the argument is a list.

**Usage:**

.. code-block:: pebl

   define Min(list)

**Example:**

.. code-block:: pebl

   numbers <- [3, 7, 2, 9, 4]
   min_value <- Min(numbers)  # Returns 2

**See Also:**

:func:`Max()`, :func:`Mean()`, :func:`Median()`, :func:`StdDev()`



.. index:: StdDev

StdDev()
--------

*Returns the standard deviation of a list*

**Description:**

StdDev computes the standard deviation of a list of numbers. It uses the formula: sqrt(n * sum(x^2) - (sum(x))^2) / n. Returns 0 for empty lists. This is a PEBL function implemented in Math.pbl.

**Usage:**

.. code-block:: pebl

   define StdDev(list)

**Example:**

.. code-block:: pebl

   data <- [2, 4, 4, 4, 5, 5, 7, 9]
   sd <- StdDev(data)  # Calculates standard deviation

**See Also:**

:func:`Mean()`, :func:`Median()`, :func:`Min()`, :func:`Max()`, :func:`Sum()`, :func:`SummaryStats()`
