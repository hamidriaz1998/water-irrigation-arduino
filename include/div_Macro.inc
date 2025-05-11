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

;***************************************************************************
;* "mul16u" - 16/16 Bit Unsigned Multiplication
;*
;* This macro multiplies two 16-bit unsigned integers:
;*   - Multiplicand: r16:r17
;*   - Multiplier: r18:r19
;* The result is a 32-bit value:
;*   - Lower 16 bits: r20:r21
;*   - Upper 16 bits: r22:r23
;***************************************************************************
.macro mul16u
    clr r20          ; Clear result lower byte
    clr r21          ; Clear result higher byte
    clr r22          ; Clear upper result lower byte
    clr r23          ; Clear upper result higher byte

    ; Multiply low bytes
    mov r24, r16
    mul r24, r18
    mov r20, r0      ; Store lower byte of result
    mov r21, r1      ; Store higher byte of result
    clr r1           ; Clear r1 as per AVR convention

    ; Multiply low byte of multiplicand with high byte of multiplier
    mov r24, r16
    mul r24, r19
    add r21, r0      ; Add lower byte to result high byte
    adc r22, r1      ; Add carry to upper result lower byte
    clr r1           ; Clear r1

    ; Multiply high byte of multiplicand with low byte of multiplier
    mov r24, r17
    mul r24, r18
    add r21, r0      ; Add lower byte to result high byte
    adc r22, r1      ; Add carry to upper result lower byte
    clr r1           ; Clear r1

    ; Multiply high bytes
    mov r24, r17
    mul r24, r19
    add r22, r0      ; Add lower byte to upper result lower byte
    adc r23, r1      ; Add carry to upper result higher byte
    clr r1           ; Clear r1
.endmacro

;***************************************************************************
;* "div16u" - 16/16 Bit Unsigned Division
;*
;* This macro divides a 16-bit unsigned dividend by a 16-bit unsigned divisor:
;*   - Dividend: r16:r17
;*   - Divisor: r18:r19
;* The result is:
;*   - Quotient: r16:r17
;*   - Remainder: r14:r15
;***************************************************************************
.macro div16u
    clr r14          ; Clear remainder lower byte
    clr r15          ; Clear remainder higher byte
    ldi r20, 16      ; Initialize loop counter for 16 bits

div16u_loop:
    ; Shift left dividend and remainder
    rol r17
    rol r16
    rol r15
    rol r14

    ; Subtract divisor from remainder
    sub r14, r18
    sbc r15, r19

    ; Check if remainder is negative
    brcc div16u_no_restore
    ; If negative, restore remainder and clear carry
    add r14, r18
    adc r15, r19
    clc
    rjmp div16u_continue

div16u_no_restore:
    ; If positive, set carry
    sec

div16u_continue:
    dec r20          ; Decrement loop counter
    brne div16u_loop ; Repeat until all bits are processed
.endmacro



; ***************************************************************************
; *		Macro written by:													*
; *			Syed Tehseen ul Hasan Shah										*
; *			Lecturer, University of Engineering and Technology Lahore.		*
; *			24-December-2023												*
; ***************************************************************************
