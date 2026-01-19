================================================================================
PEBLStream - File and Network I/O
================================================================================

This module contains functions for file I/O, network communication, and data streaming.

.. contents:: Function Index
   :local:
   :depth: 0



.. index:: AppendFile

AppendFile()
------------

*Appends a file2 to file1*

**Description:**

Appends onto the end of ``<file1>`` the contents of ``<file2>``.  Useful for compiling pooled data at the end of an experiment.

**Usage:**

.. code-block:: pebl

   AppendFile(<file1>, <file2>)

**See Also:**

:func:`FileOpenWrite()` , :func:`FileOpenAppend()` 



.. index:: AcceptNetworkConnection

AcceptNetworkConnection()
-------------------------

*Accepts an incoming network connection on a listening port*

**Description:**

Accepts an incoming TCP/IP connection on a network listener that was opened using ``OpenNetworkListener()``. Returns a network connection object that can be used to send and receive data. This is typically used after ``CheckForNetworkConnection()`` confirms a connection is available.

**Usage:**

.. code-block:: pebl

   AcceptNetworkConnection(<listener>, <port>)

**Example:**

.. code-block:: pebl

   listener <- OpenNetworkListener(4444)
   if(CheckForNetworkConnection(listener))
   {
      connection <- AcceptNetworkConnection(listener, 4444)
      SendData(connection, "Hello client!")
      CloseNetworkConnection(connection)
   }

**See Also:**

:func:`OpenNetworkListener()`, :func:`CheckForNetworkConnection()`, :func:`WaitForNetworkConnection()`, :func:`CloseNetworkConnection()`



.. index:: CheckForNetworkConnection

CheckForNetworkConnection()
---------------------------

**Description:**

Checks to see if there is an incoming TCP/IP connection on a network that is opened using ``OpenNetworkListener``.  This is an alternative to the ``WaitForNetworkConnection`` function that allows more flexibility (and allows updating the during waiting for the connection).

**Usage:**

.. code-block:: pebl

   net <- CheckForNetwokConnection(network)

**Example:**

.. code-block:: pebl

   network <-      OpenNetworkListener(4444) 
     time <- GetTime()
     while(not connected and (GetTime() < time + 5000))
      {
         connected <- CheckForNetwokConnection(network) 
      }

**See Also:**

:func:`OpenNetworkListener()`, :func:`Getdata()`, :func:`WaitForNetworkConnection()`, :func:`CloseNetwork()`



.. index:: CloseNetworkConnection

CloseNetworkConnection()
------------------------

**Description:**

Closes network connection

**Usage:**

.. code-block:: pebl

   CloseNetwork(<network>)

**Example:**

.. code-block:: pebl

   net <- WaitForNetworkConnection("localhost",1234)
   SendData(net,"Watson, come here. I need you.")
   CloseNetworkConnection(net)

**See Also:**

:func:`ConnectToIP`, :func:`ConnectToHost`,  :func:`WaitForNetworkConnection`, :func:`GetData`,  :func:`SendData`, :func:`ConvertIPString`



.. index:: ConnectToHost

ConnectToHost()
---------------

*Connects to a port on another computer, returning network object.*

**Description:**

Connects to a host computer waiting for a   connection on <port>, returning a network object that can be used to   communicate.  Host is a text hostname, like ``"myname.indiana.edu"``, or   use ``"localhost"`` to specify your current computer.

**Usage:**

.. code-block:: pebl

   ConnectToHost(<hostname>,<port>)

**See Also:**

:func:`ConnectToIP`, :func:`GetData`,  :func:`WaitForNetworkConnection`, :func:`SendData`, :func:`ConvertIPString`, :func:`CloseNetworkConnection`



.. index:: ConnectToIP

ConnectToIP()
-------------

*Connects to a port on another computer, returning network object.*

**Description:**

