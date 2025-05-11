# ==== Ultimate AVR Makefile ====
# For true hardware hackers only!
# ==============================

.PHONY: all flash clean nuke dance

# Compile that sweet assembly code
all: banner
	@echo "🚀 Assembling your masterpiece..."
	@avra main.asm -o main.hex -l main.lst
	@echo "✨ Assembly complete! Ready to flash some silicon!"

# Flash that code to your microcontroller
flash: banner
	@echo "⚡ Flashing your creation to the chip..."
	@avrdude -p atmega328p -c arduino -P /dev/ttyACM0 -U flash:w:main.hex:i
	@echo "🔥 Flash complete! Your hardware is now alive!"

# Clean up the mess
clean: banner
	@echo "🧹 Sweeping away binary artifacts..."
	@rm -f *.obj *.hex
	@echo "✅ Workspace is squeaky clean!"

# Show a cool banner
banner:
	@echo "\033[1;36m"
	@echo "╔═════════════════════════════════╗"
	@echo "║     EXTREME HARDWARE HACKING    ║"
	@echo "╚═════════════════════════════════╝"
	@echo "\033[0m"

# For when things go totally wrong
nuke:
	@echo "☢️  NUCLEAR OPTION ACTIVATED ☢️"
	@rm -rf *.obj *.hex *.lst *.cof
	@echo "🏜️  Scorched earth policy complete."

# Just for fun
dance:
	@echo "Let's celebrate your hardware skills!"
	@echo "┏(-_-)┛┗(-_-﻿ )┓┗(-_-)┛┏(-_-)┓"
