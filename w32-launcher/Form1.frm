VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "PEBL Launcher"
   ClientHeight    =   10065
   ClientLeft      =   345
   ClientTop       =   735
   ClientWidth     =   12255
   Icon            =   "Form1.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   10065
   ScaleWidth      =   12255
   Begin VB.TextBox Text4 
      Height          =   375
      Left            =   4440
      TabIndex        =   19
      Text            =   "en"
      Top             =   2040
      Width           =   735
   End
   Begin VB.ComboBox Combo3 
      Height          =   315
      ItemData        =   "Form1.frx":08CA
      Left            =   240
      List            =   "Form1.frx":08D4
      Style           =   2  'Dropdown List
      TabIndex        =   17
      Top             =   1200
      Width           =   1575
   End
   Begin VB.TextBox Text3 
      Height          =   375
      Left            =   2040
      TabIndex        =   15
      Text            =   "0"
      Top             =   1680
      Width           =   2055
   End
   Begin VB.CommandButton Command4 
      Caption         =   "Errors and Messages (stderr.txt)"
      Height          =   375
      Left            =   2880
      TabIndex        =   14
      Top             =   3720
      Width           =   2775
   End
   Begin VB.CommandButton Command3 
      Caption         =   "User Output (stdout.txt)"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   -1  'True
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   240
      TabIndex        =   13
      Top             =   3720
      Width           =   2655
   End
   Begin VB.TextBox Text2 
      BeginProperty Font 
         Name            =   "Lucida Console"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   5775
      Index           =   0
      Left            =   240
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   11
      Top             =   4080
      Width           =   11415
   End
   Begin VB.ComboBox Combo2 
      Height          =   315
      ItemData        =   "Form1.frx":08E9
      Left            =   1920
      List            =   "Form1.frx":08F9
      Style           =   2  'Dropdown List
      TabIndex        =   9
      Top             =   1200
      Width           =   2175
   End
   Begin VB.ComboBox Combo1 
      Height          =   315
      ItemData        =   "Form1.frx":091D
      Left            =   1920
      List            =   "Form1.frx":0936
      Style           =   2  'Dropdown List
      TabIndex        =   8
      Top             =   840
      Width           =   2175
   End
   Begin VB.CheckBox Check1 
      Caption         =   "Fullscreen"
      Height          =   255
      Left            =   360
      TabIndex        =   7
      Top             =   840
      Width           =   1215
   End
   Begin VB.TextBox Text1 
      Height          =   375
      Index           =   0
      Left            =   2040
      TabIndex        =   5
      Text            =   "Text1"
      Top             =   2040
      Visible         =   0   'False
      Width           =   2055
   End
   Begin VB.CommandButton Command2 
      Caption         =   "Quit"
      Height          =   615
      Left            =   1920
      TabIndex        =   4
      Top             =   120
      Width           =   1335
   End
   Begin VB.DirListBox Dir1 
      Height          =   2790
      Left            =   5520
      TabIndex        =   3
      Top             =   840
      Width           =   3255
   End
   Begin VB.DriveListBox Drive1 
      Height          =   315
      Left            =   5520
      TabIndex        =   2
      Top             =   360
      Width           =   3255
   End
   Begin VB.FileListBox File1 
      Archive         =   0   'False
      Height          =   3210
      Left            =   8880
      MultiSelect     =   2  'Extended
      Pattern         =   "*.pbl"
      TabIndex        =   1
      Top             =   360
      Width           =   3255
   End
   Begin VB.CommandButton Command1 
      Caption         =   "Run PEBL Script"
      Height          =   615
      Left            =   240
      TabIndex        =   0
      Top             =   120
      Width           =   1455
   End
   Begin VB.TextBox Text2 
      BeginProperty Font 
         Name            =   "Lucida Console"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   5775
      Index           =   1
      Left            =   240
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   12
      Top             =   4080
      Width           =   11415
   End
   Begin VB.Label Label4 
      Caption         =   "Language Code:"
      Height          =   255
      Left            =   4200
      TabIndex        =   18
      Top             =   1800
      Width           =   1215
   End
   Begin VB.Label Label3 
      Caption         =   "Participant Code:"
      Height          =   375
      Left            =   240
      TabIndex        =   16
      Top             =   1680
      Width           =   1215
   End
   Begin VB.Label Label2 
      Caption         =   "Launcher for PEBL 0.11"
      Height          =   255
      Left            =   10080
      TabIndex        =   10
      Top             =   0
      Width           =   2055
   End
   Begin VB.Label Label1 
      Caption         =   "Label1"
      Height          =   375
      Index           =   0
      Left            =   240
      TabIndex        =   6
      Top             =   2160
      Visible         =   0   'False
      Width           =   1695
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim myPathname
Dim PEBL_Executable As String
Dim exePath As String