Connects to a host computer waiting for a   connection on ``<port>``, returning a network object that can be used to   communicate.  ``<ip>`` is a numeric ip address, which must be   created with the ``ConvertIPString(ip)`` function.

**Usage:**

.. code-block:: pebl

   ConnectToIP(<ip>,<port>)

**See Also:**

:func:`ConnectToHost`, :func:`GetData`, :func:`WaitForNetworkConnection`, :func:`SendData`, :func:`ConvertIPString`, :func:`CloseNetworkConnection`



.. index:: CopyFile

CopyFile()
----------

*Makes a copy of a file*

**Description:**

This makes a copy of a specified file, by  Copying the contents of one file to another. 	This makes the copy byte-by-byte (so should work for binary data).  It is probably better to use a systemcall function to make a copy of an entire file at once. This is likely to be slower and possibly error-prone (i.e., permissions and other file properties may not copy.), but it is a useful cross-platform solution to creating a new file based on others.  It copies by name from the current working directory.

**Example:**

.. code-block:: pebl

   base <- "template.txt"
   CopyFile(base,"newfile.txt")

**See Also:**

:func:`DeleteFile()`, :func:`AppendFile()` , :func:`FileExists()`



.. index:: EndOfFile

EndOfFile()
-----------

*Returns true if at the end of a file*

**Description:**

Returns true if at the end of a file.

**Usage:**

.. code-block:: pebl

   EndOfFile(<filestream>)

**Example:**

.. code-block:: pebl

   while(not EndOfFile(fstream))
   {
    Print(FileReadLine(fstream))
   }



.. index:: EndOfLine

EndOfLine()
-----------

*Returns true if at end of line*

**Description:**

Returns true if at end of line.

**Usage:**

.. code-block:: pebl

   EndOfLine(<filestream>)



.. index:: FileClose

FileClose()
-----------

*Closes a filestream variable. Pass the variable name, not the filename*

**Description:**

Closes a filestream  variable.  Be sure to  		pass the variable name, not the filename.

**Usage:**

.. code-block:: pebl

   FileClose(<filestream>)

**Example:**

.. code-block:: pebl

   x <- FileOpenRead("file.txt")
   # Do relevant stuff here.
   FileClose(x)

**See Also:**

:func:`FileOpenAppend()`, :func:`FileOpenRead()`, :func:`FileOpenWrite()`



.. index:: FileOpenAppend

FileOpenAppend()
----------------

*Opens a filename, returning a stream that can be used for writing info. Appends if the file already exists, opens if file does not*

**Description:**

Opens a filename, returning a stream that can be   used for writing information.  Appends if the file already exists.

**Usage:**

.. code-block:: pebl

   FileOpenAppend(<filename>)

**See Also:**

:func:`FileClose()`, :func:`FileOpenRead()`, :func:`FileOpenWrite()`,  :func:`FileOpenOverWrite()`



.. index:: FileOpenOverwrite

FileOpenOverwrite()
-------------------

*Opens a filename, returning a stream that can be used for writing information. Overwrites if file already exists*

**Description:**

Opens a filename, returning a stream that can be   used for writing information.  Overwrites if file already exists.   This function should not be used for opening data files; instead,   use FileOpenWrite, which saves to a backup file if the specified   file already exists.

**Usage:**

.. code-block:: pebl

   FileOpenOverWrite(<filename>)

**See Also:**

:func:`FileClose()`, :func:`FileOpenAppend()`, :func:`FileOpenRead()`  :func:`FileOpenWrite()`



.. index:: FileOpenRead

FileOpenRead()
--------------

*Opens a filename, returning a stream to be used for reading information*

**Description:**

Opens a filename, returning  a stream to be used  		for reading information.

**Usage:**

.. code-block:: pebl

   FileOpenRead(<filename>)

**See Also:**

:func:`FileClose()`, :func:`FileOpenAppend()`, :func:`FileOpenWrite()`, :func:`FileOpenOverWrite()`



