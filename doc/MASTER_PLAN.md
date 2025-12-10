# PEBL Development Master Plan

This document tracks major improvements and architectural changes for PEBL.

## Memory Management & Performance

### High Priority

- [ ] **Fix counted_ptr reference counting system**
  - Objects (fonts, labels, widgets) are not being freed when references go out of scope
  - Valgrind confirms fonts leak even when PEBL variables are cleared
  - Need to stress-test counted_ptr system to identify root cause
  - Related files: `src/utility/rc_ptrs.h`, `src/base/Variant.cpp`, `src/base/PComplexData.cpp`

- [ ] **Implement font caching/factory pattern**
  - Currently every `EasyLabel()` creates a new font object, even with identical parameters
  - 100 labels with same font = 100 separate TTF file loads (~3.7MB leaked)
  - Font factory should cache fonts by key: {filename, style, size, fgcolor, bgcolor, aa}
  - Reuse cached fonts when parameters match
  - Location: Add to `src/libs/PEBLObjects.cpp::MakeFont()`
  - **Depends on**: fixing counted_ptr issues first, otherwise cached fonts won't be freed either

### Medium Priority

- [ ] **Investigate sporadic font color rendering bug**
  - Header text occasionally appears wrong color (grey instead of navy)
  - PEBL properties show correct RGB values (0, 0, 128)
  - No memory corruption detected by valgrind
  - Hypothesis: font color pointer management with counted_ptr during rendering
  - May be related to counted_ptr reference counting issues

## Online/Web Platform

- [ ] Revise `GetSubNum()` and `GetNewDataFile()` for online deployment
- [ ] Use MD5-string as natural unique subject number
- [ ] Handle multiple tests of same kind in test chains

## Code Quality

- [ ] Document counted_ptr reference counting mechanism
- [ ] Add memory leak tests to test suite
- [ ] Profile memory usage of common battery tasks

## Documentation

- [ ] Document font factory architecture once implemented
- [ ] Add memory management best practices guide
- [ ] Update PEBL manual with object lifecycle information
