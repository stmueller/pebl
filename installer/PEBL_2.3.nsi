############################################################################################
#      NSIS Installation Script for PEBL 2.3
#      Psychology Experiment Building Language
############################################################################################

!define APP_NAME "PEBL2"
!define COMP_NAME "PEBL Team"
!define WEB_SITE "http://pebl.sourceforge.net"
!define VERSION "02.3.1.00"
!define COPYRIGHT "Shane T. Mueller, Ph.D., 2026"
!define DESCRIPTION "Psychology Experiment Building Language"
!define LICENSE_TXT "..\COPYING"
!define INSTALLER_NAME "..\PEBL_setup.2.3.exe"
!define MAIN_APP_EXE "pebl2.exe"
!define INSTALL_TYPE "SetShellVarContext all"
!define REG_ROOT "HKCU"
!define REG_APP_PATH "Software\Microsoft\Windows\CurrentVersion\App Paths\${MAIN_APP_EXE}"
!define UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
!define USERDIR "$DOCUMENTS\pebl-exp.2.3"



VIProductVersion  "${VERSION}"
VIAddVersionKey "ProductName"  "${APP_NAME}"
VIAddVersionKey "CompanyName"  "${COMP_NAME}"
VIAddVersionKey "LegalCopyright"  "${COPYRIGHT}"
VIAddVersionKey "FileDescription"  "${DESCRIPTION}"
VIAddVersionKey "FileVersion"  "${VERSION}"

######################################################################
SetCompressor ZLIB

######################################################################

Name "${APP_NAME}"
Caption "${APP_NAME}"
OutFile "${INSTALLER_NAME}"
BrandingText "${APP_NAME}"
XPStyle on
InstallDirRegKey "${REG_ROOT}" "${REG_APP_PATH}" ""
InstallDir "$PROGRAMFILES\PEBL2"

######################################################################
!define MULTIUSER_EXECUTIONLEVEL Highest

!include "MUI.nsh"
!include MultiUser.nsh
!include MUI2.nsh

!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING

RequestExecutionLevel admin


Section

    UserInfo::getAccountType
    Pop $0
    StrCmp $0 "Admin" +4
    MessageBox MB_OK "You must run installer as administrator: $0"
    setErrorLevel 740
    quit
    Return

SectionEnd


!insertmacro MUI_PAGE_WELCOME

!ifdef LICENSE_TXT
!insertmacro MUI_PAGE_LICENSE "${LICENSE_TXT}"
!endif

!ifdef REG_START_MENU
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "PEBL2"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${REG_ROOT}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${UNINSTALL_PATH}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${REG_START_MENU}"
!insertmacro MUI_PAGE_STARTMENU Application $SM_Folder
!endif

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!insertmacro MUI_LANGUAGE "English"


icon "pebl2.ico"
Section -MainProgram
SetShellVarContext all
${INSTALL_TYPE}
SetOverwrite ifnewer

######################################################################
## PEBL Library Files
######################################################################
SetOutPath "$INSTDIR\pebl-lib"
File "..\pebl-lib\Debug.pbl"
File "..\pebl-lib\Design.pbl"
File "..\pebl-lib\EM.pbl"
File "..\pebl-lib\Graphics.pbl"
File "..\pebl-lib\HTML.pbl"
File "..\pebl-lib\Layout.pbl"
File "..\pebl-lib\Math.pbl"
File "..\pebl-lib\Transfer.pbl"
File "..\pebl-lib\UI.pbl"
File "..\pebl-lib\Utility.pbl"
File "..\pebl-lib\combinedatafiles.pbl"
File "..\pebl-lib\converttranslations.pbl"
File "..\pebl-lib\customlauncher.pbl"
File "..\pebl-lib\expbuilder.pbl"
File "..\pebl-lib\followdebug.pbl"
File "..\pebl-lib\translatetest.pbl"

######################################################################
## Media Files
######################################################################
SetOutPath "$INSTDIR\media"
File /r "..\media\fonts"
File /r "..\media\images"
File /r "..\media\settings"
File /r "..\media\sounds"
File /r "..\media\text"

######################################################################
## Documentation
######################################################################
SetOutPath "$INSTDIR\doc"
File "..\doc\Colors.txt"
File "..\COPYING"
File "..\doc\mylabel.png"
File "..\doc\pebl-mode-generic.el"
File "..\doc\pman\PEBLManual2.3.pdf"
File "..\doc\PEBLTutorial.html"
File "..\doc\ReleaseNotes.txt"
File "..\Notes_for_LLMs.txt"

