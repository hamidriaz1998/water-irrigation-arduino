.include "include/m328pdef.inc"
.include "include/delay_Macro.inc"
.include "include/UART_Macros.inc"
.include "include/div_Macro.inc"
.include "include/map_Macro.inc"
.include "include/1602_LCD_Macros.inc"
.def A = r16
.def AH = r17
.cseg
.org 0x0000

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

; Reading Analog value from LDR Sensor
loop:
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
    Serial_writeReg_ASCII AH  ; sending the received value to UART
    LCD_send_a_register AH

    LCD_send_a_character 0x3A ; ":"
    LCD_send_a_character 0x20 ; " "
    Serial_writeChar ':'      ; just for formating (e.g. 180: Day Time or 220: NightTime)
    Serial_writeChar ' '
    cpi AH,200           ; compare LDR reading with our desired threshold
    brsh LED_ON          ; jump if same or higher (AH >= 200)
    CBI PORTB,5          ; LED OFF
    ; writes the string "Day Time" to the UART
    LDI ZL, LOW (2 * day_string)
    LDI ZH, HIGH (2 * day_string)
    LDI r20, day_len
    LCD_send_a_string
    Serial_writeStr
    ; Trying map ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    map8 AH, 255, 0, 0, 100 ; output in r27
    LCD_send_a_register r27
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    delay 500
    rjmp loop
LED_ON:
    SBI PORTB,5          ; LED ON
    ; writes the string "Night Time" to the UART
    LDI ZL, LOW (2 * night_string)
    LDI ZH, HIGH (2 * night_string)
    LDI r20, night_len
    LCD_send_a_string
    Serial_writeStr
    delay 500
    rjmp loop
; It is recommanded to define the strings at the end of the code segment.
; Optionally you can use CRLF (carriage return/line feed) characters 0x0D and 0x0A at the
;end of the string.
; The string should be terminated with 0.
; The overall length of the string (including CRLF and ending zero) must be even number of
;bytes.
day_string: .db "Day Time ",0x0D,0x0A,0
day_len: .equ len_day = (2 * (day_len - day_string)) - 1
night_string: .db "Night Time ",0x0D,0x0A,0
night_len: .equ len_night = (2 * (night_len - night_string)) - 1
