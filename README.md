# PEBL - Psychology Experiment Building Language

[![License](https://img.shields.io/badge/License-GPLv2-blue.svg)](COPYING)

PEBL is a free, open-source cross-platform system for designing and running psychological experiments, behavioral tests, and surveys. It provides a powerful programming language optimized for psychology research along with a large library of pre-built psychological tests.

## Features

- **Cross-Platform**: Runs natively on Linux, Windows, macOS, and in web browsers via WebAssembly
- **100+ Pre-Built Tests**: Comprehensive battery of validated psychological assessments
- **Easy to Learn**: Custom programming language designed specifically for experimental psychology
- **Multilingual**: Built-in internationalization support with translations for major languages
- **Data Collection**: Export data to CSV, upload to servers, or collect via web deployment
- **Graphics & Multimedia**: Support for images, audio, video, and advanced visual stimuli
- **Timing Precision**: Millisecond-accurate timing for reaction time studies

## Quick Start

### Installation

**Linux (Native):**
```bash
make main
sudo make install
```

**WebAssembly (Browser):**
```bash
make em
```

### Running Your First Experiment

```bash
# Run a simple reaction time test
bin/pebl2 battery/srt/SimpleRT.pbl

# Run with specific subject number
bin/pebl2 battery/flanker/flanker.pbl -v subnum=101

# Launch the PEBL experiment launcher
bin/pebl2 bin/launcher.pbl
```

## Battery of Tests

PEBL includes a comprehensive battery of over 100 psychological tests including:

- **Attention & Executive Function**: Flanker, Stroop, Go/No-Go, Trail Making, Wisconsin Card Sort
- **Memory**: Digit Span, Corsi Blocks, N-Back, Tower of London/Hanoi
- **Decision Making**: Iowa Gambling Task, Balloon Analog Risk Task, Weather Prediction
- **Perception**: Visual Search, Multiple Object Tracking, Motion Detection
- **Surveys & Scales**: TLX, SUS, Big Five Personality, NASA-TLX, Affect Grid

Browse the [battery/](battery/) directory for the complete collection, or see [doc/BATTERY_MIGRATION_GUIDE.md](doc/BATTERY_MIGRATION_GUIDE.md) for detailed test descriptions.

## Documentation

- **[Getting Started](doc/CLAUDE.md)** - Overview of PEBL architecture and development
- **[Dependencies](doc/DEPENDENCIES.md)** - Required libraries and build dependencies
- **[Testing Guide](doc/TESTING.md)** - How to test PEBL and battery tasks
- **[Token Authentication](doc/TOKEN_AUTHENTICATION.md)** - Web-based data collection setup
- **[Deployment Guide](doc/DEPLOYMENT.md)** - Deploying PEBL experiments online

For AI assistants working with PEBL code, see [Notes_for_Claude_on_Programming_PEBL.txt](Notes_for_Claude_on_Programming_PEBL.txt).

## Creating Your Own Experiments

PEBL experiments are written in the PEBL language (`.pbl` files):

```pebl
define Start(p)
{
  gWin <- MakeWindow("black")

  # Show instructions
  inst <- EasyLabel("Press any key when you see the X",
                     gVideoWidth/2, gVideoHeight/2, gWin, 44)
  Draw()
  WaitForAnyKeyPress()

  # Show stimulus and measure response time
  Hide(inst)
  Draw()
  Wait(1000 + RandomUniform(1000))

  stimulus <- EasyLabel("X", gVideoWidth/2, gVideoHeight/2, gWin, 100)
  Draw()
  time1 <- GetTime()
  WaitForAnyKeyPress()
  time2 <- GetTime()

  rt <- time2 - time1
  MessageBox("Your reaction time: " + rt + " ms", gWin)
}
```

See the [battery/](battery/) directory for hundreds of complete examples.

## Web Deployment

PEBL can be compiled to WebAssembly for running experiments in web browsers:

```bash
# Build WebAssembly version
make em

# Deploy to web server
./deploy-to-online-platform.sh
```

See [doc/TOKEN_AUTHENTICATION.md](doc/TOKEN_AUTHENTICATION.md) for details on web-based data collection.

## Building from Source

### Requirements

- **Native Build**: GCC/Clang, SDL2, SDL2_image, SDL2_ttf, SDL2_net, SDL2_gfx
- **WebAssembly Build**: Emscripten SDK (emsdk)
- **Documentation**: LaTeX (optional, for PDF manual generation)

See [doc/DEPENDENCIES.md](doc/DEPENDENCIES.md) for detailed dependency information.

### Build Targets

```bash
make main              # Build native Linux executable
make em                # Build WebAssembly version
make doc               # Build PDF manual from LaTeX
make install           # Install to /usr/local (or PREFIX)
make clean             # Remove build artifacts
```

## Project Structure

```
pebl/
├── src/               # C++ source code for PEBL interpreter
│   ├── apps/          # Main applications (PEBL.cpp)
│   ├── base/          # Parser, evaluator, core language
│   ├── libs/          # Built-in function libraries
│   ├── objects/       # Graphics objects (windows, shapes, etc.)
│   └── platforms/     # Platform-specific code (SDL, Emscripten)
├── battery/           # Pre-built psychological tests
├── pebl-lib/          # Standard library (.pbl files)
├── media/             # Fonts, images, sounds, word lists
├── doc/               # Documentation
└── bin/               # Compiled executables
```

## Contributing

PEBL is open source and welcomes contributions! Areas where help is especially appreciated:

- Adding new psychological tests to the battery
- Translations for internationalization
- Bug fixes and performance improvements
- Documentation improvements
- Platform-specific testing (Windows, macOS)

## License

PEBL is released under the GNU General Public License v2.0. See [COPYING](COPYING) for details.

The PEBL test battery includes tests from various researchers. Individual tests may have additional citation requirements - see the `.about.txt` files in each test directory.

## Citation

If you use PEBL in your research, please cite:

```
Mueller, S. T. (2014). PEBL: The Psychology Experiment Building Language (Version 2.0) [Computer software]. Retrieved from http://pebl.sourceforge.net
```

## Links

- **Website**: http://pebl.sourceforge.net
- **Documentation**: http://pebl.sourceforge.net/doc.html
- **Mailing List**: pebl-list@lists.sourceforge.net
- **Bug Reports**: https://github.com/[your-username]/pebl/issues

## Support

- **Email**: pebl.exp@gmail.com
- **Community**: Join the PEBL mailing list for questions and discussion
- **Bug Reports**: Please use GitHub issues for bug reports and feature requests

---

**PEBL** - Making psychological research more accessible, one experiment at a time.