.. index:: FileOpenWrite

FileOpenWrite()
---------------

*Opens a filename, returning a stream that can be used for writing information. Creates new file if file already exists*

**Description:**

Opens a filename, returning a stream that can be   used for writing information.  If the specified filename exists, it   won't overwrite that file.  Instead, it will create a related   filename, appending a -integer before the filename extension.

**Usage:**

.. code-block:: pebl

   FileOpenWrite(<filename>)

**See Also:**

:func:`FileClose()`, :func:`FileOpenAppend()`, :func:`FileOpenRead()`, :func:`FileOpenOverWrite()`



.. index:: FilePrint

FilePrint()
-----------

**Description:**

Like ``Print_``, but to a file.  Prints a   string to a file,	without appending a newline character.  Returns a   copy of the string it prints.

**Usage:**

.. code-block:: pebl

   FilePrint_(<filestream>, <value>)

**Example:**

.. code-block:: pebl

   FilePrint_(fstream, "This line doesn't end.")

**See Also:**

:func:`Print_()`, :func:`FilePrint()`



.. index:: FileReadCharacter

FileReadCharacter()
-------------------

*Reads and returns a single character from a filestream*

**Description:**

Reads and returns a single character from a filestream.

**Usage:**

.. code-block:: pebl

   FileReadCharacter(<filestream>)

**See Also:**

:func:`FileReadList()`, :func:`FileReadTable()`    :func:`FileReadLine()`, 	:func:`FileReadText()`, 	:func:`FileReadWord()`,



.. index:: FileReadLine

FileReadLine()
--------------

*Reads and returns a line from a file; all characters up until the next newline or the end of the file*

**Description:**

Reads and returns a line from a file; all characters up 		until the next newline or the end of the file.

**Usage:**

.. code-block:: pebl

   FileReadLine(<filestream>)

**See Also:**

:func:`FileReadCharacter()`, :func:`FileReadList()`, :func:`FileReadTable()`, :func:`FileReadText()`, :func:`FileReadWord()`



.. index:: FileReadList

FileReadList()
--------------

*Given a filename, will open it, read in all the items into a list (one item per line), and close the file afterwards*

**Description:**

Given a filename, will open it, read in all the   items into a list (one item per line), and close the file afterward.   Ignores blank lines or lines starting with ``#``.  Useful with a   number of pre-defined data files stored in ``media/text/``.  See   Section~\ref{sec:media}: Provided Media Files.

**Usage:**

.. code-block:: pebl

   FileReadList(<filename>)

**Example:**

.. code-block:: pebl

   FileReadList("data.txt")

**See Also:**

:func:`FileReadCharacter()`, :func:`FileReadTable()` 	:func:`FileReadLine()`, 	:func:`FileReadText()`, 	:func:`FileReadWord()`,



.. index:: FileReadTable

FileReadTable()
---------------

**Description:**

Reads a table directly from a file. Data in file should 		separated by spaces.  Reads each line onto a sublist, 		with space-separated tokens as items in sublist.  Ignores 		blank lines or lines beginning with ``#``. Optionally, 		specify a token separator other than space.

**Usage:**

.. code-block:: pebl

   FileReadTable(<filename>, <optional-separator>)

**Example:**

.. code-block:: pebl

   a <- FileReadTable("data.txt")

**See Also:**

:func:`FileReadCharacter()`, :func:`FileReadList()`, :func:`FileReadLine()`, :func:`FileReadText()`, :func:`FileReadWord()`



.. index:: FileReadText

FileReadText()
--------------

*Reads all of the text in the file into a variable*

**Description:**

Returns all of the text from a file, ignoring any lines 		beginning with ``#``. Opens and closes the file transparently.

**Usage:**

.. code-block:: pebl

   FileReadText(<filename>)

**Example:**

.. code-block:: pebl

   instructions <- FileReadText("instructions.txt")

**See Also:**