Dim MyFilenames
Dim NUMBER
Private SelectedTab As Integer



Private Sub Command1_Click()

 Execute = """" + PEBL_Executable + """"
 
 'add -S command-line argument
   If (Text3.Text = "") Then
      'We don't do anything here.
   Else
      Execute = Execute + " -s " + Text3
   End If
 
 'add any other command-line arguments
  For i = 1 To NUMBER
    If Text1(i) <> "" Then
      Execute = Execute + " -v " + Text1(i)
    End If
   Next i
   
'add any filenames
For j = 0 To File1.ListCount - 1
   
   If File1.Selected(j) Then
    Execute = Execute + " " + """" + File1.Path + "\" + File1.List(j) + """"
   End If
   
Next j
'Debug.Print Check1

'add driver
Select Case Combo3.ListIndex
  Case 0:
     Execute = Execute + " --driver windib"
 
  Case 1:
     Execute = Execute + " --driver directx"

End Select


'add fullscreen mode
If Check1 Then
 Execute = Execute + " --fullscreen "
Else
 Execute = Execute + " --windowed "
End If

'add display size
Execute = Execute + " --display "
Select Case Combo1.ListIndex
Case 0:
 Execute = Execute + " 512x384 "
Case 1:
 Execute = Execute + " 640x480 "
Case 2:
 Execute = Execute + " 800x600 "

Case 3:
 Execute = Execute + " 960x720 "
Case 4:
 Execute = Execute + " 1024x786 "
Case 5:
 Execute = Execute + " 1152x864 "
Case 6:
 Execute = Execute + " 1280x1024 "

Case Else:
Execute = Execute + " 800x600 "
End Select

'add color depth
Execute = Execute + " --depth "
Select Case Combo2.ListIndex
Case 0:
 Execute = Execute + " 15 "
Case 1:
 Execute = Execute + " 16 "
Case 2:
 Execute = Execute + " 24 "

Case 3:
 Execute = Execute + " 32 "

Case Else:
Execute = Execute + " 16 "
End Select

If Text4.Text <> "" Then
 Execute = Execute + " --language " + Text4.Text
End If

Execute = Execute

'  MsgBox Execute
'  MsgBox CurDir()
  
 'execute the command in a separate process, waiting for it to return.
 ExecCmd (Execute)
 
 reload
 
End Sub
Private Sub reload()

 Set fs = CreateObject("Scripting.FileSystemObject")
 
 'Read in the stdout file, line by line
 sFilename = File1.Path & "\stdout.txt"
 If fs.fileexists(sFilename) Then
   hfile = FreeFile
   Open sFilename For Input As #hfile
  alltext = ""
  Do While Not EOF(hfile)
    Line Input #hfile, mytext
    alltext = alltext & mytext & vbCrLf
  Loop
  Text2(0).Text = alltext
   Close #hfile
  Else
   Text2(0).Text = "Experiment complete: No output produced."
 End If
 
 'Do the same for the stderr file.
 sFilename = File1.Path & "\stderr.txt"
 If fs.fileexists(sFilename) Then
  hfile = FreeFile
  Open sFilename For Input As #hfile
  alltext = ""
  Do While Not EOF(hfile)
    Line Input #hfile, mytext
    alltext = alltext & mytext & vbCrLf
  Loop
  Text2(1).Text = alltext
  Close #hfile
  Else
    Text2(1).Text = "Experiment complete.  Failed to load [" & sFilename & "]."
  End If

