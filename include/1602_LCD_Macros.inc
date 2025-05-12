; Macros to send Data/Characters and commands to the 16x2 Character LCD

; [List of all the available macros in this file]
;	LCD_init
;	LCD_backlight_OFF
;	LCD_backlight_ON
;	LCD_send_a_command
;	LCD_send_a_character
;	LCD_send_a_register
;	LCD_send_a_register_16bit
;	LCD_send_a_string
;	LCD_LCD_clear
;	LCD_LCD_home
;	div


;Data Pin Connections for 16x2 LCD
; [LCD pins]          [Arduino UNO Pins]
;   RS   ---------------  8 (PB0)
;   E    ---------------  9 (PB1)
;   D4   ---------------  4 (PD4)
;   D5   ---------------  5 (PD5)
;   D6   ---------------  6 (PD6)
;   D7   ---------------  7 (PD7)
;   A   ----------------  13 (PB5)	; Anode pin of LCD Backlight LED


; Macro to initilize the 16x2 LCD
.macro LCD_init
	; Setting pins to Output for LCD
	sbi DDRD,PD4 ; D4
	sbi DDRD,PD5 ; D5
	sbi DDRD,PD6 ; D6
	sbi DDRD,PD7 ; D7
	;Setting LCD Mode selection pins
	sbi DDRB,PB0 ; RS pin of LCD
	sbi DDRB,PB1 ; E pin of LCD
	;Setting LCD Backlight pin
	sbi DDRB,PB5 ; LCD Backlight control pin (BLA or A pin of LCD)

	; SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	; according to datasheet, we need at least 40 ms after power rises above 2.7 V
	; before sending commands. Arduino can turn on way before 4.5 V so we'll wait 50ms
	delay 50
	
	; Now we pull both RS and E pins LOW to begin commands
	cbi PORTB, PB0 ; RS pin of LCD
	cbi PORTB, PB1 ; E pin of LCD

	; put the LCD into 4 bit or 8 bit mode
	; this is according to the Hitachi HD44780 datasheet
	; figure 24, pg 46
	
	; we start in 8bit mode, try to set 4 bit mode
	LCD_send_a_command 0x03
	delay 5	; wait min 4.1ms

	; second try
	LCD_send_a_command 0x03
	delay 5	; wait min 4.1ms
		
	; third go!
	LCD_send_a_command 0x03
	delay 1
	
	; finally, set to 4-bit interface
	LCD_send_a_command 0x02

	; finally, set # lines, font size, etc.
	LCD_send_a_command 0x28

	; turn the display on with no cursor or blinking default
	LCD_send_a_command 0x0C
	
	; clear it off
	LCD_clear

	; Initialize to default text direction
	LCD_send_a_command 0x06
.endmacro

; Macro to clear the LCD
.macro LCD_clear
	LCD_send_a_command 0x01
	delay 10  ; this command takes a long time!
.endmacro

; Macro to set LCD cursor position to zero
.macro LCD_home
	LCD_send_a_command 0x02
	delay 10  ; this command takes a long time!
.endmacro

; Macro to Turn OFF the LCD Backlight
.macro LCD_backlight_OFF
	cbi PORTB,PB5 ; Backlight OFF
.endmacro

; Macro to Turn ON the LCD Backlight
.macro LCD_backlight_ON
	sbi PORTB,PB5 ; Backlight ON
.endmacro

