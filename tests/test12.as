START:  mov #, r1                ; Invalid immediate (no value)
        mov r8, r2               ; Invalid register (r8 doesn't exist)
        mov invalid_symbol, r3   ; Undefined symbol
        mov MAT[r1], r4          ; Invalid matrix (missing second index)
        mov MAT[r1][r8], r5      ; Invalid matrix (invalid register)
        stop