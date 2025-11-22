############################################################################################
#      NSIS Installation Script created by NSIS Quick Setup Script Generator v1.09.18
#               Entirely Edited with NullSoft Scriptable Installation System                
#              by Vlasis K. Barkas aka Red Wine red_wine@freemail.gr Sep 2006               
############################################################################################

!define APP_NAME "PEBL"
!define COMP_NAME "PEBL Team"
!define WEB_SITE "http://pebl.sourceforge.net"
!define VERSION "00.14.01.00"
!define COPYRIGHT "Shane T. Mueller, Ph.D., 2014"
!define DESCRIPTION "Application"
!define LICENSE_TXT "..\COPYING"
!define INSTALLER_NAME "..\PEBL_setup.0.14.exe"
!define MAIN_APP_EXE "pebl.exe"
!define INSTALL_TYPE "SetShellVarContext all"
!define REG_ROOT "HKCU"
!define REG_APP_PATH "Software\Microsoft\Windows\CurrentVersion\App Paths\${MAIN_APP_EXE}"
!define UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
!define USERDIR "$DOCUMENTS\pebl-exp.0.14"


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
InstallDir "$PROGRAMFILES\PEBL"

######################################################################
!define MULTIUSER_EXECUTIONLEVEL Highest
;!define MULTIUSER_NOUNINSTALL ;Uncomment if no uninstaller is created

!include "MUI.nsh"
!include MultiUser.nsh
!include MUI2.nsh

;!define MULTIUSER_INSTALLMODEPAGE_TEXT_TOP 	"Text to display on the top of the page."
;!define MULTIUSER_INSTALLMODEPAGE_TEXT_ALLUSERS 	"Text to display on the combo button for a per-machine installation."
;!define MULTIUSER_INSTALLMODEPAGE_TEXT_CURRENTUSER 	"Text to display on the combo button for a per-user installation. "

;!insertmacro MULTIUSER_PAGE_INSTALLMODE
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES 

!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING

!insertmacro MUI_PAGE_WELCOME

!ifdef LICENSE_TXT
!insertmacro MUI_PAGE_LICENSE "${LICENSE_TXT}"
!endif

!ifdef REG_START_MENU
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "PEBL"
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


icon "..\devcpp\pebl.ico"
Section -MainProgram
SetShellVarContext all
${INSTALL_TYPE}
SetOverwrite ifnewer

SetOutPath "$INSTDIR\pebl-lib"
File "..\pebl-lib\Design.pbl"
File "..\pebl-lib\Graphics.pbl"
File "..\pebl-lib\Math.pbl"
File "..\pebl-lib\Taguchi.pbl"
File "..\pebl-lib\Utility.pbl"
File "..\pebl-lib\UI.pbl"
File "..\pebl-lib\combinedatafiles.pbl"

