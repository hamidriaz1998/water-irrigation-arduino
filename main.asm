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

; Initialize I/O ports
    SBI DDRB,5          ; Set PORTB pin 5 as output for LED

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
    LCD_backlight_ON 
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
    Serial_writeReg_ASCII AH  ; sending the received value to UART
    LCD_send_a_register AH

    LCD_send_a_character 0x3A ; ":"
    LCD_send_a_character 0x20 ; " "
    Serial_writeChar ':'      ; just for formating (e.g. 180: Day Time or 220: NightTime)
    Serial_writeChar ' '
    cpi AH,200           ; compare LDR reading with our desired threshold
    brlo SKIP_LED_ON     ; branch if lower (AH < 200)
    jmp LED_ON           ; use jmp for long range
SKIP_LED_ON:
    CBI PORTB,5          ; LED OFF
    ; writes the string "Day Time" to the UART
    LDI ZL, LOW (2 * day_string)
    LDI ZH, HIGH (2 * day_string)
    LDI r20, day_string_len   ; Fixed string length variable
    LCD_send_a_string
    Serial_writeStr
    
    ; Position cursor on second line
    LCD_send_a_command 0xC0   ; Move to second line
    
    ; Trying map ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ldi r22, 0      ; in_min
    ldi r23, 255    ; in_max
    ldi r24, 0      ; out_min
    ldi r25, 100    ; out_max
    map8 AH, r22, r23, r24, r25  ; output in r27
    
    ; Display percentage value on LCD
    LCD_send_a_register r27
    LCD_send_a_character 0x25   ; '%' character (the % symbol)
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    delay 500
    rjmp loop
LED_ON:
    SBI PORTB,5          ; LED ON
    ; writes the string "Night Time" to the UART
    LDI ZL, LOW (2 * night_string)
    LDI ZH, HIGH (2 * night_string)
    LDI r20, night_string_len  ; Fixed string length variable
    LCD_send_a_string
    Serial_writeStr
    delay 500
    rjmp loop

; String definitions with correct length calculations
day_string: .db "Moisture ",0x0D,0x0A,0
.equ day_string_len = 11        ; Length of "Moisture " + CR + LF + null

night_string: .db "Moisture ",0x0D,0x0A,0
.equ night_string_len = 11      ; Length of "Moisture " + CR + LF + null