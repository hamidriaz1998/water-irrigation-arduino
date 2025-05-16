.include "include/m328pdef.inc"
.include "include/delay_Macro.inc"
.include "include/UART_Macros.inc"
.include "include/div_Macro.inc"
.include "include/map_Macro.inc"
.include "include/1602_LCD_Macros.inc"
.include "include/div_16_by_8.inc"
.def A = r16
.def AH = r17
.def LastCmd = r21      ; New register to store last command received
.def temp8 = r22        ; Changed from r31 (ZH) to r22 to avoid conflict
.def OverrideCounter = r23
.def MAX_OVERRIDE = r24
.cseg
.org 0x0000

; Initialize I/O ports
    SBI DDRB,5          ; Set PORTB pin 5 as output for LED
    SBI DDRB,4        ; Set PORTB pin 4 as output for Relay for watering
    CBI PORTB,4         ; Initialize relay to OFF state
    LDI LastCmd, ' '   ; Initialize LastCmd to space character
    LDI OverrideCounter, 0  ; No override active
    LDI MAX_OVERRIDE, 20    ; 10 loops = about 5 seconds (with your delay)

; ADC Configuration
    LDI A,0b11000111     ; [ADEN ADSC ADATE ADIF ADIE ADIE ADPS2 ADPS1 ADPS0]
    STS ADCSRA,A
    LDI A,0b01100000     ; [REFS1 REFS0 ADLAR – MUX3 MUX2 MUX1 MUX0]
    STS ADMUX,A          ; Select ADC0 (PC0) pin
    SBI PORTC,PC0        ; Enable Pull-up Resistor
    Serial_begin         ; initilize UART serial communication

    LCD_init                      ; Initialize LCD
    LCD_backlight_ON              ; Turn on LCD backlight
    LCD_clear                     ; Clear LCD display
    delay 100                     ; Give LCD time to initialize


; Reading Analog value from Moisture sensor
loop:
    ; First check for any serial commands
    call check_serial_command     ; Check for commands from ESP32
    
    ; LCD_backlight_ON 
    LCD_clear                ; Clear LCD display
    delay 10                 ; Give LCD time to clear
    LDS A,ADCSRA         ; Start Analog to Digital Conversion
    ORI A,(1<<ADSC)
    STS ADCSRA,A
wait:
    LDS A,ADCSRA         ; wait for conversion to complete
    sbrc A,ADSC
    rjmp wait

    LDS A,ADCL           ; Must Read ADCL before ADCH
    LDS AH,ADCH

    delay 100            ; delay 100ms
    ; Serial_writeReg_ASCII AH  ; sending the received value to UART
    LCD_send_a_register AH

    LCD_send_a_character 0x3A ; ":"
    LCD_send_a_character 0x20 ; " "
    ; Serial_writeChar ':'      ; just for formating (e.g. 180: Day Time or 220: NightTime)
    ; Serial_writeChar ' '
    cpi AH,200           ; compare LDR reading with our desired threshold
    brlo SKIP_LED_ON     ; branch if lower (AH < 200)
    jmp LED_ON           ; use jmp for long range
