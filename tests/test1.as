; Test basic instructions and data types
.entry MAIN
.entry DATA_VAL
.extern EXTERNAL_FUNC

MAIN:   mov #10, r1
        add r1, r2
        cmp r1, #-5
        sub r3, DATA_VAL
        lea STR, r4
        jmp END

LOOP:   inc r1
        dec r2
        not r3
        clr r4
        red r5
        prn r6
        jsr EXTERNAL_FUNC
        bne LOOP

END:    stop

STR:    .string "Hello World"
DATA_VAL: .data 100, -200, 0
MATRIX: .mat [3][2] 1,2,3,4,5,6