; Macro to send a command to control the LCD
; Usage Example: LCD_send_a_command 0x01
; all the supported commands are writtern at the end of this file
.macro LCD_send_a_command
	PUSH r16
	LDI r16,@0			; receives the command
	; Clear the pins
	CBI PORTD, PD7         ; Clear PD7
	CBI PORTD, PD6         ; Clear PD6
	CBI PORTD, PD5         ; Clear PD5
	CBI PORTD, PD4         ; Clear PD4
	CBI PORTB, PB0         ; Set RS pin to LOW (set LCD mode to "Command Mode")
	; Send the upper 4 bits of the Command to LCD
	SBRC r16, 7            ; Check the MSB (7th bit)
	SBI PORTD, PD7         ; Set PD7 according to the command bit
	SBRC r16, 6            ; Check the 6th bit
	SBI PORTD, PD6         ; Set PD6 according to the command bit
	SBRC r16, 5            ; Check the 5th bit
	SBI PORTD, PD5         ; Set PD5 according to the command bit
	SBRC r16, 4            ; Check the 4th bit
	SBI PORTD, PD4         ; Set PD4 according to the command bit

	SBI PORTB, PB1         ; Set E pin to HIGH (set LCD to receive the command)
	delay 10
	CBI PORTB, PB1         ; Set E pin to LOW (set LCD to process the command)

	; Clear the pins
	CBI PORTD, PD7         ; Clear PD7
	CBI PORTD, PD6         ; Clear PD6
	CBI PORTD, PD5         ; Clear PD5
	CBI PORTD, PD4         ; Clear PD4

	; Send the lower 4 bits of the Command to LCD
	SBRC r16, 3            ; Check the 3rd bit
	SBI PORTD, PD7         ; Set PD7 according to the command bit
	SBRC r16, 2            ; Check the 2nd bit
	SBI PORTD, PD6         ; Set PD6 according to the command bit
	SBRC r16, 1            ; Check the 1st bit
	SBI PORTD, PD5         ; Set PD5 according to the command bit
	SBRC r16, 0            ; Check the 0th bit
	SBI PORTD, PD4         ; Set PD4 according to the command bit

	SBI PORTB, PB1         ; Set E pin to HIGH (set LCD to receive the command)
	delay 10
	CBI PORTB, PB1         ; Set E pin to LOW (set LCD to process the command)

	; Clear the pins
	CBI PORTD, PD7         ; Clear PD7
	CBI PORTD, PD6         ; Clear PD6
	CBI PORTD, PD5         ; Clear PD5
	CBI PORTD, PD4         ; Clear PD4
	
	POP r16
.endmacro

; Macro to send a single character to display on the LCD
; Usage Example: LCD_send_a_character 'A'
.macro LCD_send_a_character
	PUSH r16
	LDI r16,@0			; receives the character
	; Clear the pins
	CBI PORTD, PD7         ; Clear PD7
	CBI PORTD, PD6         ; Clear PD6
	CBI PORTD, PD5         ; Clear PD5
	CBI PORTD, PD4         ; Clear PD4
	SBI PORTB, PB0         ; Set RS pin to HIGH (set LCD mode to "Data Mode")
	; Send the upper 4 bits of the Character to LCD
	SBRC r16, 7            ; Check the MSB (7th bit)
	SBI PORTD, PD7         ; Set PD7 according to the data bit
	SBRC r16, 6            ; Check the 6th bit
	SBI PORTD, PD6         ; Set PD6 according to the data bit
	SBRC r16, 5            ; Check the 5th bit
	SBI PORTD, PD5         ; Set PD5 according to the data bit
	SBRC r16, 4            ; Check the 4th bit
	SBI PORTD, PD4         ; Set PD4 according to the data bit

	SBI PORTB, PB1         ; Set E pin to HIGH (set LCD to receive the data)
	delay 10
	CBI PORTB, PB1         ; Set E pin to LOW (set LCD to process the data)

	; Clear the pins
	CBI PORTD, PD7         ; Clear PD7
	CBI PORTD, PD6         ; Clear PD6
	CBI PORTD, PD5         ; Clear PD5
	CBI PORTD, PD4         ; Clear PD4

	; Send the lower 4 bits of the Character to LCD
	SBRC r16, 3            ; Check the 3rd bit
	SBI PORTD, PD7         ; Set PD7 according to the data bit
	SBRC r16, 2            ; Check the 2nd bit
	SBI PORTD, PD6         ; Set PD6 according to the data bit
	SBRC r16, 1            ; Check the 1st bit
	SBI PORTD, PD5         ; Set PD5 according to the data bit
	SBRC r16, 0            ; Check the 0th bit
	SBI PORTD, PD4         ; Set PD4 according to the data bit

	SBI PORTB, PB1         ; Set E pin to HIGH (set LCD to receive the data)
	delay 10
	CBI PORTB, PB1         ; Set E pin to LOW (set LCD to process the data)

	; Clear the pins
	CBI PORTD, PD7         ; Clear PD7
	CBI PORTD, PD6         ; Clear PD6
	CBI PORTD, PD5         ; Clear PD5
	CBI PORTD, PD4         ; Clear PD4
	
	POP r16