:func:`FileReadCharacter()`, :func:`FileReadList()`, :func:`FileReadTable()`, :func:`FileReadLine()`, :func:`FileReadWord()`



.. index:: FileReadWord

FileReadWord()
--------------

**Description:**

Reads and returns  a `word' from a file; the next 		connected stream of characters not including a ``' '`` 		or a newline. Will not read newline characters.

**Usage:**

.. code-block:: pebl

   FileReadWord(<filestream>)

**See Also:**

:func:`FileReadCharacter()`, :func:`FileReadList()`, :func:`FileReadTable()`, :func:`FileReadLine()`, :func:`FileReadText()`



.. index:: MD5File

MD5File()
---------

**Description:**

Computes MD5 checksum of a file. Returns blank if file does not exist.

**Usage:**

.. code-block:: pebl

   MD5File(<filename>)

**Example:**

.. code-block:: pebl

   text <- FileReadText("test.pbl")
   Print(MD5Sum(text))
   #returns: 3396a651bd3c96f9799ce02eecb48801; see similar example next

   Print(MD5File("test.pbl"))
   # returns 3396a651bd3c96f9799ce02eecb48801

   Print(MD5File("doesnotexist.txt"))
   #returns 0

**See Also:**

:func:`MD5Sum()`, :func:`FileReadText()`



.. index:: MD5Sum

MD5Sum()
--------

**Description:**

Computes MD5 checksum on text. Returns blank if no string provided.

**Usage:**

.. code-block:: pebl

   MD5Sum(<text>)

**Example:**

.. code-block:: pebl

   Print(MD5Sum(""))
   #Return: d41d8cd98f00b204e9800998ecf8427e

   Print(MD5Sum("bananana"))
   #returns bb8e9af523e4aeffa88f1807fb2af9ce

   text <- FileReadText("test.pbl")
   Print(MD5Sum(text))
   #returns: 3396a651bd3c96f9799ce02eecb48801

**See Also:**

:func:`MD5File()`, :func:`FileReadText()`



.. index:: GetData

GetData()
---------

*return a string from network connection*

**Description:**

Gets Data from network connection.  Example of   usage in demo/nim.pbl.

**Usage:**

.. code-block:: pebl

   val <- GetData(<network>,<size>)

**See Also:**

:func:`ConnectToIP`, :func:`ConnectToHost`, :func:`WaitForNetworkConnection`,    :func:`SendData`, :func:`ConvertIPString`, :func:`CloseNetworkConnection`



.. index:: GetMyIPAddress

GetMyIPAddress()
----------------

*Returns the local IP address of this computer*

**Description:**

Returns the IP address of the current computer as a formatted string. The returned address is suitable for use in networking functions. If multiple network interfaces are present, it typically returns the primary interface address.

**Usage:**

.. code-block:: pebl

   GetMyIPAddress(<interface_number>)

**Example:**

.. code-block:: pebl

   myIP <- GetMyIPAddress(0)
   Print("My IP address is: " + myIP)

**See Also:**

:func:`ConnectToHost()`, :func:`ConnectToIP()`, :func:`OpenNetworkListener()`



.. index:: GetPPortState

GetPPortState()
---------------

*Gets state of parallel port data bits*

**Description:**

Gets the parallel port state, as a list of 8 'bits' (1s or 0s).

**See Also:**

:func:`COMPortGetByte`, :func:`COMPortSendByte`, :func:`OpenPPort` :func:`OpenCOMPort`, :func:`SetPPortMode`, :func:`GetPPortState`



.. index:: OpenCOMPort

OpenCOMPort()
-------------

*Opens a serial (com) port*

**Description:**

This opens a COM/Serial port, and is used by many usb devices for communication.  The general process is to use OpenComPort to create the port, and then send and receive text strings from that port.  These are sent one byte at a time. The mode argument is a 3-character string that specifies aspects of the mode (see  Teunis van Beelen's rs232 library at `http://www.teuniz.net/RS-232/ <http://www.teuniz.net/RS-232/>`_. The first character is the data bits (5,6,7 or 8), parity (N=none, E=even, O=odd), and the third is the stop bit (1 or 2 bits).   Within the demo\ directory, there is some basic code for communicating with the cedrus response box that uses these functions.  In addition, that script provide a NumToASCII() function that can be useful in translating numbers to strings to communicate with a device.