SKIP_LED_ON:
    CBI PORTB,5          ; LED OFF

    
    ; First line - Show raw value
    LCD_send_a_command 0x80   ; Move cursor to first line
    LCD_send_a_character 'V'
    LCD_send_a_character 'a'
    LCD_send_a_character 'l'
    LCD_send_a_character 'u'
    LCD_send_a_character 'e'
    LCD_send_a_character ':'
    LCD_send_a_character ' '
    LCD_send_a_register AH    ; Show raw ADC value
    
    ; Second line - Show percentage
    LCD_send_a_command 0xC0   ; Move cursor to second line
    
    ; Calculate a direct percentage without the map8 function to compare
    mov r20, AH          ; Copy raw value
    com r20              ; Invert (255-value)
    lsr r20              ; Divide by 2
    lsr r20              ; Divide by 2 (now /4)
    lsr r20              ; Now /8
    add r20, r20         ; Multiply by 2 (now /4)
    add r20, r20         ; Multiply by 2 (now /2)
    
    LCD_send_a_character 'M'
    LCD_send_a_character 'o'
    LCD_send_a_character 'i'
    LCD_send_a_character 's'
    LCD_send_a_character 't'
    LCD_send_a_character 'u'
    LCD_send_a_character 'r'
    LCD_send_a_character 'e'
    LCD_send_a_character ':'
    LCD_send_a_character ' '
    LCD_send_a_register r20
    LCD_send_a_character '%'
    LCD_send_a_character ' '     ; First space

    LCD_send_a_register LastCmd  ; Show last command received
    


    ; Check if override is active
    cpi OverrideCounter, 0
    brne skip_auto_control    ; Changed from breq check_moisture_control to inverse logic
    
    ; No override active, do moisture-based control
    ; This is effectively check_moisture_control:
    cpi r20,30           ; Compare moisture percentage with 30%
    brlo PUMP_OFF   ; If dryness < 30%, turn pump off
    sbi PORTB, 4         ; Turn ON the pump if dryness >= 30%
    jmp skip_moisture_control
    
auto_pump_off:
    cbi PORTB, 4         ; Turn off the pump based on moisture reading
    jmp skip_moisture_control

skip_auto_control:
    ; Override is active, decrement counter
    dec OverrideCounter
    
    ; Get last command
    mov r16, LastCmd
    
    ; ON->OFF transition: Force ON for transition period
    cpi r16, 'F'       ; Going to OFF
    brne check_off_to_on
    sbis PORTB, 4      ; Is pump currently ON?
    rjmp keep_pump_on  ; Yes, keep it ON during transition
    rjmp skip_moisture_control  ; No, continue with normal OFF state
    
check_off_to_on:
    ; OFF->ON transition: Force OFF for transition period
    cpi r16, 'N'       ; Going to ON
    brne skip_moisture_control
    sbic PORTB, 4      ; Is pump currently OFF?
    rjmp keep_pump_off ; Yes, keep it OFF during transition
    rjmp skip_moisture_control  ; No, continue with normal ON state
    
keep_pump_on:
    sbi PORTB, 4       ; Force pump ON during transition
    rjmp skip_moisture_control
    
keep_pump_off:
    cbi PORTB, 4       ; Force pump OFF during transition
    rjmp skip_moisture_control

skip_moisture_control:
    delay 500
    jmp loop
PUMP_OFF:
    CBI PORTB,4   ; Turn off the pump
         
    delay 200
    jmp loop
LED_ON:
    SBI PORTB,5          ; LED ON
    

    ; First line - Show raw value (same as SKIP_LED_ON section)
    LCD_send_a_command 0x80   ; Move cursor to first line
    LCD_send_a_character 'V'
    LCD_send_a_character 'a'
    LCD_send_a_character 'l'
    LCD_send_a_character 'u'
    LCD_send_a_character 'e'
    LCD_send_a_character ':'
    LCD_send_a_character ' '
    LCD_send_a_register AH    ; Show raw ADC value
    
    ; Second line - Show percentage (same as SKIP_LED_ON section)
    LCD_send_a_command 0xC0   ; Move cursor to second line
    
    ; Calculate a direct percentage without the map8 function to compare
    mov r20, AH          ; Copy raw value
    com r20              ; Invert (255-value)
    lsr r20              ; Divide by 2
    lsr r20              ; Divide by 2 (now /4)
    lsr r20              ; Now /8
    add r20, r20         ; Multiply by 2 (now /4)
    add r20, r20         ; Multiply by 2 (now /2)
    
    LCD_send_a_character 'M'
    LCD_send_a_character 'o'
    LCD_send_a_character 'i'
    LCD_send_a_character 's'
    LCD_send_a_character 't'
    LCD_send_a_character 'u'
    LCD_send_a_character 'r'
    LCD_send_a_character 'e'
    LCD_send_a_character ':'
    LCD_send_a_character ' '
    LCD_send_a_register r20
    LCD_send_a_character '%'
    LCD_send_a_character ' '     ; First space

    LCD_send_a_register LastCmd  ; Show last command received
    cpi OverrideCounter, 0
    breq skip_override_check
    jmp skip_auto_control 
