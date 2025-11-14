================================================================================
PEBLMath - Mathematical
================================================================================

This module contains compiled mathematical functions for PEBL.

.. contents:: Function Index
   :local:
   :depth: 0


Abs()
-----

**Description:**

Returns the absolute value of the number.

**Usage:**

.. code-block:: pebl

   Abs(<num>)

**Example:**

.. code-block:: pebl

   Abs(-300)  	# ==300
   Abs(23)    	# ==23

**See Also:**

:func:`Round()`, :func:`Floor()`, :func:`AbsFloor()`, :func:`Sign()`, :func:`Ceiling()`


AbsFloor()
----------

**Description:**

Rounds ``<num>`` toward 0 to an integer.

**Usage:**

.. code-block:: pebl

   AbsFloor(<num>)

**Example:**

.. code-block:: pebl

   AbsFloor(-332.7)   	# == -332
   AbsFloor(32.88)    	# == 32

**See Also:**

:func:`Round()`, :func:`Floor()`, :func:`Abs()`, :func:`Sign()`, :func:`Ceiling()`


ACos()
------

**Description:**

Inverse cosine of ``<num>``, in degrees.

**Usage:**

.. code-block:: pebl

   ACos(<num>)

**See Also:**

:func:`Cos()`, :func:`Sin()`, :func:`Tan()`, :func:`ATan()`, :func:`ATan()`


ASin()
------

**Description:**

Inverse Sine of ``<num>``, in degrees.

**Usage:**

.. code-block:: pebl

   ASin(<num>)

**See Also:**

:func:`Cos()`, :func:`Sin()`, :func:`Tan()`, :func:`ATan()`, :func:`ACos()`, :func:`ATan()`


ATan()
------

**Description:**

Inverse Tan of ``<num>``, in degrees.

**See Also:**

:func:`Cos()`, :func:`Sin()`, :func:`Tan()`, :func:`ATan()`, :func:`ACos()`, :func:`ATan()`


Ceiling()
---------

**Description:**

Rounds ``<num>`` up to the next integer.

**Usage:**

.. code-block:: pebl

   Ceiling(<num>)

**Example:**

.. code-block:: pebl

   Ceiling(33.23)  	# == 34
   Ceiling(-33.02) 	# == -33

**See Also:**

:func:`Round()`, :func:`Floor()`, :func:`AbsFloor()`, :func:`Ceiling()`


Cos()
-----

**Description:**

Cosine of ``<deg>`` degrees.

**Example:**

.. code-block:: pebl

   Cos(33.5)
     Cos(-32)

**See Also:**

:func:`Sin()`, :func:`Tan()`, :func:`ATan()`, :func:`ACos()`, :func:`ATan()`


DegToRad()
----------

**Description:**

Converts degrees to radians.

**Usage:**

.. code-block:: pebl

   DegToRad(<deg>)

**Example:**

.. code-block:: pebl

   DegToRad(180) # == 3.14159...

**See Also:**

:func:`Cos()`, :func:`Sin()`, :func:`Tan()`, :func:`ATan()`, :func:`ACos()`, :func:`ATan()`


Div()
-----

**Description:**

Returns round(``<num>/<mod>``)

**Usage:**

.. code-block:: pebl

   Div(<num>, <mod>)

**See Also:**

:func:`Mod()`


Exp()
-----

**Description:**

$e$ to the power of ``<pow>``.

**Usage:**

.. code-block:: pebl

   Exp(<pow>)

**Example:**

.. code-block:: pebl

   Exp(0) 		# == 1
   Exp(3)		# == 20.0855

**See Also:**

:func:`Log()`


Floor()
-------

**Description:**

Rounds ``<num>`` down to the next integer.

**Usage:**

.. code-block:: pebl

   Floor(<num>)

**Example:**

.. code-block:: pebl

   Floor(33.23)	# == 33
   Floor(3.999)  	# ==3
   Floor(-32.23) 	# == -33

**See Also:**

:func:`AbsFloor()`, :func:`Round()`, :func:`Ceiling()`


Ln()
----

**Description:**

Natural log of ``<num>``.

**Usage:**

.. code-block:: pebl

   Ln(<num>)

**See Also:**

:func:`Log()`, :func:`Log2()`, :func:`LogN()`, :func:`Exp()`


LogN()
------

**Description:**

Log base ``<base>`` of ``<num>``.

**Usage:**

.. code-block:: pebl

   LogN(<num>, <base>)

**Example:**

.. code-block:: pebl

   LogN(100,10)	# == 2
   LogN(256,2)	# == 8

**See Also:**