**Example:**

.. code-block:: pebl

   port <- OpenCOMPort(16,9600,"8N1")
     Print( ComPortGetByte(port))

**See Also:**

:func:`COMPortGetByte`, :func:`COMPortSendByte`, :func:`OpenPPort`, :func:`SetPPortMode`, :func:`GetPPortMode`



.. index:: OpenNetworkListener

OpenNetworkListener()
---------------------

*Opens a port for listening*

**Description:**

Creates a network object that listens on a particular port, and is able to accept incoming connections. You can the nuse ``CheckForNetworkConnections`` to accept incoming connections.   This is an alternative to the ``WaitForNetworkConnection`` function that allows more flexibility (and allows updating the during waiting for the connection).

**Usage:**

.. code-block:: pebl

   net <- OpennetworkListener(port)

**Example:**

.. code-block:: pebl

   network <-      OpenNetworkListener(4444) 
     time <- GetTime()
     while(not connected and (GetTime() < time + 5000))
      {
         connected <- CheckForNetwokConnection(network) 
      }

**See Also:**

:func:`CheckForNetworkConnection()`, :func:`Getdata()`, :func:`WaitForNetworkConnection()`, :func:`CloseNetwork()`



.. index:: OpenPPort

OpenPPort()
-----------

*Opens parallel port*

**Description:**

Opens a Parallel  port, returning an object that can be used for parallel port communications.

**See Also:**

:func:`COMPortGetByte`, :func:`COMPortSendByte`, :func:`OpenCOMPort`, :func:`SetPPortMode`, :func:`GetPPortMode`



.. index:: ParseJSON

ParseJSON()
-----------

*Parses a JSON string into PEBL data structures*

**Description:**

Parses a JSON-formatted string and converts it into PEBL data structures (lists and custom objects). JSON objects become PEBL custom objects with properties, and JSON arrays become PEBL lists. This is useful for working with web APIs and configuration files.

**Usage:**

.. code-block:: pebl

   ParseJSON(<json_string>)

**Example:**

.. code-block:: pebl

   jsonString <- GetHTTPText("https://api.example.com/data", "", "")
   data <- ParseJSON(jsonString)

   ##Access parsed data
   Print(data.name)
   Print(data.values)

**See Also:**

:func:`GetHTTPText()`, :func:`JSONText()`, :func:`PostHTTP()`, :func:`MakeCustomObject()`



.. index:: Print

Print()
-------

**Description:**

Prints ``<value>`` to stdout; doesn't append a newline afterwards.

**Usage:**

.. code-block:: pebl

   Print_(<value>)

**Example:**

.. code-block:: pebl

   Print_("This line")
   Print_(" ")
   Print_("and")
   Print_(" ")
   Print("Another line")
   # prints out: 'This line and Another line'

**See Also:**

:func:`Print()`, :func:`FilePrint()`



.. index:: SendData

SendData()
----------

*Sends a data string over connection.*

**Description:**

Sends data on network connection.  Example of   usage in demo/nim.pbl. You can only send text data.

**Usage:**

.. code-block:: pebl

   SendData(<network>,<data_as_string>)

**See Also:**

:func:`ConnectToIP`, :func:`ConnectToHost`, :func:`WaitForNetworkConnection`, :func:`GetData`, :func:`ConvertIPString`, :func:`CloseNetworkConnection`



.. index:: SetNetworkPort

SetNetworkPort()
----------------

*Configures the default network port for connections*

