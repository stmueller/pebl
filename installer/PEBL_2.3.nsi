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
## Note: /x data excludes data directories (contain participant data, not for distribution)
File /r /x data "..\battery\affectgrid"
File /r /x data "..\battery\aiming"
#File /r /x data "..\battery\aman"
File /r /x data "..\battery\ANT"
File /r /x data "..\battery\antisaccade"
File /r /x data "..\battery\BART"
File /r /x data "..\battery\bcst"
File /r /x data "..\battery\bcst-64"
File /r /x data "..\battery\bigfive"
File /r /x data "..\battery\BNT"
File /r /x data "..\battery\brownpeterson"
File /r /x data "..\battery\BST"
File /r /x data "..\battery\CANUM"
File /r /x data "..\battery\changedetection"
File /r /x data "..\battery\chimptest"
File /r /x data "..\battery\clocktest"
File /r /x data "..\battery\connections"
#File /r /x data "..\battery\control"
File /r /x data "..\battery\corsi"
File /r /x data "..\battery\crt"
#File /r /x data "..\battery\dccs"
File /r /x data "..\battery\dexterity"
File /r /x data "..\battery\donkey"
File /r /x data "..\battery\dotjudgment"
File /r /x data "..\battery\DRM"
File /r /x data "..\battery\dspan"
File /r /x data "..\battery\dualtask"
File /r /x data "..\battery\ebbinghaus"
#File /r /x data "..\battery\emgonogo"
File /r /x data "..\battery\evenodd"
#File /r /x data "..\battery\figuralfluency"
File /r /x data "..\battery\fitts"
File /r /x data "..\battery\flanker"
File /r /x data "..\battery\fourchoice"
File /r /x data "..\battery\freerecall"
File /r /x data "..\battery\generation"
File /r /x data "..\battery\globallocal"
File /r /x data "..\battery\gonogo"
File /r /x data "..\battery\handedness"
File /r /x data "..\battery\hicks"
File /r /x data "..\battery\iat"
File /r /x data "..\battery\inspection"
File /r /x data "..\battery\iowa"
File /r /x data "..\battery\itemorder"
#File /r /x data "..\battery\jewels"
#File /r /x data "..\battery\kohs"
File /r /x data "..\battery\leftrightsound"
File /r /x data "..\battery\letterdigit"
File /r /x data "..\battery\lexicaldecision"
File /r /x data "..\battery\linejudgment"
File /r /x data "..\battery\luckvogel"
File /r /x data "..\battery\manikin"
File /r /x data "..\battery\matchtosample"
File /r /x data "..\battery\mathproc"
File /r /x data "..\battery\mathtest"
File /r /x data "..\battery\matrixrotation"
File /r /x data "..\battery\maze"
File /r /x data "..\battery\memoryautomaticity"
File /r /x data "..\battery\microscope"
File /r /x data "..\battery\MOT"
File /r /x data "..\battery\movetotarget"
File /r /x data "..\battery\mspan"
File /r /x data "..\battery\mullerlyer"
File /r /x data "..\battery\nback"
File /r /x data "..\battery\objectjudgment"
File /r /x data "..\battery\oddball"
File /r /x data "..\battery\ospan"
File /r /x data "..\battery\pairedassociates"
File /r /x data "..\battery\partial-report"
File /r /x data "..\battery\PASAT"
File /r /x data "..\battery\pathmemory"
File /r /x data "..\battery\patterncomparison"
File /r /x data "..\battery\pcards"
File /r /x data "..\battery\pcpt"
File /r /x data "..\battery\pcpt-ax"
#File /r /x data "..\battery\PCST"
File /r /x data "..\battery\plusminus"
File /r /x data "..\battery\ppvt"
File /r /x data "..\battery\probedigit"
File /r /x data "..\battery\probmonitor"
File /r /x data "..\battery\probrev"
File /r /x data "..\battery\ptracker"
File /r /x data "..\battery\ptrails"
File /r /x data "..\battery\pursuitrotor"
File /r /x data "..\battery\randomgeneration"
File /r /x data "..\battery\RAT"
File /r /x data "..\battery\readingspan"
File /r /x data "..\battery\rotation"
File /r /x data "..\battery\satest"
File /r /x data "..\battery\scales"
File /r /x data "..\battery\screencalibrate"
File /r /x data "..\battery\shapememory"
File /r /x data "..\battery\simon"
File /r /x data "..\battery\SNARC"
File /r /x data "..\battery\spanvariants"
File /r /x data "..\battery\spatialcuing"
#File /r /x data "..\battery\spatialgrid"
File /r /x data "..\battery\spatialpriming"
File /r /x data "..\battery\srt"
#File /r /x data "..\battery\SRRT"
File /r /x data "..\battery\SSSQ"
File /r /x data "..\battery\sternberg"
File /r /x data "..\battery\stroop"
File /r /x data "..\battery\stroop-color"
File /r /x data "..\battery\stroop-number"
File /r /x data "..\battery\stroop-vic"
#File /r /x data "..\battery\stroopM"
File /r /x data "..\battery\SUS"
File /r /x data "..\battery\survey"
File /r /x data "..\battery\switcher"
File /r /x data "..\battery\symbolcounter"
File /r /x data "..\battery\symmetryspan"
File /r /x data "..\battery\tapping"
File /r /x data "..\battery\template"
File /r /x data "..\battery\test-simple"
File /r /x data "..\battery\timetap"
File /r /x data "..\battery\timewall"
File /r /x data "..\battery\tiredness"
File /r /x data "..\battery\TLX"
File /r /x data "..\battery\TNT"
File /r /x data "..\battery\toav"
File /r /x data "..\battery\toh"
File /r /x data "..\battery\tol"
File /r /x data "..\battery\tracking"
File /r /x data "..\battery\tsp"
File /r /x data "..\battery\twocoladd"
File /r /x data "..\battery\typing"
File /r /x data "..\battery\urns"
File /r /x data "..\battery\VAScales"
File /r /x data "..\battery\vigilance"
File /r /x data "..\battery\vsearch"
File /r /x data "..\battery\wft"
File /r /x data "..\battery\wpt"

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
