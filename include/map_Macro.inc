.macro map8
    ; @0 = x
    ; @1 = in_min
    ; @2 = in_max
    ; @3 = out_min
    ; @4 = out_max
    ; r27 = result

    ; Temp usage:
    ; r22 = x - in_min
    ; r23 = out_max - out_min
    ; r24 = mul result
    ; r25 = in_max - in_min

    push r22
    push r23
    push r24
    push r25
    ; Step 1: x - in_min → r22
    mov r22, @0
    sub r22, @1

    ; Step 2: out_max - out_min → r23
    mov r23, @4
    sub r23, @3

    ; Step 3: Multiply r22 * r23 → result in r24
    mul r22, r23        ; result in r1:r0
    mov r24, r0         ; we only need 8-bit result (low byte)

    ; Step 4: in_max - in_min → r25
    mov r25, @2
    sub r25, @1         ; r25 = in_max - in_min

    ; Step 5: Divide (r24 ÷ r25) using your macro
    mov r16, r24        ; dividend
    mov r17, r25        ; divisor
    div                 ; result in r16, remainder in r15

    ; Step 6: Add out_min to quotient
    add r16, @3         ; result = quotient + out_min

    ; Step 7: Store final result
    mov r27, r16
    
    
    pop r25
    pop r24
    pop r23
    pop r22
.endmacro