######################################################################
## Demo and Tutorials
######################################################################
SetOutPath "$INSTDIR\demo"
File "..\experiments\*.pbl"
File "..\experiments\*.txt"
File "..\demo\*.pbl"
File "..\demo\*.txt"
File "..\demo\test.csv"
SetOutPath "$INSTDIR\demo\tests"
File "..\demo\tests\*"

SetOutPath "$INSTDIR\tutorials"
File /r "..\tutorials"

######################################################################
## Executables and DLLs
######################################################################
SetOutPath "$INSTDIR\bin"
File "..\bin\pebl2.exe"
File "..\bin\pebl-launcher.exe"
File "..\bin\pebl-validator.exe"
File "..\bin\launcher.pbl"
File "..\bin\setparameters.pbl"
File "..\bin\*.dll"

######################################################################
## Battery Tests
######################################################################
SetOutPath "$INSTDIR\battery"
File "..\battery\..png"
File "..\battery\..about.txt"
File "..\battery\battery.txt"

## All battery test directories
File /r "..\battery\affectgrid"
File /r "..\battery\aiming"
#File /r "..\battery\aman"
File /r "..\battery\ANT"
File /r "..\battery\antisaccade"
File /r "..\battery\BART"
File /r "..\battery\bcst"
File /r "..\battery\bcst-64"
File /r "..\battery\bigfive"
File /r "..\battery\BNT"
File /r "..\battery\brownpeterson"
File /r "..\battery\BST"
File /r "..\battery\CANUM"
File /r "..\battery\changedetection"
File /r "..\battery\chimptest"
File /r "..\battery\clocktest"
File /r "..\battery\connections"
#File /r "..\battery\control"
File /r "..\battery\corsi"
File /r "..\battery\crt"
#File /r "..\battery\dccs"
File /r "..\battery\dexterity"
File /r "..\battery\donkey"
File /r "..\battery\dotjudgment"
File /r "..\battery\DRM"
File /r "..\battery\dspan"
File /r "..\battery\dualtask"
File /r "..\battery\ebbinghaus"
#File /r "..\battery\emgonogo"
File /r "..\battery\evenodd"
#File /r "..\battery\figuralfluency"
File /r "..\battery\fitts"
File /r "..\battery\flanker"
File /r "..\battery\fourchoice"
File /r "..\battery\freerecall"
File /r "..\battery\generation"
File /r "..\battery\globallocal"
File /r "..\battery\gonogo"
File /r "..\battery\handedness"
File /r "..\battery\hicks"
File /r "..\battery\iat"
File /r "..\battery\inspection"
File /r "..\battery\iowa"
File /r "..\battery\itemorder"
#File /r "..\battery\jewels"
#File /r "..\battery\kohs"
File /r "..\battery\leftrightsound"
File /r "..\battery\letterdigit"
File /r "..\battery\lexicaldecision"
File /r "..\battery\linejudgment"
File /r "..\battery\luckvogel"
File /r "..\battery\manikin"
File /r "..\battery\matchtosample"
File /r "..\battery\mathproc"
File /r "..\battery\mathtest"
File /r "..\battery\matrixrotation"
File /r "..\battery\maze"
File /r "..\battery\memoryautomaticity"
File /r "..\battery\microscope"
File /r "..\battery\MOT"
File /r "..\battery\movetotarget"
File /r "..\battery\mspan"
File /r "..\battery\mullerlyer"
File /r "..\battery\nback"
File /r "..\battery\objectjudgment"
File /r "..\battery\oddball"
File /r "..\battery\ospan"
File /r "..\battery\pairedassociates"
File /r "..\battery\partial-report"
File /r "..\battery\PASAT"
File /r "..\battery\pathmemory"
File /r "..\battery\patterncomparison"
File /r "..\battery\pcards"
File /r "..\battery\pcpt"
File /r "..\battery\pcpt-ax"
#File /r "..\battery\PCST"
File /r "..\battery\plusminus"
File /r "..\battery\ppvt"
File /r "..\battery\probedigit"
File /r "..\battery\probmonitor"
File /r "..\battery\probrev"
File /r "..\battery\ptracker"
File /r "..\battery\ptrails"
File /r "..\battery\pursuitrotor"
File /r "..\battery\randomgeneration"
File /r "..\battery\RAT"
File /r "..\battery\readingspan"
File /r "..\battery\rotation"
File /r "..\battery\satest"
File /r "..\battery\scales"
File /r "..\battery\screencalibrate"
File /r "..\battery\shapememory"
File /r "..\battery\simon"
File /r "..\battery\SNARC"
File /r "..\battery\spanvariants"
File /r "..\battery\spatialcuing"
#File /r "..\battery\spatialgrid"
File /r "..\battery\spatialpriming"
File /r "..\battery\srt"
#File /r "..\battery\SRRT"
File /r "..\battery\SSSQ"
File /r "..\battery\sternberg"
File /r "..\battery\stroop"
File /r "..\battery\stroop-color"
File /r "..\battery\stroop-number"
File /r "..\battery\stroop-vic"
#File /r "..\battery\stroopM"
File /r "..\battery\SUS"
File /r "..\battery\survey"
File /r "..\battery\switcher"
File /r "..\battery\symbolcounter"
File /r "..\battery\symmetryspan"
File /r "..\battery\tapping"
File /r "..\battery\template"
File /r "..\battery\test-simple"
File /r "..\battery\timetap"
File /r "..\battery\timewall"
File /r "..\battery\tiredness"
File /r "..\battery\TLX"
File /r "..\battery\TNT"
File /r "..\battery\toav"
File /r "..\battery\toh"
File /r "..\battery\tol"
File /r "..\battery\tracking"
File /r "..\battery\tsp"
File /r "..\battery\twocoladd"
File /r "..\battery\typing"
File /r "..\battery\urns"
File /r "..\battery\VAScales"
File /r "..\battery\vigilance"
File /r "..\battery\vsearch"
File /r "..\battery\wft"
File /r "..\battery\wpt"

