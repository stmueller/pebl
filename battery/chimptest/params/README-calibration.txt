CHIMP TEST SCREEN CALIBRATION
=============================

The chimp test uses visual angle (degrees) to size stimuli precisely for visual
perception experiments. There are two ways to specify screen calibration:

METHOD 1: INTERNAL CALIBRATION (Default)
-----------------------------------------

Use the built-in calibration tool to measure your screen:

    "calibrateScreen": "internal"

This will:
- Display a credit card sized rectangle on screen
- Prompt user to measure it with a physical credit card
- Calculate pixels per centimeter automatically
- Override the pixelsPerCm parameter with measured value

Example parameter file:

{
  "boxSizeDegrees": 1.0,
  "eyeDistance": 18.0,
  "calibrateScreen": "internal"
}

METHOD 2: LAB SETUP WITH KNOWN VALUES
--------------------------------------

For lab setups where display properties are known, specify calibration values
directly and skip the calibration step:

    "calibrateScreen": "none"
    "pixelsPerCm": 37.8
    "eyeDistance": 24.0

This is useful when:
- You have already measured your display's pixel density
- Running many sessions on the same hardware
- Using standardized lab equipment

Example parameter file (see chimptest-labsetup.par.json):

{
  "boxSizeDegrees": 1.0,
  "pixelsPerCm": 37.8,
  "eyeDistance": 24.0,
  "calibrateScreen": "none"
}

CALCULATING PIXELS PER CM
--------------------------

To find pixelsPerCm for your display:

1. Measure screen width in centimeters
2. Find screen resolution in pixels (e.g., 1920×1080)
3. Calculate: pixelsPerCm = horizontal_pixels / screen_width_cm

Example:
- Screen: 50.8 cm wide (20 inches)
- Resolution: 1920 pixels wide
- pixelsPerCm = 1920 / 50.8 = 37.8

VISUAL ANGLE PARAMETERS
-----------------------

boxSizeDegrees: Size of each number box in degrees visual angle
  - Default: 1.0 degree
  - Smaller values = smaller boxes (harder task)
  - Larger values = larger boxes (easier task)

eyeDistance: Distance from participant's eyes to screen (inches)
  - Default: 18.0 inches (45.72 cm)
  - Standard viewing distances: 18-24 inches for desktop, 24-36 for large screens
  - This affects the pixel size calculation: smaller distance = larger visual angle

EXAMPLE CONFIGURATIONS
----------------------

Standard desktop setup (internal calibration):
{
  "boxSizeDegrees": 1.0,
  "eyeDistance": 18.0,
  "calibrateScreen": "internal"
}

Lab setup with known display (skip calibration):
{
  "boxSizeDegrees": 1.0,
  "pixelsPerCm": 37.8,
  "eyeDistance": 24.0,
  "calibrateScreen": "none"
}

Harder task (smaller boxes):
{
  "boxSizeDegrees": 0.75,
  "eyeDistance": 18.0,
  "calibrateScreen": "internal"
}

DATA OUTPUT
-----------

All calibration values are recorded in the output files:
- pixelsPerCm: Measured or specified pixel density
- eyeDistanceCm: Converted from inches to cm
- screenWidthCm, screenHeightCm: Calculated screen dimensions
- boxSizePixels: Actual pixel size of boxes
- boxSizeDegrees: Requested visual angle
- spacing: Pixel spacing between boxes

This allows verification and replication of exact stimulus sizes.
