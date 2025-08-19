; Basic valid assembly with all instruction types
.entry MAIN
.entry DATALABEL
.extern EXTFUNC

MAIN:   mov #10, r1          ; Two operand instruction
        add r1, r2
        sub #-5, r3
        cmp DATALABEL, r4
        lea STR, r5

LOOP:   inc r1               ; One operand instructions
        dec r2
        not r3
        clr r4
        jmp END
        bne LOOP
        jsr EXTFUNC
        red r5
        prn #42

END:    rts                  ; No operand instructions
        stop

STR:    .string "Hello"      ; Data directives
DATALABEL: .data 1, 2, 3
MAT:    .mat [2][2] 10,20,30,40