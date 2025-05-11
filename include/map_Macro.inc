.include "div_Macro.inc"

.macro map
;***************************************************************************
;* "map" - maps values
;*
;* This macro calculates a mapped value using the formula:
;* mapped = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
;* where:
;* x       - the input value to be mapped
;* in_min  - the lower bound of the input range
;* in_max  - the upper bound of the input range
;* out_min - the lower bound of the output range
;* out_max - the upper bound of the output range
;***************************************************************************

; Input:
;   r24:r25 - x (input value, 16-bit)
;   r26:r27 - in_min (16-bit)
;   r28:r29 - in_max (16-bit)
;   r30:r31 - out_min (16-bit)
;   r18:r19 - out_max (16-bit)
; Output:
;   r24:r25 - mapped value (16-bit)

    ; Step 1: Calculate x - in_min
    movw r20, r24       ; Copy x to r20:r21
    sub r20, r26        ; Subtract low byte of in_min
    sbc r21, r27        ; Subtract high byte of in_min with carry

    ; Step 2: Calculate in_max - in_min
    movw r22, r28       ; Copy in_max to r22:r23
    sub r22, r26        ; Subtract low byte of in_min
    sbc r23, r27        ; Subtract high byte of in_min with carry

    ; Step 3: Calculate out_max - out_min
    movw r16, r18       ; Copy out_max to r16:r17
    sub r16, r30        ; Subtract low byte of out_min
    sbc r17, r31        ; Subtract high byte of out_min with carry

    ; Step 4: Multiply (x - in_min) * (out_max - out_min)
    movw r16, r20       ; Load (x - in_min) into r16:r17
    movw r18, r16       ; Load (out_max - out_min) into r18:r19
    .macro mul16u       ; Use the 16-bit multiplication macro
    movw r24, r20       ; Store lower 16 bits of result in r24:r25

    ; Step 5: Divide the result by (in_max - in_min)
    movw r16, r24       ; Load dividend into r16:r17
    movw r18, r22       ; Load divisor (in_max - in_min) into r18:r19
    .macro div16u       ; Use the 16-bit division macro
    movw r24, r16       ; Store quotient back into r24:r25

    ; Step 6: Add out_min to the result
    add r24, r30        ; Add low byte of out_min
    adc r25, r31        ; Add high byte of out_min with carry

.endm
