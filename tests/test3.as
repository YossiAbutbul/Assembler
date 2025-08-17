; Basic valid assembly with all instruction types
.entry MAIN
.entry DATA_LABEL
.extern EXT_FUNC

MAIN:   mov #10, r1          ; Two operand instruction
        add r1, r2
        sub r3, #-5
        cmp DATA_LABEL, r4
        lea STR, r5

LOOP:   inc r1               ; One operand instructions
        dec r2
        not r3
        clr r4
        jmp END
        bne LOOP
        jsr EXT_FUNC
        red r5
        prn #42

END:    rts                  ; No operand instructions
        stop

STR:    .string "Hello"      ; Data directives
DATA_LABEL: .data 1, 2, 3
MAT:    .mat [2][2] 10,20,30,40