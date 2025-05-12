; 16-bit by 8-bit division macro
.macro div16by8
    ; Input:
    ;   r1:r0 = 16-bit dividend
    ;   r25   = 8-bit divisor
    ; Output:
    ;   r16   = quotient (8-bit result)
    ;   r15   = remainder (8-bit)
    
    push r18
    push r19
    push r20
    
    ; Initialize remainder and result
    clr r15        ; Clear remainder
    clr r16        ; Clear quotient
    
    ; Move dividend to working registers
    mov r19, r1    ; High byte
    mov r18, r0    ; Low byte
    
    ; Loop counter (16 iterations for 16 bits)
    ldi r20, 16
    
div16by8_loop:
    ; Shift dividend left through remainder
    lsl r18        ; Shift low byte left, bit 7 goes to carry
    rol r19        ; Shift high byte left with carry
    rol r15        ; Shift carry into remainder
    
    ; Check if we can subtract divisor
    cp r15, r25
    brlo div16by8_skip   ; If remainder < divisor, skip subtraction
    
    ; Subtract divisor from remainder
    sub r15, r25
    inc r16        ; Set result bit to 1
    
div16by8_skip:
    dec r20        ; Decrement loop counter
    brne div16by8_loop
    
    pop r20
    pop r19
    pop r18
.endmacro