:func:`Log()`, :func:`Log2()`, :func:`Ln()`, :func:`Exp()`


Mean()
------

**Description:**

Returns the mean of the numbers in ``<list>``.

**Example:**

.. code-block:: pebl

   c <- [3,4,5,6]
     m <- Mean(c) # m == 4.5

**See Also:**

:func:`Quantile()`


Mod()
-----

**Description:**

Returns ``<num>``, ``<mod>``, or remainder of ``<num>/<mod>``

**Usage:**

.. code-block:: pebl

   Mod( <num> <mod>)

**Example:**

.. code-block:: pebl

   Mod(34, 10)	# == 4
   Mod(3, 10)	# == 3

**See Also:**

:func:`Div()`


NthRoot()
---------

**Description:**

``<num>`` to the power of  1/``<root>``.

**Usage:**

.. code-block:: pebl

   NthRoot(<num>, <root>)


Pow()
-----

**Description:**

Raises or lowers ``<num>`` to the power of ``<pow>``.

**Usage:**

.. code-block:: pebl

   Pow(<num>, <pow>)

**Example:**

.. code-block:: pebl

   Pow(2,6)	# == 64
   Pow(5,0)	# == 1


Quantile()
----------

**Description:**

Returns the ``<num>`` quantile of 		the numbers in ``<list>``. ``<num>`` should be  between         0 and 100

**Usage:**

.. code-block:: pebl

   Quantile(<list>, <num>)

**Example:**

.. code-block:: pebl

   ##Find 75th percentile to use as a threshold.
         thresh <- Quantile(rts,75)

**See Also:**

:func:`Mean()`


RadToDeg()
----------

**Description:**

Converts ``<rad>`` radians to degrees.

**Usage:**

.. code-block:: pebl

   RadToDeg( <rad>)

**See Also:**

:func:`DegToRad()`, :func:`Tan()`, :func:`Cos()`, :func:`Sin()`, :func:`ATan()`, :func:`ASin()`, :func:`ACos()`


Random()
--------

**Description:**

Returns a random number between 0 and 1.

**Usage:**

.. code-block:: pebl

   Random()

**Example:**

.. code-block:: pebl

   a <- Random()

**See Also:**

:func:`Random()`, :func:`RandomBernoulli()`, :func:`RandomBinomial()`, :func:`RandomDiscrete()`, :func:`RandomExponential()`, :func:`RandomLogistic()`, :func:`RandomLogNormal()`, :func:`RandomNormal()`, :func:`RandomUniform()`, :func:`RandomizeTimer()`, :func:`SeedRNG()`


RandomBernoulli()
-----------------

**Description:**

Returns 0 with probability ``(1-<p>)`` and 1 with probability ``<p>``.

**Usage:**

.. code-block:: pebl

   RandomBernoulli(<p>)

**Example:**

.. code-block:: pebl

   RandomBernoulli(.3)

**See Also:**

:func:`Random()`, :func:`RandomBernoulli()`,   :func:`RandomBinomial`, :func:`RandomDiscrete()`,   :func:`RandomExponential()`, :func:`RandomLogistic()`,   :func:`RandomLogNormal()`, :func:`RandomNormal()`,   :func:`RandomUniform()`, :func:`RandomizeTimer()`, :func:`SeedRNG()`


RandomBinomial()
----------------

**Description:**

Returns a random number according to the Binomial   distribution with probability ``<p>`` and repetitions ``<n>``,   i.e., the number of ``<p>`` Bernoulli trials that succeed out of   ``<n>`` attempts.

**Usage:**

.. code-block:: pebl

   RandomBinomial(<p> <n>)

**Example:**

.. code-block:: pebl

   RandomBinomial(.3, 10) # returns number from 0 to 10

**See Also:**

:func:`Random()`, :func:`RandomBernoulli()`, :func:`RandomBinomial`, 		:func:`RandomDiscrete()`, :func:`RandomExponential()`, :func:`RandomLogistic()`, 		:func:`RandomLogNormal()`, :func:`RandomNormal()`, :func:`RandomUniform()`,     		:func:`RandomizeTimer()`, :func:`SeedRNG()`


RandomDiscrete()
----------------

**Description:**

Returns a random integer between 1 and the argument  		(inclusive), each with equal probability.  If the argument is  		a floating-point value, it will be truncated down; if it is  		less than 1, it will return 1, and possibly a warning message.

**Usage:**

.. code-block:: pebl

   RandomDiscrete(<num>)

**Example:**

.. code-block:: pebl

   # Returns a random integer between 1 and 30:
   RandomDiscrete(30)

**See Also:**

