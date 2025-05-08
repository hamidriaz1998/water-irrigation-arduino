.include "include/m328pdef.inc"
.def	mask 	= r16	; mask register
.def	ledR 	= r17	; led register
.def	oLoopR 	= r18	; outer loop register
.def	iLoopRl = r24	; inner loop register low
.def	iLoopRh = r25	; inner loop register high
.equ	oVal 	= 80	; outer loop value
.equ	iVal 	= 2000	; inner loop value
.cseg
.org	0x00
clr	ledR		; clear led register to zero
ldi	mask,0xFF	; load the mask register 11111111
out	DDRB,mask	; set PORTB to all 8 bits OUT
start:
out	PORTB,ledR	; write led register to PORTB
inc     ledR		; increment ledR from 0 to 255
ldi     oLoopR,oVal	; initialize outer loop count
oLoop:
ldi    iLoopRl,LOW(iVal)	; intialize inner loop count in inner
ldi    iLoopRh,HIGH(iVal)	; loop high and low registers
iLoop:
sbiw    iLoopRl,1	; decrement inner loop register
brne    iLoop		; branch to iLoop if iLoop register != 0
dec	oLoopR		; decrement outer loop register
brne    oLoop		; branch to oLoop if outer loop register != 0
rjmp    start		; jump back to start

