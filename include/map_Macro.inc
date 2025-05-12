.macro map8
    ; @0 = x (input value to map)
    ; @1 = in_min
    ; @2 = in_max
    ; @3 = out_min
    ; @4 = out_max
    ; r27 = result
    
    push r16
    push r17
    push r18
    push r19
    push r20
    push r21
    push r22
    
    ; Step 1: Calculate (x - in_min)
    mov r16, @0      ; x
    sub r16, @1      ; x - in_min
    
    ; Step 2: Calculate (out_max - out_min)
    mov r17, @4      ; out_max
    sub r17, @3      ; out_max - out_min
    
    ; Step 3: Calculate (x - in_min) * (out_max - out_min)
    mul r16, r17     ; Result in r1:r0
    
    ; Step 4: Calculate (in_max - in_min)
    mov r22, @2      ; in_max
    sub r22, @1      ; in_max - in_min
    
    ; Step 5: Using our division macro for 16-bit / 8-bit
    ; r1:r0 already contains the dividend from mul
    mov r25, r22     ; Move divisor to r25 for div16by8
    
    ; Call the division macro
    div16by8
    
    ; r16 now contains the quotient
    add r16, @3      ; Add out_min to the result
    
    ; Save result to r27
    mov r27, r16
    
    pop r22
    pop r21
    pop r20
    pop r19
    pop r18
    pop r17
    pop r16
.endmacro