skip_override_check:
    cpi r20,30           ; Compare r20 with 30
    brsh PUMP_ON         ; If r20 >= 30 (dryness above 30%), jump to PUMP_ON 
    CBI PORTB,4          ; Turn OFF the pump if dryness > 30% 
    
    delay 500
    jmp loop
PUMP_ON:
    SBI PORTB,4          ; Turn on the pump

    delay 200
    jmp loop


; Check for serial commands from ESP32
check_serial_command:
    ; Save registers
    push r16
    push r17
    
    ; Use Serial_read macro to check for and read data
    Serial_read
    
    ; If no data received (r16 = 0), skip processing
    cpi r16, 0
    brne ValidCommand  ; Exit if no data
    rjmp cmd_exit
    
ValidCommand:
    ; Get previous state for transitions
    mov r17, LastCmd    ; Store previous state in r17
    
    ; Process N command (ON)
    cpi r16, 'N'
    brne try_F_command
    
    ; Store new state
    ldi r16, 'N'
    mov LastCmd, r16
    
    ; Check transition: OFF -> ON
    cpi r17, 'F'        ; Was previous state OFF?
    brne normal_N_command
    
    ; OFF -> ON transition: Force ON
    cbi PORTB, 4        ; Turn on pump during transition
    ldi r16, 20        ; Set longer override (50s)
    mov OverrideCounter, r16
    rjmp send_ack_N
    
normal_N_command:
    ; ON -> ON: Use moisture control
    cpi r17, ' '
    brne NormalN
    cbi PORTB, 4        ; Turn on pump during transition
    ldi r16, 20        ; Set longer override (10s)
    mov OverrideCounter, r16
    rjmp send_ack_N
NormalN:
    ldi r16, 1          ; Short override
    mov OverrideCounter, r16
    rjmp send_ack_N
    
send_ack_N:
    ; Send acknowledgment
    Serial_writeChar 'A'
    Serial_writeChar 'N'
    Serial_writeNewLine
    rjmp cmd_exit
    
try_F_command:
    ; Process F command (OFF)
    cpi r16, 'F'
    brne cmd_exit      ; Not a recognized command
    
    ; Store new state
    ldi r16, 'F'
    mov LastCmd, r16
    
    ; Check transition: ON -> OFF
    cpi r17, 'N'        ; Was previous state ON?
    brne normal_F_command
    
    ; ON -> OFF transition: Force Off
    sbi PORTB, 4        ; Turn Off pump during transition
    ldi r16, 20        ; Set longer override (10s)
    mov OverrideCounter, r16
    rjmp send_ack_F
    
normal_F_command:
    ; OFF -> OFF: Use moisture control
    cpi r17 , ' '
    brne NormalF
    sbi PORTB, 4        ; Turn Off pump during transition
    ldi r16, 20        ; Set longer override (10s)
    mov OverrideCounter, r16
    rjmp send_ack_F 
NormalF:
    ldi r16, 1          ; Short override
    mov OverrideCounter, r16
    rjmp send_ack_F
    
send_ack_F:
    ; Send acknowledgment
    Serial_writeChar 'A'
    Serial_writeChar 'F'
    Serial_writeNewLine
    
cmd_exit:
    ; Restore registers
    pop r17
    pop r16
    ret
; String definitions with correct length calculations
day_string: .db "Moisture      ",0  ; 14 characters + null terminator
.equ day_string_len = 14

night_string: .db "Moisture      ",0  ; 14 characters + null terminator
.equ night_string_len = 14