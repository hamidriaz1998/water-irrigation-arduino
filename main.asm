.include "include/m328pdef.inc"
.include "include/delay_Macro.inc"
.include "include/map_Macro.inc"
.include "include/1602_LCD_Macros.inc"

; Define constants
.equ SENSOR_PIN = 0       ; ADC0 pin
.equ ADC_MAX    = 1023    ; Maximum ADC value
.equ PERCENT_MAX = 100    ; Maximum percentage value

; Define strings for LCD display
.cseg
moisture_text: .db "Moisture: ", 0, 0   ; Extra 0 to make length even
percent_sign: .db " %", 0, 0            ; Extra 0 to make length even
len_moisture: .equ moisture_len = (2 * (percent_sign - moisture_text)) - 2
len_percent: .equ percent_len = 4       ; Length of " %" with padding
value_text: .db "Value: ", 0
len_value: .equ value_len = (2 * (value_text - value_text)) - 2
; .org 0x0000

; Main program
main:
    ; Initialize ADC
    LDI r16, (1<<REFS0)           ; Set reference voltage to AVcc
    STS ADMUX, r16
    LDI r16, (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0) ; Enable ADC, set prescaler to 128
    STS ADCSRA, r16

    LCD_init                      ; Initialize LCD
    LCD_backlight_ON              ; Turn on LCD backlight
    LCD_clear                     ; Clear LCD display

loop:
    LCD_clear                     ; Clear LCD display
    LCD_send_a_command 0x80       ; Set cursor to beginning of first row

    ; Display "Moisture: " text
    LDI ZL, LOW(2*moisture_text)
    LDI ZH, HIGH(2*moisture_text)
    LDI r20, moisture_len
    LCD_send_a_string

    ; Read analog value from ADC
    LDI r16, (1<<REFS0)|SENSOR_PIN ; Select ADC channel with AVcc reference
    STS ADMUX, r16
    LDI r16, (1<<ADEN)|(1<<ADSC)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0) ; Start conversion
    STS ADCSRA, r16
wait_conversion:
    LDS r16, ADCSRA
    SBRC r16, ADSC                ; Wait for conversion to complete (ADSC becomes 0)
    RJMP wait_conversion
    ; x
    LDS r16, ADCL                 ; Read low byte
    LDS r17, ADCH                 ; Read high byte

    CLR r18  ; in_min
    LDI r19, ADC_MAX ; in_max
    CLR r20  ; out_min
    LDI r21, PERCENT_MAX ; out_max
    CLR r22  ; result

    map8 r17, r18, r19, r20, r21, r22


    ; Final percentage is in r16 (lower byte)

    ; Ensure value is within bounds (should be 0-100)
    CPI r22, PERCENT_MAX+1
    BRLO display_value     ; If less than or equal to 100, continue
    LDI r22, PERCENT_MAX   ; Cap at 100% if above

display_value:
    ; Display percentage value using LCD_send_a_register
    LCD_send_a_register r22

    ; Display "%" sign
    LDI ZL, LOW(2*percent_sign)
    LDI ZH, HIGH(2*percent_sign)
    LDI r20, percent_len
    LCD_send_a_string

    delay 2000                    ; Wait 1 second before next reading

    RJMP loop