:func:`Random()`, :func:`RandomBernoulli()`, :func:`RandomBinomial`,  		:func:`RandomDiscrete()`, :func:`RandomExponential()`, :func:`RandomLogistic()`, 		:func:`RandomLogNormal()`, :func:`RandomNormal()`, :func:`RandomUniform()`, 		:func:`RandomizeTimer()`, :func:`SeedRNG()`


RandomExponential()
-------------------

**Description:**

Returns a random number according to exponential  		distribution with mean ``<mean>`` (or decay 1/mean).

**Usage:**

.. code-block:: pebl

   RandomExponential(<mean>)

**Example:**

.. code-block:: pebl

   RandomExponential(100)

**See Also:**

:func:`Random()`, :func:`RandomBernoulli()`, :func:`RandomBinomial`, 		:func:`RandomDiscrete()`, :func:`RandomLogistic()`, :func:`RandomLogNormal()`,  		:func:`RandomNormal()`, :func:`RandomUniform()`, :func:`RandomizeTimer`, :func:`SeedRNG()`


RandomizeTimer()
----------------

**Description:**

Seeds the RNG with the current time.

**Usage:**

.. code-block:: pebl

   RandomizeTimer()

**Example:**

.. code-block:: pebl

   RandomizeTimer()
   x <- Random()

**See Also:**

:func:`Random()`, :func:`RandomBernoulli()`, :func:`RandomBinomial`, 		:func:`RandomDiscrete()`, :func:`RandomExponential()`, :func:`RandomLogistic()`, 		:func:`RandomLogNormal()`, :func:`RandomNormal()`, :func:`RandomUniform()`, :func:`SeedRNG()`


RandomLogistic()
----------------

**Description:**

Returns a random number according to the logistic distribution with parameter ``<p>``: f(x) = exp(x)/(1+exp(x))

**Usage:**

.. code-block:: pebl

   RandomLogistic(<p>)

**See Also:**

:func:`Random()`, :func:`RandomBernoulli()`, :func:`RandomBinomial`,  		:func:`RandomDiscrete()`, :func:`RandomExponential()`, :func:`RandomLogNormal()`,  		:func:`RandomNormal()`, :func:`RandomUniform()`, :func:`RandomizeTimer`, :func:`SeedRNG()`


RandomLogNormal()
-----------------

**Description:**

Returns a random number according to the log-normal  		distribution with parameters ``<median>`` and ``<spread>``. Generated by calculating $median ``*`` exp(spread ``*`` RandomNormal(0,1))$. ``<spread>`` is a shape parameter, and only affects the variance as a function of the median; similar to the coefficient of variation.  A value near 0 is a sharp distribution (.1-.3), larger values are more spread out; values greater than 2 make  little difference in the shape.

**Usage:**

.. code-block:: pebl

   RandomLogNormal(<median>, <spread>)

**Example:**

.. code-block:: pebl

   RandomLogNormal(5000, .1)

**See Also:**

:func:`Random()`, :func:`RandomBernoulli()`, :func:`RandomBinomial`,  		:func:`RandomDiscrete()`, :func:`RandomExponential()`, :func:`RandomLogistic()`, 		:func:`RandomNormal()`, :func:`RandomUniform()`, :func:`RandomizeTimer`, :func:`SeedRNG()`


RandomNormal()
--------------

**Description:**

Returns a random number according to the standard normal distribution with ``<mean>`` and ``<stdev>``.

**Usage:**

.. code-block:: pebl

   RandomNormal(<mean>, <stdev>)

**See Also:**

:func:`Random()`, :func:`RandomBernoulli()`, :func:`RandomBinomial`, 		:func:`RandomDiscrete()`, :func:`RandomExponential()`, :func:`RandomLogistic()`,  		:func:`RandomLogNormal()`, :func:`RandomUniform()`, :func:`RandomizeTimer`, :func:`SeedRNG()`


RandomUniform()
---------------

**Description:**

Returns a random floating-point number between 0 and ``<num>``.

**Usage:**

.. code-block:: pebl

   RandomUniform(<num>)

**See Also:**

:func:`Random()`, :func:`RandomBernoulli()`,   :func:`RandomBinomial`, :func:`RandomDiscrete()`,   :func:`RandomExponential()`, :func:`RandomLogistic()`,   :func:`RandomLogNormal()`, :func:`RandomNormal()`, :func:`RandomizeTimer()`,   :func:`SeedRNG()`


Round()
-------

**Description:**

Rounds ``<num>`` to nearest integer, or if optional ``<precision>`` argument is included, to nearest $10^{-precision}$.

**Usage:**

.. code-block:: pebl

   Round(<num>)
   Round(<num>,<precision>)

