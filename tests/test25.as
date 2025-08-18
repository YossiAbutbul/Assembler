; Complex program testing multiple features
.entry MAIN
.entry RESULT
.extern PRINT_FUNC
.extern INPUT_FUNC

; Data section
ARRAY:      .mat [3][3] 1,2,3,4,5,6,7,8,9
COUNTER:    .data 0
RESULT:     .data 0
MESSAGE:    .string "Processing complete"

; Main program
MAIN:       mov #0, r1           ; Initialize counter
            mov #3, r2           ; Array size

PROCESS_LOOP:
            cmp r1, r2           ; Check if done
            bne CONTINUE         ; Continue if not done
            jmp DONE             ; Exit if done

CONTINUE:   mov ARRAY[r0][r0], r0 ; Get diagonal element
            add r3, RESULT       ; Add to result
            inc r1               ; Increment counter
            jmp PROCESS_LOOP     ; Continue loop

DONE:       jsr PRINT_FUNC       ; Call external function
            lea MESSAGE, r4      ; Load message address
            prn #42              ; Print test value
            stop                 ; End program

; Utility functions
HELPER:     inc COUNTER
            rts