**Description:**

Sets the default network port number to be used for subsequent network operations. This allows you to configure the port once rather than specifying it for each connection. The port number should be between 1024 and 65535 for user applications.

**Usage:**

.. code-block:: pebl

   SetNetworkPort(<port>)

**Example:**

.. code-block:: pebl

   SetNetworkPort(8080)
   listener <- OpenNetworkListener(8080)

**See Also:**

:func:`OpenNetworkListener()`, :func:`ConnectToHost()`, :func:`ConnectToIP()`



.. index:: SetPPortMode

SetPPortMode()
--------------

*Sets parallel port mode (input/output)*

**Description:**

Sets a parallel port mode, either "<input>" or "<output>".

**See Also:**

:func:`COMPortGetByte`, :func:`COMPortSendByte`, :func:`OpenPPort` :func:`OpenCOMPort`, :func:`SetPPortMode`, :func:`GetPPortState`



.. index:: SetPPortState

SetPPortState()
---------------

*Sets parallel port state*

**Description:**

Sets a parallel port state, using a list of 8 'bits' (1s or 0s).

**See Also:**

:func:`COMPortGetByte`, :func:`COMPortSendByte`, :func:`OpenPPort` :func:`OpenCOMPort`, :func:`SetPPortMode`, :func:`GetPPortState`



.. index:: WaitForNetworkConnection

WaitForNetworkConnection()
--------------------------

**Description:**

Listens on a port, waiting until another computer or process   connects. Return a network object that can be used for communication.

**Usage:**

.. code-block:: pebl

   WaitForNetworkConnection(<port>)

**See Also:**

:func:`ConnectToHost`, :func:`ConnectToIP`, :func:`GetData`, :func:`WaitForNetworkConnection`,    :func:`SendData`, :func:`ConvertIPString`, :func:`CloseNetworkConnection`



.. index:: WritePNG

WritePNG()
----------

*Makes a .png from a window or object*

**Description:**

WritePNG() creates a graphic file of the screen or   a widget on the screen.  It can also be given an arbitrary widget.   For the most part, widgets added to other widgets will be captured   fine, but sometimes polygons and shapes added to other widgets may   not appear in the output png.

**Usage:**

.. code-block:: pebl

   x <-  WritePNG("screen1.png",gWin)
   
     ## Use like this to create an animated screencast
      define DrawMe()
       {
         pname <- "fileout"+ZeroPad(gid,5)+".png"
         Draw()
         WritePNG(pname,gWin)
       }
     
      define Start(p)  
      {
        gid <- 1
        gWin <- MakeWindow()
        img <- MakeImage("pebl.png")
        AddObject(img,gWin)
        while(gid < 100)
         {
            Move(img,RandomDiscrete(800),
                     RandomDiscrete(600))
    
            DrawMe()
            gid <- gid + 1
         }
   
      }

**See Also:**

:func:`FileWriteTable`



.. index:: COMPortGetByte

COMPortGetByte()
----------------

*Gets a byte from the COM port*

**Description:**

Reads a single byte from an open COM/serial port. Returns the byte value as an integer (0-255). If no data is available, it returns -1. This is used for serial communication with external devices.

**Usage:**

.. code-block:: pebl

   COMPortGetByte(<port>)

**Example:**

.. code-block:: pebl

   port <- OpenCOMPort(1, 9600, "8N1")
   byte <- COMPortGetByte(port)
   if(byte >= 0)
   {
      Print("Received byte: " + byte)
   }

**See Also:**

:func:`COMPortSendByte()`, :func:`OpenCOMPort()`



.. index:: COMPortSendByte

COMPortSendByte()
-----------------

*Sends a byte to the COM port*

**Description:**

Sends a single byte to an open COM/serial port. The byte should be an integer value between 0 and 255. This is used for serial communication with external devices.

**Usage:**

.. code-block:: pebl

   COMPortSendByte(<port>, <byte>)

