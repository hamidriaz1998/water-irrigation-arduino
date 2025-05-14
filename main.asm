.include "include/m328pdef.inc"
.include "include/delay_Macro.inc"
.include "include/UART_Macros.inc"
.include "include/div_Macro.inc"
.include "include/map_Macro.inc"
.include "include/1602_LCD_Macros.inc"
.include "include/div_16_by_8.inc"
.def A = r16
.def AH = r17
.cseg
.org 0x0000

; Initialize I/O ports
    SBI DDRB,5          ; Set PORTB pin 5 as output for LED
    SBI DDRB,4        ; Set PORTB pin 4 as output for Relay for watering
    CBI PORTB,4         ; Initialize relay to OFF state

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

; Reading Analog value from LDR Sensor
loop:
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
    
    ; Send to serial too
    ; Serial_writeStr
    Serial_writeReg_ASCII r20
    cpi r20,30           ; Check if moisture percentage is below 30%
    brlo PUMP_OFF        ; If dryness < 30%, turn pump off 
    SBI PORTB,4          ; Turn ON the pump if dryness <= 30% 

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
    
    ; Send to serial too
    ; Serial_writeStr
    Serial_writeReg_ASCII r20
    cpi r20,30           ; Compare r20 with 30
    brsh PUMP_ON         ; If r20 >= 30 (dryness above 30%), jump to PUMP_ON 
    CBI PORTB,4          ; Turn OFF the pump if dryness > 30% 
    
    delay 500
    jmp loop
PUMP_ON:
    SBI PORTB,4          ; Turn on the pump

    delay 200
    jmp loop

; String definitions with correct length calculations
day_string: .db "Moisture      ",0  ; 14 characters + null terminator
.equ day_string_len = 14

night_string: .db "Moisture      ",0  ; 14 characters + null terminator
.equ night_string_len = 14