.endmacro


; Macro to send a register to display its integer value on the LCD
; Usage Example: LCD_send_a_register r16
.macro LCD_send_a_register
	PUSH r15
	PUSH r16
	PUSH r17
	PUSH r18
	PUSH r19
	PUSH r28
	PUSH r29
	PUSH r30

	MOV r16,@0			; receives the register
	mov r19, r16		; take backup of origional value to r19
	
	; Convert the 8-bit (0-255) integer to ASCII encoded integer
	; by simply spliting it and adding 48 to its individual digits then
	; print those individual digits to LCD

	; Split the integer into individual digits
	ldi r28, 0          ; Initialize r28 (ones place) to 0
	ldi r29, 0          ; Initialize r29 (tens place) to 0
	ldi r30, 0          ; Initialize r30 (hundreds place) to 0

	; Ones place
	ldi r17, 10         ; Load divisor (10) into r17
	div					; Divide r16 by 10, result in r16, remainder in r15
	mov r28, r15        ; Move remainder (ones place) to r28

	; Tens place
	ldi r17, 10         ; Load divisor (10) into r17
	div			        ; Divide r16 by 10, result in r16, remainder in r15
	mov r29, r15        ; Move remainder (tens place) to r29

	; Hundreds p
	ldi r17, 10         ; Load divisor (10) into r17
	div			        ; Divide r16 by 10, result in r16, remainder in r15
	mov r30, r15        ; Move remainder (hundreds place) to r30

	; Now r28 has the ones place, r29 has the tens place, and r30 has the hundreds place
	; e.g. integer 235 will split into --> r30=2 , r29=3 , r28=5

	LDI r18, 0	; counter for the loop logic
	LDI r17, 48 ; 48 will be added to each digit to encode it to ASCII integer
	
	; loop to send all 3 register r30, r29 and r28 to LCD one by one
	LCD_send_a_register_writing:
	cpi r18, 0
	BREQ LCD_send_a_register_writing_L1
	cpi r18, 1
	BREQ LCD_send_a_register_writing_L2
	cpi r18, 2
	BREQ LCD_send_a_register_writing_L3
	rjmp LCD_send_a_register_writing_end

	; sending r30 to LCD
	LCD_send_a_register_writing_L1:
	CPI r19, 100
	BRLO LCD_send_a_register_writing_skip_temp	; if the origional number was from 0 to 99, then skip sending r30 to LCD
	add r30, r17
	mov r16, r30
	rjmp LCD_send_a_register_writing_start

	; sending r29 to LCD
	LCD_send_a_register_writing_L2:
	CPI r19, 10
	BRLO LCD_send_a_register_writing_skip_temp	; if the origional number was from 0 to 9, then skip sending r29 as well
	add r29, r17
	mov r16, r29
	rjmp LCD_send_a_register_writing_start

	; sending r28 to LCD
	LCD_send_a_register_writing_L3:
	add r28, r17
	mov r16, r28
	rjmp LCD_send_a_register_writing_start

	LCD_send_a_register_writing_start:
	; Clear the pins
	CBI PORTD, PD7         ; Clear PD7
	CBI PORTD, PD6         ; Clear PD6
	CBI PORTD, PD5         ; Clear PD5
	CBI PORTD, PD4         ; Clear PD4
	SBI PORTB, PB0         ; Set RS pin to HIGH (set LCD mode to "Data Mode")
	; Send the upper 4 bits of the Register to LCD
	SBRC r16, 7            ; Check the MSB (7th bit)
	SBI PORTD, PD7         ; Set PD7 according to the data bit
	SBRC r16, 6            ; Check the 6th bit
	SBI PORTD, PD6         ; Set PD6 according to the data bit
	SBRC r16, 5            ; Check the 5th bit
	SBI PORTD, PD5         ; Set PD5 according to the data bit
	SBRC r16, 4            ; Check the 4th bit
	SBI PORTD, PD4         ; Set PD4 according to the data bit

	SBI PORTB, PB1         ; Set E pin to HIGH (set LCD to receive the data)
	delay 10
	CBI PORTB, PB1         ; Set E pin to LOW (set LCD to process the data)

	RJMP LCD_send_a_register_writing_skip_temp_end
	LCD_send_a_register_writing_skip_temp:
	RJMP LCD_send_a_register_writing_skip
	LCD_send_a_register_writing_skip_temp_end:

	; Clear the pins
	CBI PORTD, PD7         ; Clear PD7
	CBI PORTD, PD6         ; Clear PD6
	CBI PORTD, PD5         ; Clear PD5
	CBI PORTD, PD4         ; Clear PD4

	; Send the lower 4 bits of the Register to LCD
	SBRC r16, 3            ; Check the 3rd bit
	SBI PORTD, PD7         ; Set PD7 according to the data bit
	SBRC r16, 2            ; Check the 2nd bit
	SBI PORTD, PD6         ; Set PD6 according to the data bit
	SBRC r16, 1            ; Check the 1st bit
	SBI PORTD, PD5         ; Set PD5 according to the data bit
	SBRC r16, 0            ; Check the 0th bit
	SBI PORTD, PD4         ; Set PD4 according to the data bit

	SBI PORTB, PB1         ; Set E pin to HIGH (set LCD to receive the data)
	delay 10
	CBI PORTB, PB1         ; Set E pin to LOW (set LCD to process the data)

	; Clear the pins
	CBI PORTD, PD7         ; Clear PD7
	CBI PORTD, PD6         ; Clear PD6
	CBI PORTD, PD5         ; Clear PD5
	CBI PORTD, PD4         ; Clear PD4
	
	LCD_send_a_register_writing_skip:
	INC r18
	rjmp LCD_send_a_register_writing

	LCD_send_a_register_writing_end:
	POP r30
	POP r29
	POP r28
	POP r19
	POP r18
	POP r17
	POP r16
	POP r15