**Example:**

.. code-block:: pebl

   port <- OpenCOMPort(1, 9600, "8N1")
   ##Send ASCII 'A' (65)
   COMPortSendByte(port, 65)

**See Also:**

:func:`COMPortGetByte()`, :func:`OpenCOMPort()`



.. index:: GetHTTPFile

GetHTTPFile()
-------------

*Downloads a file from a web server via HTTP*

**Description:**

Fetches a file from a web server using HTTP and saves it to a local file. Supports HTTP and HTTPS protocols. Useful for downloading stimuli, configuration files, or data from web servers during experiments.

**Usage:**

.. code-block:: pebl

   GetHTTPFile(<url>, <output_filename>, <username>, <password>)

**Example:**

.. code-block:: pebl

   ##Download without authentication
   success <- GetHTTPFile("http://example.com/data.csv", "local_data.csv", "", "")

   ##Download with authentication
   success <- GetHTTPFile("https://secure.example.com/file.zip", "download.zip", "user", "pass")

**See Also:**

:func:`GetHTTPText()`, :func:`PostHTTP()`, :func:`PostHTTPFile()`



.. index:: GetHTTPText

GetHTTPText()
-------------

*Retrieves text content from a web server via HTTP*

**Description:**

Fetches content from a web server using HTTP and returns it as a text string. Supports HTTP and HTTPS protocols. Useful for retrieving instructions, configurations, or data from web services during experiments.

**Usage:**

.. code-block:: pebl

   GetHTTPText(<url>, <username>, <password>)

**Example:**

.. code-block:: pebl

   ##Fetch text without authentication
   text <- GetHTTPText("http://example.com/instructions.txt", "", "")
   Print(text)

   ##Fetch JSON data with authentication
   jsonData <- GetHTTPText("https://api.example.com/config", "user", "pass")
   config <- ParseJSON(jsonData)

**See Also:**

:func:`GetHTTPFile()`, :func:`PostHTTP()`, :func:`ParseJSON()`



.. index:: PostHTTP

PostHTTP()
----------

*Sends data to a web server via HTTP POST*

**Description:**

Sends data to a web server using the HTTP POST method. Returns the server's response as a string. This is useful for submitting experimental data to web servers, interacting with web APIs, or logging data remotely.

**Usage:**

.. code-block:: pebl

   PostHTTP(<url>, <post_data>, <username>, <password>, <content_type>)

**Example:**

.. code-block:: pebl

   ##Post form data
   postData <- "subjectID=123&condition=A&score=85"
   response <- PostHTTP("http://example.com/submit", postData, "", "", "application/x-www-form-urlencoded")

   ##Post JSON data
   jsonData <- "{"subject":123,"rt":450}"
   response <- PostHTTP("https://api.example.com/data", jsonData, "user", "pass", "application/json")

**See Also:**

:func:`PostHTTPFile()`, :func:`GetHTTPText()`, :func:`GetHTTPFile()`



.. index:: PostHTTPFile

PostHTTPFile()
--------------

*Uploads a file to a web server via HTTP POST*

**Description:**

Uploads a file to a web server using HTTP POST multipart/form-data encoding. This is useful for uploading experimental data files, log files, or other content to a web server for storage or processing.

**Usage:**

.. code-block:: pebl

   PostHTTPFile(<url>, <filename>, <fieldname>, <username>, <password>, <additional_fields>)

**Example:**

.. code-block:: pebl

   ##Upload data file
   response <- PostHTTPFile("http://example.com/upload", "data.csv", "datafile", "", "", "")

   ##Upload with additional form fields
   additionalData <- "subjectID=123&session=1"
   response <- PostHTTPFile("https://secure.example.com/upload", "results.txt", "file", "user", "pass", additionalData)

**See Also:**

:func:`PostHTTP()`, :func:`GetHTTPFile()`, :func:`FileOpenRead()`

