; Test 8: Valid References (Should Pass Second Pass)
; Expected: No errors, successful assembly

; === External symbols ===
.extern PRINT_FUNC
.extern INPUT_FUNC

; === Data definitions ===
COUNTER_1: .data 10
RESULT: .data 0
MATRIX: .mat [2][2] 1,2,3,4
MESSAGE: .string "Hello"

; === Instructions with valid references ===
MAIN: mov COUNTER, r0          ; Valid data reference
      add #1, r0               ; Valid immediate
      mov r0, RESULT           ; Valid data reference
      mov MATRIX[r0][r1], r2   ; Valid matrix reference
      jsr PRINT_FUNC           ; Valid external reference
      jmp END                  ; Valid label reference

LOOP: cmp #10, r0              ; Valid immediate
      bne MAIN                 ; Valid label reference
      
; === Entry declarations for defined symbols ===
.entry MAIN
.entry COUNTER
.entry RESULT

; === End ===
END: stop