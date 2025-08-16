; Test all four addressing modes
.extern EXT_VAR

START:  mov #42, r1          ; Immediate to register
        mov r1, DIRECT_VAR   ; Register to direct
        mov DIRECT_VAR, r2   ; Direct to register
        mov MAT[r1][r2], r3  ; Matrix to register
        mov r3, MAT[r4][r5]  ; Register to matrix
        
        add #-10, DIRECT_VAR ; Immediate to direct
        cmp MAT[r6][r7], #0  ; Matrix to immediate
        lea MAT, r1          ; Matrix address to register
        
        jmp EXT_VAR          ; Jump to external
        prn #-512            ; Print minimum value
        prn #511             ; Print maximum value
        stop

DIRECT_VAR: .data 25
MAT:    .mat [2][3] 10,20,30,40,50,60