.endmacro

; Macro to send a 16-bit register to display its integer value on the LCD
; Usage Example: LCD_send_a_register_16bit r16,r17 (where r16:r17 contains the 16-bit value, r16 is MSB)
.macro LCD_send_a_register_16bit
    PUSH r14
    PUSH r15
    PUSH r16
    PUSH r17
    PUSH r18
    PUSH r19
    PUSH r20
    PUSH r21
    PUSH r22
    PUSH r23
    PUSH r24
    PUSH r25
    PUSH r26
    PUSH r27
    PUSH r28
    PUSH r29
    PUSH r30
    
    ; Receive the 16-bit value into r16:r17 (r16 is MSB, r17 is LSB)
    MOV r16, @0      ; MSB
    MOV r17, @1      ; LSB
    
    ; Take backup of original value
    MOV r22, r16     ; MSB backup
    MOV r23, r17     ; LSB backup
    
    ; We'll extract up to 5 digits (0-65535)
    ; Each digit will be stored in r25-r29
    ; r25 = ones, r26 = tens, r27 = hundreds, r28 = thousands, r29 = ten thousands
    
    ; Initialize digit registers
    LDI r25, 0       ; ones place
    LDI r26, 0       ; tens place
    LDI r27, 0       ; hundreds place
    LDI r28, 0       ; thousands place
    LDI r29, 0       ; ten thousands place
    
    ; Divisor for obtaining each digit (10)
    LDI r18, 10      ; LSB of divisor
    LDI r19, 0       ; MSB of divisor
    
    ; Extract ones place
    div16u           ; Divide r16:r17 by r18:r19
    MOV r25, r14     ; remainder LSB is ones place
    
    ; Extract tens place
    div16u           ; Divide quotient by 10
    MOV r26, r14     ; remainder LSB is tens place
    
    ; Extract hundreds place
    div16u           ; Divide quotient by 10
    MOV r27, r14     ; remainder LSB is hundreds place
    
    ; Extract thousands place
    div16u           ; Divide quotient by 10
    MOV r28, r14     ; remainder LSB is thousands place
    
    ; Extract ten thousands place
    div16u           ; Divide quotient by 10
    MOV r29, r14     ; remainder LSB is ten thousands place
    
    ; Now display the digits, skipping leading zeros
    LDI r24, 48      ; ASCII offset for numbers
    
    ; Set up for loop counter
    LDI r20, 0       ; counter for loop logic
    
    ; Create threshold values for comparison
    ; To determine when to start displaying digits
    LDI r16, HIGH(10000)  ; MSB of 10000
    LDI r17, LOW(10000)   ; LSB of 10000
    CP r22, r16      ; Compare original MSB with threshold MSB
    BRLO lcd16_skip_10000  ; If lower, skip
    BRNE lcd16_display_all ; If higher, display all digits
    CP r23, r17      ; If equal MSB, compare LSB
    BRLO lcd16_skip_10000  ; If lower, skip
    
    ; Display all 5 digits
    lcd16_display_all:
    LDI r21, 5       ; Display all 5 digits
    RJMP lcd16_display_start
    
    lcd16_skip_10000:
    LDI r16, HIGH(1000)  ; MSB of 1000
    LDI r17, LOW(1000)   ; LSB of 1000
    CP r22, r16      ; Compare original MSB with threshold MSB
    BRLO lcd16_skip_1000   ; If lower, skip
    BRNE lcd16_display_4   ; If higher, display 4 digits
    CP r23, r17      ; If equal MSB, compare LSB
    BRLO lcd16_skip_1000   ; If lower, skip
    
    ; Display 4 digits
    lcd16_display_4:
    LDI r21, 4       ; Display 4 digits
    RJMP lcd16_display_start
    
    lcd16_skip_1000:
    LDI r16, HIGH(100)   ; MSB of 100
    LDI r17, LOW(100)    ; LSB of 100
    CP r22, r16      ; Compare original MSB with threshold MSB
    BRLO lcd16_skip_100    ; If lower, skip
    BRNE lcd16_display_3   ; If higher, display 3 digits
    CP r23, r17      ; If equal MSB, compare LSB
    BRLO lcd16_skip_100    ; If lower, skip
    
    ; Display 3 digits
    lcd16_display_3:
    LDI r21, 3       ; Display 3 digits
    RJMP lcd16_display_start
    
    lcd16_skip_100:
    LDI r16, HIGH(10)    ; MSB of 10
    LDI r17, LOW(10)     ; LSB of 10
    CP r22, r16      ; Compare original MSB with threshold MSB
    BRLO lcd16_display_1   ; If lower, display 1 digit
    BRNE lcd16_display_2   ; If higher, display 2 digits
    CP r23, r17      ; If equal MSB, compare LSB
    BRLO lcd16_display_1   ; If lower, display 1 digit
    
    ; Display 2 digits
    lcd16_display_2:
    LDI r21, 2       ; Display 2 digits
    RJMP lcd16_display_start
    
    ; Display 1 digit
    lcd16_display_1:
    LDI r21, 1       ; Display 1 digit
    
    ; Start displaying digits
    lcd16_display_start:
    
    ; Loop through digits
    lcd16_display_loop:
    CPI r20, 0
    BREQ lcd16_display_10000
    CPI r20, 1
    BREQ lcd16_display_1000
    CPI r20, 2
    BREQ lcd16_display_100
    CPI r20, 3
    BREQ lcd16_display_10
    CPI r20, 4
    BREQ lcd16_display_1
    RJMP lcd16_display_end
    
    lcd16_display_10000:
    CP r20, r21
    BRSH lcd16_display_skip
    MOV r16, r29
    ADD r16, r24
    RJMP lcd16_send_digit
    
    lcd16_display_1000:
    CP r20, r21
    BRSH lcd16_display_skip
    MOV r16, r28
    ADD r16, r24
    RJMP lcd16_send_digit
    
    lcd16_display_100:
    CP r20, r21
    BRSH lcd16_display_skip
    MOV r16, r27
    ADD r16, r24
    RJMP lcd16_send_digit
    
    lcd16_display_10:
    CP r20, r21
    BRSH lcd16_display_skip
    MOV r16, r26
    ADD r16, r24
    RJMP lcd16_send_digit
    
    lcd16_display_1:
    CP r20, r21
    BRSH lcd16_display_skip
    MOV r16, r25
    ADD r16, r24
    RJMP lcd16_send_digit
    
    lcd16_send_digit:
    ; Send the digit to LCD (reuse code from original macro)
    ; Clear the pins
    CBI PORTD, PD7         ; Clear PD7
    CBI PORTD, PD6         ; Clear PD6
    CBI PORTD, PD5         ; Clear PD5
    CBI PORTD, PD4         ; Clear PD4
    SBI PORTB, PB0         ; Set RS pin to HIGH (set LCD mode to "Data Mode")
    
    ; Send the upper 4 bits
    SBRC r16, 7            ; Check the MSB (7th bit)
    SBI PORTD, PD7         ; Set PD7 according to the data bit
    SBRC r16, 6            ; Check the 6th bit
    SBI PORTD, PD6         ; Set PD6 according to the data bit
    SBRC r16, 5            ; Check the 5th bit
    SBI PORTD, PD5         ; Set PD5 according to the data bit
    SBRC r16, 4            ; Check the 4th bit
    SBI PORTD, PD4         ; Set PD4 according to the data bit

    SBI PORTB, PB1         ; Set E pin to HIGH
    delay 10
    CBI PORTB, PB1         ; Set E pin to LOW

    ; Clear the pins
    CBI PORTD, PD7         ; Clear PD7
    CBI PORTD, PD6         ; Clear PD6
    CBI PORTD, PD5         ; Clear PD5
    CBI PORTD, PD4         ; Clear PD4

    ; Send the lower 4 bits
    SBRC r16, 3            ; Check the 3rd bit
    SBI PORTD, PD7         ; Set PD7 according to the data bit
    SBRC r16, 2            ; Check the 2nd bit
    SBI PORTD, PD6         ; Set PD6 according to the data bit
    SBRC r16, 1            ; Check the 1st bit
    SBI PORTD, PD5         ; Set PD5 according to the data bit
    SBRC r16, 0            ; Check the 0th bit
    SBI PORTD, PD4         ; Set PD4 according to the data bit

    SBI PORTB, PB1         ; Set E pin to HIGH
    delay 10
    CBI PORTB, PB1         ; Set E pin to LOW

    ; Clear the pins
    CBI PORTD, PD7         ; Clear PD7
    CBI PORTD, PD6         ; Clear PD6
    CBI PORTD, PD5         ; Clear PD5
    CBI PORTD, PD4         ; Clear PD4
    
    lcd16_display_skip:
    INC r20
    CPI r20, 5
    BRLT lcd16_display_loop
    
    lcd16_display_end:
    
    POP r30
    POP r29
    POP r28
    POP r27
    POP r26
    POP r25
    POP r24
    POP r23
    POP r22
    POP r21
    POP r20
    POP r19
    POP r18
    POP r17
    POP r16
    POP r15
    POP r14
