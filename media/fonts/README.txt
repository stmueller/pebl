PEBL Fonts
===========

Fonts in this directory are redistributed with PEBL in accordance with
their original licenses. Fonts are distributed here for convenience, and
presence here should not be taken as being an integral part of the PEBL
system. Any font may be removed from future releases. Each font is freely
redistributable (with some restrictions), and their use does not place
restrictions upon how a PEBL user may license their experiment scripts.

Each set of redistributed fonts has a different license, which are
described below and in the associated license files.


DEJAVU FONTS
============

Information about DejaVu fonts is available at: https://dejavu-fonts.github.io/

DejaVu fonts are based on Bitstream Vera fonts and are distributed under
a permissive license (Bitstream Vera + public domain modifications).
The license is reproduced in dejavu-fonts-LICENSE.txt

DejaVu fonts provide excellent coverage for Western scripts including
Latin, Greek, Cyrillic, Arabic, and Hebrew. They are the default fonts
used by PEBL for most Western language environments.


NOTO FONTS
==========

Information about Noto fonts is available at: https://fonts.google.com/noto

Noto fonts are developed by Google with the goal of supporting all
languages with no missing glyphs ("No Tofu"). They are distributed under
the SIL Open Font License, Version 1.1, reproduced in noto-fonts-LICENSE.txt

Noto fonts provide comprehensive international script coverage including
Arabic, Hebrew, Thai, Devanagari (Hindi/Marathi/Nepali), Bengali, and
CJK (Chinese/Japanese/Korean). PEBL automatically selects appropriate
Noto fonts based on the language setting.


FONTFORGE FONTS
===============

George Williams' fontforge fonts (Humanistic.ttf) are distributed under
a BSD-like license, reproduced in fontforge-license.txt


PEBL CUSTOM FONTS
=================

Stimulasia.ttf is a custom PEBL font with special spacing characters.
It is freely available for any use.

Optician-Sans.otf is a specialized font designed for vision testing,
distributed under an open license.


-----------------------------------------------------------------------

List of fonts distributed with PEBL and their descriptions:

DejaVu Fonts:
 DejaVuSans.ttf             DejaVu Sans Regular
 DejaVuSans-Bold.ttf        DejaVu Sans Bold
 DejaVuSansMono.ttf         DejaVu Sans Mono
 DejaVuSerif.ttf            DejaVu Serif Regular
 DejaVuSerif-Bold.ttf       DejaVu Serif Bold

Noto Fonts (Core Western + International):
 NotoSans-Regular.ttf           Noto Sans Regular
 NotoSans-Bold.ttf              Noto Sans Bold
 NotoSansMono-Regular.ttf       Noto Sans Mono Regular
 NotoSansMono-Bold.ttf          Noto Sans Mono Bold
 NotoSerif-Regular.ttf          Noto Serif Regular
 NotoSerif-Bold.ttf             Noto Serif Bold
 NotoSansArabic-Regular.ttf     Noto Sans Arabic (for Arabic script)
 NotoSansHebrew-Regular.ttf     Noto Sans Hebrew (for Hebrew script)
 NotoSansThai-Regular.ttf       Noto Sans Thai (for Thai script)
 NotoSansDevanagari-Regular.ttf Noto Sans Devanagari (Hindi/Marathi/Nepali)
 NotoSansBengali-Regular.ttf    Noto Sans Bengali (for Bengali script)
 NotoSansGeorgian-Regular.ttf   Noto Sans Georgian (for Georgian script)
 NotoSansCJK-Regular.ttc        Noto Sans CJK (Chinese/Japanese/Korean)

Made by George Williams:
 Humanistic.ttf            Nice interesting sharp font

PEBL Custom Fonts:
 Stimulasia.ttf            Special spacing characters mapped to 0-9 and A-O
                           0-9 are spaces with varying widths (0=1/5 char, 1=10/500, etc.)
 Optician-Sans.otf         Specialized font for vision testing


FONT SELECTION
==============

PEBL automatically selects appropriate fonts based on the language setting.
Three global font variables are available:

 gPEBLBaseFont        - Default sans-serif font for current language
 gPEBLBaseFontMono    - Monospace font for current language
 gPEBLBaseFontSerif   - Serif font for current language

Language-specific font selection (ISO 639-1 codes):
 AR (Arabic)           -> NotoSansArabic
 HE/IW (Hebrew)        -> NotoSansHebrew
 TH (Thai)             -> NotoSansThai
 HI/MR/NE (Devanagari) -> NotoSansDevanagari
 BN (Bengali)          -> NotoSansBengali
 KA (Georgian)         -> NotoSansGeorgian
 ZH/CN/TW (Chinese)    -> NotoSansCJK
 JA/JP (Japanese)      -> NotoSansCJK
 KO/KR/KP (Korean)     -> NotoSansCJK
 Default (Western)     -> DejaVuSans

To use language-specific fonts, set the --language parameter when running
PEBL, or use the PEBL_LANGUAGE environment variable.


VERSION HISTORY
===============

As of PEBL 3.0+ (November 2024):
- Modernized font system to use DejaVu 2.37 + Noto fonts
- Removed obsolete fonts: Bitstream Vera, FreeFonts, SIL fonts (Charis/Doulos/Gentium)
- Removed decorative fonts: Caslon, Caliban
- Removed redundant CJK fonts (fireflysung, wqy-zenhei, UnBatang)
- Added comprehensive Noto font coverage for international scripts
- Improved automatic language-based font selection

Prior to PEBL 3.0:
- Used Bitstream Vera fonts as primary fonts
- Included FreeFonts (FreeSans, FreeMono, FreeSerif)
- Included SIL fonts (CharisSIL, DoulosSIL, Gentium)
- Included various decorative and special-purpose fonts
