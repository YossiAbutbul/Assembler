; Test all four addressing modes
.extern EXTVAR

START:  mov #42, r1          ; Immediate to register
        mov r1, DIRECTVAR   ; Register to direct
        mov DIRECTVAR, r2   ; Direct to register
        mov MAT[r1][r2], r3  ; Matrix to register
        mov r3, MAT[r4][r5]  ; Register to matrix
        
        add #-10, DIRECTVAR ; Immediate to direct
        cmp MAT[r6][r7], #0  ; Matrix to immediate
        lea MAT, r1          ; Matrix address to register
        
        jmp EXTVAR          ; Jump to external
        stop

DIRECTVAR: .data 25
MAT:    .mat [2][3] 10,20,30,40,50,60