.endmacro

; Macro to send a string (length <256) to display on the LCD
; the Z register pair should have the address of the buffer
; r20 should have the length of the buffer
;	For example:	
;	LDI ZL, LOW (2 * hello_string)
;	LDI ZH, HIGH (2 * hello_string)
;	LDI r20, string_len
;	
; it is recommanded to define the constants (arrays, strings, etc.) at the end of the code segment
; .db directive is used to declare constants
;
;		hello_string:	.db	"Hello World",0
;		len: .equ	string_len   = (2 * (len - hello_string)) - 1
;
;	Note: the string length should be in even numbers, if your string length is
;		  in odd numbers then add an extra space at the end of the string
;		  to make overall string length to even number.
.macro LCD_send_a_string
	; save the states of R16 and R17
	PUSH            R16
	PUSH            R17
	PUSH			R20

	LCD_writeBuffer_LOOP:
		; load the current byte/character pointed to be Z and increment the Z pointer
		LPM             r16, Z+
		; check if the remaining size of the string is non-zero and return if it is
		CPI             r20, 0
		BREQ            LCD_writeBuffer_END
		LCD_writeBuffer_CHAR:
	
		; Send the string character to the LCD
		; Clear the pins
		CBI PORTD, PD7         ; Clear PD7
		CBI PORTD, PD6         ; Clear PD6
		CBI PORTD, PD5         ; Clear PD5
		CBI PORTD, PD4         ; Clear PD4
		SBI PORTB, PB0         ; Set RS pin to HIGH (set LCD mode to "Data Mode")
		; Send the upper 4 bits of the Register to LCD
		SBRC r16, 7            ; Check the MSB (7th bit)
		SBI PORTD, PD7         ; Set PD7 according to the data bit
		SBRC r16, 6            ; Check the 6th bit
		SBI PORTD, PD6         ; Set PD6 according to the data bit
		SBRC r16, 5            ; Check the 5th bit
		SBI PORTD, PD5         ; Set PD5 according to the data bit
		SBRC r16, 4            ; Check the 4th bit
		SBI PORTD, PD4         ; Set PD4 according to the data bit

		SBI PORTB, PB1         ; Set E pin to HIGH (set LCD to receive the data)
		delay 10
		CBI PORTB, PB1         ; Set E pin to LOW (set LCD to process the data)

		; Clear the pins
		CBI PORTD, PD7         ; Clear PD7
		CBI PORTD, PD6         ; Clear PD6
		CBI PORTD, PD5         ; Clear PD5
		CBI PORTD, PD4         ; Clear PD4

		; Send the lower 4 bits of the Register to LCD
		SBRC r16, 3            ; Check the 3rd bit
		SBI PORTD, PD7         ; Set PD7 according to the data bit
		SBRC r16, 2            ; Check the 2nd bit
		SBI PORTD, PD6         ; Set PD6 according to the data bit
		SBRC r16, 1            ; Check the 1st bit
		SBI PORTD, PD5         ; Set PD5 according to the data bit
		SBRC r16, 0            ; Check the 0th bit
		SBI PORTD, PD4         ; Set PD4 according to the data bit

		SBI PORTB, PB1         ; Set E pin to HIGH (set LCD to receive the data)
		delay 10
		CBI PORTB, PB1         ; Set E pin to LOW (set LCD to process the data)

		DEC r20
	RJMP            LCD_writeBuffer_LOOP
	LCD_writeBuffer_END:

	LCD_send_a_command 0x0C ; screen on, Cursor off 
	; restore the states of R16 and R17 and return
	POP				R20
	POP             R17
	POP             R16