SectionEnd

######################################################################
## Icons and Registry
######################################################################

Section -Icons_Reg
SetShellVarContext all
SetOutPath "$INSTDIR"
WriteUninstaller "$INSTDIR\uninstall.exe"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
SetShellVarContext all
CreateDirectory "$SMPROGRAMS\$SM_Folder"

SetOutPath "$INSTDIR\bin"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk" "$INSTDIR\bin\pebl2.exe"
CreateShortCut "$SMPROGRAMS\$SM_Folder\PEBL Launcher.lnk" "$INSTDIR\bin\pebl-launcher.exe"

SetOutPath "$INSTDIR"
CreateShortCut "$SMPROGRAMS\$SM_Folder\Documentation.lnk" "$INSTDIR\doc\PEBLManual2.3.pdf"
!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
!endif
!insertmacro MUI_STARTMENU_WRITE_END
!endif

!ifndef REG_START_MENU
SetShellVarContext all
CreateDirectory "$SMPROGRAMS\PEBL2"
CreateDirectory $INSTDIR

SetOutPath "$INSTDIR\bin"
CreateShortCut "$SMPROGRAMS\PEBL2\${APP_NAME}.lnk" "$INSTDIR\bin\pebl2.exe"
CreateShortCut "$SMPROGRAMS\PEBL2\PEBL Launcher.lnk" "$INSTDIR\bin\pebl-launcher.exe"
!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\PEBL2\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
!endif
!endif

WriteRegStr ${REG_ROOT} "${REG_APP_PATH}" "" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayName" "${APP_NAME}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayIcon" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayVersion" "${VERSION}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "Publisher" "${COMP_NAME}"

!ifdef WEB_SITE
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "URLInfoAbout" "${WEB_SITE}"
!endif
SectionEnd

######################################################################
## Uninstaller
######################################################################

Section Uninstall
${INSTALL_TYPE}

# Remove all installed directories recursively
RMDir /r "$INSTDIR\pebl-lib"
RMDir /r "$INSTDIR\battery"
RMDir /r "$INSTDIR\bin"
RMDir /r "$INSTDIR\doc"
RMDir /r "$INSTDIR\media"
RMDir /r "$INSTDIR\demo"
RMDir /r "$INSTDIR\tutorials"

Delete "$INSTDIR\uninstall.exe"
!ifdef WEB_SITE
Delete "$INSTDIR\${APP_NAME} website.url"
!endif

RmDir "$INSTDIR"

!ifdef REG_START_MENU
SetShellVarContext all
!insertmacro MUI_STARTMENU_GETFOLDER "Application" $SM_Folder
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\$SM_Folder\PEBL Launcher.lnk"
Delete "$SMPROGRAMS\$SM_Folder\Documentation.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk"
!endif
RmDir "$SMPROGRAMS\$SM_Folder"
!endif

!ifndef REG_START_MENU
Delete "$SMPROGRAMS\PEBL2\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\PEBL2\PEBL Launcher.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\PEBL2\${APP_NAME} Website.lnk"
!endif
RmDir "$SMPROGRAMS\PEBL2"
!endif

DeleteRegKey ${REG_ROOT} "${REG_APP_PATH}"
DeleteRegKey ${REG_ROOT} "${UNINSTALL_PATH}"
SectionEnd

######################################################################

Function .onInit
  SetShellVarContext all
  !insertmacro MULTIUSER_INIT
FunctionEnd

Function un.onInit
  SetShellVarContext all
  !insertmacro MULTIUSER_UNINIT
FunctionEnd