End Sub

Private Sub Command2_Click()
 End
End Sub

Private Sub Command3_Click()
 Text2(1).Visible = False
 Text2(0).Visible = True
 Text2(0).SetFocus
 Command3.Font.Underline = True
 Command4.Font.Underline = False

 
End Sub

Private Sub Command4_Click()
 Text2(0).Visible = False
 Text2(1).Visible = True
 Text2(1).SetFocus
 Command3.Font.Underline = False
 Command4.Font.Underline = True

End Sub



Private Sub Dir1_Change()
File1.Path = Dir1.Path
ChDir Dir1.Path

End Sub

Private Sub Drive1_Change()
Dir1.Path = Drive1.Drive

End Sub

Private Sub Form_Load()

Combo1.ListIndex = 2
Combo2.ListIndex = 2
Combo3.ListIndex = 0

'Try to find the file pebl-init.txt.  If it doesn't exist,
'no worries
If Len(Dir$("pebl-init.txt")) = 0 Then
  MsgBox ("Unable to find file init file pebl-init.txt in " + CurDir())
  

  End
Else
 Open "pebl-init.txt" For Input As #1
 'The first line is the executable location
 Line Input #1, PEBL_Executable
 'Debug.Print ("PEBL Executable: [" + PEBL_Executable + "]")

 'parse the filename to find out where stdout and stderr will be.
   newpath = Split(PEBL_Executable, "\")
   exePath = ""
   l = UBound(newpath)
    
    For i = 0 To l - 1
       exePath = exePath & newpath(i) & "\"
    Next i
   
 Line Input #1, myPathname
 'Debug.Print ("Pathname: [" + myPathname + "]")

 
 Line Input #1, myfilenamelist
 'Debug.Print ("filename: [" + myfilenamelist + "]")
End If


MyFilenames = Split(myfilenamelist)
'myfilenames is now an array, each containing a filename.
 For j = 0 To UBound(MyFilenames)
   MyFilenames(j) = Mid$(MyFilenames(j), 2, Len(MyFilenames(j)) - 2)
   'Debug.Print MyFilenames(j)
   
 Next j
 

'Set the directory & file selector
 Dir1.Path = myPathname
 ChDir Dir1.Path
 'MsgBox CurDir()
 
 
'Now, go through the file1 selector list, selecting
'any entries listed.
For i = 0 To File1.ListCount
   
   found = False
   For j = 0 To UBound(MyFilenames)
         Debug.Print File1.List(i) + "  " + MyFilenames(j)
         If File1.List(i) = MyFilenames(j) Then
           File1.Selected(i) = True
         End If
   Next j
Next i

NUMBER = 0
toplabel = 2100
While Not EOF(1)

 NUMBER = NUMBER + 1
 
 'Resize the control array:
 Load Label1(NUMBER)
 Load Text1(NUMBER)
 
 
 'Get the label and initial value
 Line Input #1, namevalue
 namevaluearray = Split(namevalue, "|")
 
 Label1(NUMBER).Top = toplabel
 Text1(NUMBER).Top = toplabel
 
 toplabel = toplabel + 500
 
  Label1(NUMBER).Visible = True
  Text1(NUMBER).Visible = True
  
  Label1(NUMBER).Caption = namevaluearray(0)
  Text1(NUMBER).Text = namevaluearray(1)
 
 
 Debug.Print "Stuff: [" + namevalue + "]"
 Debug.Print namevaluearray(0)
 Debug.Print namevaluearray(1)
 
Wend


Text2(0).Text = ""
Text2(1).Text = ""
Text2(0).Visible = True
Close #1

End Sub




Private Sub TabStrip1_Change()

clicked = TabStrip1.SelectedItem.Index
If (clicked = SelectedTab) Then Exit Sub

Text2(clicked).Visible = True
Text2(1 - clicked).Visible = False
SelectedTab = clicked

End Sub