.endmacro


; Macro to divide two 8-bit integers
.macro div
;***************************************************************************
;* "div8u" - 8/8 Bit Unsigned Division
;*
;* This macro divides the two register variables "r16" (dividend) and
;* "r17" (divisor).
;* The result (quotient) is placed in "r16" and the remainder in "r15".
;*
;***************************************************************************
; Input:
;		r16		; dividend
;		r17		; divisor
; Output:
;		r15		; remainder
;		r16		; quotient
	PUSH r18

	div8u:
	sub r15,r15		;clear remainder and carry
	ldi r18,9		;init loop counter
	d8u_1:
	rol r16			;shift left dividend
	dec r18			;decrement counter
	brne d8u_2		;if done
	rjmp exit		;return
	d8u_2:
	rol r15			;shift dividend into remainder
	sub r15,r17		;remainder = remainder - divisor
	brcc d8u_3		;if result negative
	add r15,r17		;restore remainder
	clc				;clear carry to be shifted into result
	rjmp d8u_1		;else
	d8u_3:
	sec				;set carry to be shifted into result
	rjmp d8u_1
	exit:

	POP r18
.endmacro



; List of all the supported commands to control the LCD
;
;  [Command HEX Value]            [Command Description]
;		0x01						Clear Display Screen
;		0x30						Function Set: 8-bit, 1 Line, 5x7 Dots
;		0x38						Function Set: 8-bit, 2 Line, 5x7 Dots
;		0x20						Function Set: 4-bit, 1 Line, 5x7 Dots
;		0x28						Function Set: 4-bit, 2 Line, 5x7 Dots
;		0x06						Entry Mode
;		0x08						Display off, Cursor off
;		0x0E						Display on, Cursor on
;		0x0C						Display on, Cursor off
;		0x0F						Display on, Cursor blinking
;		0x18						Shift entire display left
;		0x1C						Shift entire display right
;		0x10						Move cursor left by one character
;		0x14						Move cursor right by one character
;		0x80						Force cursor to beginning of 1st row
;		0xC0						Force cursor to beginning of 2nd row



; ***************************************************************************
; *		Macros written by:													*
; *			Syed Tehseen ul Hasan Shah										*
; *			Lecturer, University of Engineering and Technology Lahore.		*
; *			03-January-2024												*
; ***************************************************************************