SetOutPath "$INSTDIR\media"
File /r "..\media\text"
File /r "..\media\sounds"
File /r "..\media\images"
File /r "..\media\fonts"
#File "..\battery\fileselect.pbl"
SetOutPath "$INSTDIR\doc"
File "..\doc\Colors.txt"
File "..\COPYING"
File "..\doc\mylabel.png"
File "..\doc\pebl-mode-generic.el"
File "..\doc\pman\PEBLManual0.14.pdf"
File "..\doc\PEBLTutorial.html"
File "..\doc\ReleaseNotes.txt"
SetOutPath "$INSTDIR\demo"
File "..\experiments\*.pbl"
File "..\experiments\*.txt"
File "..\demo\*.pbl"
File "..\demo\*.txt"
File "..\demo\test.csv"
SetOutPath "$INSTDIR\bin"
File "..\bin\pebl.exe"
File "..\bin\launcher.pbl"
File "..\dlls\*"
SetOutPath "$INSTDIR\battery"
File "..\battery\..png"
File "..\battery\..about.txt"
File "..\battery\battery.txt"
##Do each test directory separately for greater control
File /r "..\battery\ANT"
File /r "..\battery\BART"
File /r "..\battery\bcst"
File /r "..\battery\brownpeterson"
File /r "..\battery\BST"
File /r "..\battery\changedetection"
File /r "..\battery\clocktest"
File /r "..\battery\connections"
File /r "..\battery\corsi"
File /r "..\battery\crt"
File /r "..\battery\devicemimicry"
File /r "..\battery\dexterity"
File /r "..\battery\donkey"
File /r "..\battery\dotjudgment"
File /r "..\battery\DRM"
File /r "..\battery\dspan"
File /r "..\battery\ebbinghaus"
File /r "..\battery\fileselect.pbl"
File /r "..\battery\fitts"
File /r "..\battery\flanker"
File /r "..\battery\fourchoice"
File /r "..\battery\freerecall"
File /r "..\battery\generation"
File /r "..\battery\globallocal"
File /r "..\battery\gonogo"
File /r "..\battery\hicks"
File /r "..\battery\iat"
File /r "..\battery\iowa"
File /r "..\battery\itemorder"
File /r "..\battery\letterdigit"
File /r "..\battery\lexicaldecision"
File /r "..\battery\luckvogel"
File /r "..\battery\manikin"
File /r "..\battery\matchtosample"
File /r "..\battery\mathproc"
File /r "..\battery\mathtest"
File /r "..\battery\matrixrotation"
File /r "..\battery\mspan"
File /r "..\battery\mullerlyer"
File /r "..\battery\nback"
File /r "..\battery\objectjudgment"
File /r "..\battery\oddball"
File /r "..\battery\ospan"
File /r "..\battery\pairedassociates"
File /r "..\battery\partial-report"
File /r "..\battery\pathmemory"
File /r "..\battery\patterncomparison"
File /r "..\battery\pcards"
File /r "..\battery\pcpt"
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
File /r "..\battery\simon"
File /r "..\battery\spatialcuing"
File /r "..\battery\spatialpriming"
File /r "..\battery\srt"
File /r "..\battery\sternberg"
File /r "..\battery\stroop"
File /r "..\battery\survey"
File /r "..\battery\switcher"
File /r "..\battery\symbolcounter"
File /r "..\battery\tapping"
File /r "..\battery\timetap"
File /r "..\battery\timewall"
File /r "..\battery\toav"
File /r "..\battery\toh"
File /r "..\battery\tol"
File /r "..\battery\tsp"
File /r "..\battery\twocoladd"
File /r "..\battery\typing"
File /r "..\battery\vigilance"
File /r "..\battery\vsearch"
File /r "..\battery\wft"
File /r "..\battery\wpt"




######################################################################
##Not autogenerated::create the pebl-init.txt file to depend on
## the particular locations.
ClearErrors            
## This is no longer needed
#FileOpen $0 ${USERDIR}\pebl-init.txt w
#FileWrite $0 "$INSTDIR\bin\pebl.exe$\r$\n"
#FileWrite $0 "$DOCUMENTS\pebl-exp.0.13$\r$\n"
#FileWrite $0 "Condition|0$\r$\n"
#FileClose $0
######################################################################

SectionEnd

######################################################################

