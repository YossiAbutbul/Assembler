; Test all valid registers
START:  mov r0, r1
        mov r1, r2
        mov r2, r3
        mov r3, r4
        mov r4, r5
        mov r5, r6
        mov r6, r7
        
; Test invalid registers
        mov r8, r1               ; Should error
        mov r-1, r2              ; Should error
        mov r, r3                ; Should error
        mov r01, r4              ; Should error
        mov R1, r5               ; Case sensitive error