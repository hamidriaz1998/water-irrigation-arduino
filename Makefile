all:
	avra main.asm -o main.hex
	
flash:
	avrdude -p atmega328p -c arduino -P /dev/ttyACM0 -U flash:w:main.hex:i

clean:
	rm *.obj *.hex