**Example:**

.. code-block:: pebl

   Round(33.23)       # == 33
   Round(56.65)       # == 57
   Round(33.12234,2)  # == 33.12
   Round(43134.23,-2) # == 43100

**See Also:**

:func:`Ceiling()`, :func:`Floor()`, :func:`AbsFloor()`, :func:`ToInt()`


SeedRNG()
---------

**Description:**

Seeds the random number generator with ``<num>``   to reproduce a random sequence.  This function can be used cleverly   to create a multi-session experiment: Start by seeding the RNG with   a single number for each subject; generate the stimulus sequence,   then extract the appropriate stimuli for the current block. Remember   to ``RandomizeTimer()`` afterward if necessary.

**Usage:**

.. code-block:: pebl

   SeedRNG(<num>)

**Example:**

.. code-block:: pebl

   ##This makes sure you get the same random order 
       ## across sessions for individual subjects.
        SeedRNG(gSubNum)
        stimTmp <- Sequence(1:100,1)
        stim <- Shuffle(stimTmp)
        RandomizeTimer()

**See Also:**

:func:`RandomizeTimer()`


Sign()
------

**Description:**

Returns +1 or -1, depending on sign of argument.

**Usage:**

.. code-block:: pebl

   Sign(<num>)

**Example:**

.. code-block:: pebl

   Sign(-332.1)  # == -1
   Sign(65)      # == 1

**See Also:**

:func:`Abs()`


Sin()
-----

**Description:**

Sine of ``<deg>`` degrees.

**Usage:**

.. code-block:: pebl

   Sin(<deg>)

**Example:**

.. code-block:: pebl

   Sin(180)
    Sin(0)

**See Also:**

:func:`Cos()`, :func:`Tan()`, :func:`ATan()`, :func:`ACos()`, :func:`ASin()`


Sqrt()
------

**Description:**

Square root of ``<num>``.

**Usage:**

.. code-block:: pebl

   Sqrt(<num>)

**Example:**

.. code-block:: pebl

   Sqrt(100)  # == 10


Tan()
-----

**Description:**

Tangent of ``<deg>`` degrees.

**Usage:**

.. code-block:: pebl

   Tan(<deg>)

**Example:**

.. code-block:: pebl

   Tan(180)

**See Also:**

:func:`Cos()`, :func:`Sin()`, :func:`ATan()`, :func:`ACos()`, :func:`ASin()`


ToFloat()
---------

**Description:**

Converts number to internal floating-point representation.

**Usage:**

.. code-block:: pebl

   ToFloat(<number>)


ToInteger()
-----------

**Description:**

Rounds a number to an integer, changing internal  		representation.

**Usage:**

.. code-block:: pebl

   ToInteger(<number>)
   ToInteger(<floating-point>)
   ToInteger(<string-as-number>)

**Example:**

.. code-block:: pebl

   ToInteger(33.332)  # == 33
   ToInteger("3213")  # == 3213

**See Also:**

:func:`Round()`, :func:`Ceiling()`, :func:`AbsCeiling()`, :func:`Floor()`, :func:`AbsFloor()`


ToNumber()
----------

**Description:**

Converts a variant to a number. Most useful for   character strings that are interpretable as a number, but may also   work for other subtypes.

**Usage:**

.. code-block:: pebl

   ToNumber(<string)
   ToNumber(<number>)

**Example:**

.. code-block:: pebl

   a <- ToNumber("3232")
   Print(a + 1)		# produces the output 3233.

**See Also:**

:func:`ToString()`, :func:`ToFloat()`, :func:`Round()`


ToString()
----------

**Description:**

Converts value to a string representation. Most   useful for numerical values.  This conversion is done automatically   when strings are combined with numbers.

**Usage:**

.. code-block:: pebl

   ToString(<number>)
   ToString(<string>)

**Example:**

.. code-block:: pebl

   a <- ToString(333.232)
   Print(a + "111")
   # produces the output '333.232111'.

**See Also:**

:func:`ToString()`, ``+``.


================================================================================
Functions Pending Documentation
================================================================================


RandomDiscrete()
----------------

.. note::
   **Documentation pending.** This function exists in PEBL but needs full documentation.

**Description:**

Returns a random integer between 1 and the argument  		(inclusive), each with equal probability.  If the argument is  		a floating-point value, it will be truncated down; if it is  		less than 1, it will return 1, and possibly a warning message.


ToInteger()
-----------

.. note::
   **Documentation pending.** This function exists in PEBL but needs full documentation.

**Description:**

Rounds a number to an integer, changing internal  		representation.


================================================================================