Section -Icons_Reg
SetShellVarContext all
SetOutPath "$INSTDIR"
WriteUninstaller "$INSTDIR\uninstall.exe"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
SetShellVarContext all
CreateDirectory "$SMPROGRAMS\$SM_Folder"
SetOutPath "$INSTDIR"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk" """$INSTDIR\bin\pebl.exe"""
CreateShortCut "$SMPROGRAMS\$SM_Folder\doc.lnk" "$INSTDIR\doc\pman\PEBLManual0.14.pdf"
!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
!endif
!insertmacro MUI_STARTMENU_WRITE_END
!endif

!ifndef REG_START_MENU
SetShellVarContext all
CreateDirectory "$SMPROGRAMS\PEBL"
SetOutPath "$INSTDIR"
CreateShortCut "$SMPROGRAMS\PEBL\${APP_NAME}.lnk" "$INSTDIR\bin\pebl.exe" 
!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\PEBL\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
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

Section Uninstall
${INSTALL_TYPE}
RMDir /r "$INSTDIR\pebl-lib\*"
RMDir /r "$INSTDIR\battery\*"
RMDir /r "$INSTDIR\bin\*"
RMDir /r "$INSTDIR\doc\*"
RMDir /r "$INSTDIR\media\*"
RMDir /r "$INSTDIR\demo\*"


#Delete "$INSTDIR\battery\*"
# Delete "$INSTDIR\${MAIN_APP_EXE}"
# Delete "$INSTDIR\*"
# Delete "$INSTDIR\pebl-lib\Design.pbl"
# Delete "$INSTDIR\pebl-lib\Graphics.pbl"
# Delete "$INSTDIR\pebl-lib\Math.pbl"
# Delete "$INSTDIR\pebl-lib\Taguchi.pbl"
# Delete "$INSTDIR\pebl-lib\Utility.pbl"
# Delete "$INSTDIR\media\text\Consonants.txt"
# Delete "$INSTDIR\media\text\DigitNames.txt"
# Delete "$INSTDIR\media\text\Digits.txt"
# Delete "$INSTDIR\media\text\Letters.txt"
# Delete "$INSTDIR\media\text\Lowercase.txt"
# Delete "$INSTDIR\media\text\LowercaseConsonants.txt"
# Delete "$INSTDIR\media\text\LowercaseVowels.txt"
# Delete "$INSTDIR\media\text\Uppercase.txt"
# Delete "$INSTDIR\media\text\UppercaseConsonants.txt"
# Delete "$INSTDIR\media\text\UppercaseVowels.txt"
# Delete "$INSTDIR\media\text\Vowels.txt"
# Delete "$INSTDIR\media\sounds\boo.wav"
# Delete "$INSTDIR\media\sounds\buzz500ms.wav"
# Delete "$INSTDIR\media\sounds\cheer.wav"
# Delete "$INSTDIR\media\sounds\chirp1.wav"
# Delete "$INSTDIR\media\images\frowney-big.png"
# Delete "$INSTDIR\media\images\frowney-small.png"
# Delete "$INSTDIR\media\images\pebl.bmp"
# Delete "$INSTDIR\media\images\pebl.png"
# Delete "$INSTDIR\media\images\smiley-big.png"
# Delete "$INSTDIR\media\images\smiley-small.png"
# Delete "$INSTDIR\media\fonts\Caliban.ttf"
# Delete "$INSTDIR\media\fonts\Caslon-Black.ttf"
# Delete "$INSTDIR\media\fonts\CaslonBold.ttf"
# Delete "$INSTDIR\media\fonts\CaslonItalic.ttf"
# Delete "$INSTDIR\media\fonts\CaslonRoman.ttf"
# Delete "$INSTDIR\media\fonts\CharisSILB.ttf"
# Delete "$INSTDIR\media\fonts\CharisSILBI.ttf"
# Delete "$INSTDIR\media\fonts\CharisSILI.ttf"
# Delete "$INSTDIR\media\fonts\CharisSILR.ttf"
# Delete "$INSTDIR\media\fonts\COPYRIGHT.TXT"
# Delete "$INSTDIR\media\fonts\DoulosSILR.ttf"
# Delete "$INSTDIR\media\fonts\DejaVuSans.ttf"
# Delete "$INSTDIR\media\fonts\DejaVuSansMono.ttf"
# Delete "$INSTDIR\media\fonts\DejaVuSerif.ttf"
# Delete "$INSTDIR\media\fonts\fontforge-license.txt"
# Delete "$INSTDIR\media\fonts\freefont-COPYING.txt"
# Delete "$INSTDIR\media\fonts\FreeMono.ttf"
# Delete "$INSTDIR\media\fonts\FreeMonoBold.ttf"
# Delete "$INSTDIR\media\fonts\FreeMonoBoldOblique.ttf"
# Delete "$INSTDIR\media\fonts\FreeMonoOblique.ttf"
# Delete "$INSTDIR\media\fonts\FreeSans.ttf"
# Delete "$INSTDIR\media\fonts\FreeSansBold.ttf"
# Delete "$INSTDIR\media\fonts\FreeSansBoldOblique.ttf"
# Delete "$INSTDIR\media\fonts\FreeSansOblique.ttf"
# Delete "$INSTDIR\media\fonts\FreeSerif.ttf"
# Delete "$INSTDIR\media\fonts\FreeSerifBold.ttf"
# Delete "$INSTDIR\media\fonts\FreeSerifBoldItalic.ttf"
# Delete "$INSTDIR\media\fonts\FreeSerifItalic.ttf"
# Delete "$INSTDIR\media\fonts\GenAI102.TTF"
# Delete "$INSTDIR\media\fonts\GenAR102.TTF"
# Delete "$INSTDIR\media\fonts\GenI102.ttf"
# Delete "$INSTDIR\media\fonts\GenR102.ttf"
# Delete "$INSTDIR\media\fonts\Humanistic.ttf"
# Delete "$INSTDIR\media\fonts\OFL.txt"
# Delete "$INSTDIR\media\fonts\README.txt"
# Delete "$INSTDIR\media\fonts\RELEASENOTES.TXT"
# Delete "$INSTDIR\media\fonts\Stimulasia.sfd"
# Delete "$INSTDIR\media\fonts\Stimulasia.ttf"
# Delete "$INSTDIR\media\fonts\Bandal.ttf"
# Delete "$INSTDIR\media\fonts\bandal-license.txt"
# Delete "$INSTDIR\media\fonts\vera-COPYRIGHT.TXT"
# Delete "$INSTDIR\media\fonts\Vera-README.TXT"
# Delete "$INSTDIR\media\fonts\Vera-RELEASENOTES.TXT"
# Delete "$INSTDIR\media\fonts\Vera.ttf"
# Delete "$INSTDIR\media\fonts\VeraBd.ttf"
# Delete "$INSTDIR\media\fonts\VeraBI.ttf"
# Delete "$INSTDIR\media\fonts\VeraIt.ttf"
# Delete "$INSTDIR\media\fonts\VeraMoBd.ttf"
# Delete "$INSTDIR\media\fonts\VeraMoBI.ttf"
# Delete "$INSTDIR\media\fonts\VeraMoIt.ttf"
# Delete "$INSTDIR\media\fonts\VeraMono.ttf"
# Delete "$INSTDIR\media\fonts\VeraSe.ttf"
# Delete "$INSTDIR\media\fonts\VeraSeBd.ttf"
# Delete "$INSTDIR\doc\Colors.txt"
# Delete "$INSTDIR\doc\COPYING"
# Delete "$INSTDIR\doc\mylabel.png"
# Delete "$INSTDIR\doc\pebl-mode-generic.el"
# Delete "$INSTDIR\doc\PEBLTutorial.html"
# Delete "$INSTDIR\doc\ReleaseNotes.txt"
# Delete "$INSTDIR\bin\jpeg.dll"
# Delete "$INSTDIR\bin\launcher.exe"
# Delete "$INSTDIR\bin\libfreetype-6.dll"
# Delete "$INSTDIR\bin\libpng12-0.dll"
# Delete "$INSTDIR\bin\libtiff-3.dll"
# Delete "$INSTDIR\bin\pebl.exe"
# Delete "$INSTDIR\bin\SDL.dll"
# Delete "$INSTDIR\bin\SDL_gfx.dll"
# Delete "$INSTDIR\bin\SDL_image.dll"
# Delete "$INSTDIR\bin\SDL_net.dll"
# Delete "$INSTDIR\bin\SDL_ttf.dll"
# Delete "$INSTDIR\bin\zlib1.dll"

RmDir "$INSTDIR"
#RmDir "$INSTDIR\battery"
#RmDir "$INSTDIR\demo"
#RmDir "$INSTDIR\doc"
#RmDir "$INSTDIR\experiments\stim"
#RmDir "$INSTDIR\experiments"
#RmDir "$INSTDIR\media\fonts"
#RmDir "$INSTDIR\media\images"
#RmDir "$INSTDIR\media\sounds"
#RmDir "$INSTDIR\media\text"
#RmDir "$INSTDIR\pebl-lib"
 
Delete "$INSTDIR\uninstall.exe"
!ifdef WEB_SITE
Delete "$INSTDIR\${APP_NAME} website.url"
!endif

RmDir "$INSTDIR"

!ifdef REG_START_MENU
SetShellVarContext all
!insertmacro MUI_STARTMENU_GETFOLDER "Application" $SM_Folder
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk"
!endif
RmDir "$SMPROGRAMS\$SM_Folder"
!endif

!ifndef REG_START_MENU
Delete "$SMPROGRAMS\PEBL\${APP_NAME}.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\PEBL\${APP_NAME} Website.lnk"
!endif
RmDir "$SMPROGRAMS\PEBL"
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

