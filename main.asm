.include "include/m328pdef.inc"
.include "include/delay_Macro.inc"
.include "include/1602_LCD_Macros.inc"
.cseg
.org 0x0000
LCD_init ; initilize the 16x2 LCD

loop:

LCD_send_a_command 0x01 ; clear the LCD
; Display an integer on LCD
LDI r16, 123
LCD_send_a_register r16
delay 1000
LCD_send_a_command 0x01 ; clear the LCD
; Display character on LCD
; Sending Hello World to LCD character-by-character
LCD_send_a_character 0x48 ; 'H'
LCD_send_a_character 0x45 ; 'E'
LCD_send_a_character 0x4C ; 'L'
LCD_send_a_character 0x4C ; 'L'
LCD_send_a_character 0x4F ; 'O'
LCD_send_a_character 0x20 ; ' ' (space)
LCD_send_a_character 0x57 ; 'W'
LCD_send_a_character 0x4F ; 'O'
LCD_send_a_character 0x52 ; 'R'
LCD_send_a_character 0x4C ; 'L'
LCD_send_a_character 0x44 ; 'D'
LCD_send_a_character 0x21 ; '!'
LCD_send_a_command 0xC0 ; move curser to next line
LCD_send_a_character 0x43 ; 'C'
LCD_send_a_character 0x4F ; 'O'
LCD_send_a_character 0x41 ; 'A'
LCD_send_a_character 0x4C ; 'L'
LCD_send_a_command 0x14 ; move curser one step forward (another way to add space)
LCD_send_a_character 0x4C ; 'L'
LCD_send_a_character 0x41 ; 'A'
LCD_send_a_character 0x42 ; 'B'
delay 1000
LCD_send_a_command 0x01 ; clear the LCD
rjmp loop
; it is recommanded to define the strings at the end of the code segment
; The length of the string must be even number of bytes
hello_string: .db "Tehseen.",0
len: .equ string_len = (2 * (len - hello_string)) - 1
