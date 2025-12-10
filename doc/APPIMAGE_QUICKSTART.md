# PEBL AppImage Quick Start

## For Developers: Building

```bash
# Build AppImage (one command)
make appimage

# Output: PEBL-2.1-x86_64.AppImage (~100MB)
```

That's it! The AppImage is ready to distribute.

---

## For Users: Running

```bash
# 1. Download
wget https://github.com/muellerlab/pebl/releases/latest/download/PEBL-2.1-x86_64.AppImage

# 2. Make executable
chmod +x PEBL-2.1-x86_64.AppImage

# 3. Run
./PEBL-2.1-x86_64.AppImage
```

**First run:** Creates `~/Documents/pebl-exp.2.1/` with all experiments

---

## Usage

```bash
# GUI Launcher (browse experiments)
./PEBL-2.1-x86_64.AppImage

# Run specific experiment
./PEBL-2.1-x86_64.AppImage battery/stroop/stroop.pbl

# Run your own script
./PEBL-2.1-x86_64.AppImage ~/myexperiment.pbl

# Show help
./PEBL-2.1-x86_64.AppImage --help

# With arguments
./PEBL-2.1-x86_64.AppImage test.pbl -s 001 --fullscreen
```

---

## Optional: System Integration

**Add to PATH:**
```bash
mkdir -p ~/bin
mv PEBL-2.1-x86_64.AppImage ~/bin/pebl2
# Add to ~/.bashrc: export PATH="$HOME/bin:$PATH"

# Then run anywhere:
pebl2
pebl2 experiment.pbl
```

**Create desktop shortcut:**
```bash
cat > ~/.local/share/applications/pebl2.desktop <<EOF
[Desktop Entry]
Name=PEBL 2.1
Exec=/home/$USER/bin/pebl2
Icon=pebl2
Type=Application
Categories=Science;Education;
EOF
```

---

## No Installation Needed!

✓ No `sudo apt install`
✓ No compilation
✓ No dependency hunting
✓ Works on any Linux distro
✓ Can run from USB drive
✓ Multiple versions can coexist

---

## Support

- Full manual: Launch PEBL → Help → Manual
- Website: http://pebl.sourceforge.net
- Issues: https://github.com/stmueller/pebl/issues
