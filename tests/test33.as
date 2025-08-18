; Comprehensive test file for improved error messages
; This file should generate specific error messages for each line

; === Data Directive Error Tests ===
DATA1:  .data                    ; ERROR_DATA_NO_VALUES
DATA2:  .data 1,                 ; ERROR_DATA_TRAILING_COMMA  
DATA3:  .data ,1                 ; ERROR_DATA_LEADING_COMMA
DATA4:  .data 1,,2               ; ERROR_DATA_DOUBLE_COMMA
DATA5:  .data 1, ,3              ; ERROR_DATA_EMPTY_VALUE
DATA6:  .data 1 2 3              ; ERROR_MISSING_COMMA
DATA7:  .data -513,512           ; ERROR_DATA_OUT_OF_RANGE (2 errors)
DATA8:  .data 1,   ,5            ; ERROR_DATA_EMPTY_VALUE (spaces between commas)

; === Valid data for comparison ===
DATA_OK: .data -512,511          ; Should be OK

; === String Error Tests ===
STR1:   .string missing quotes   ; ERROR_STRING_MISSING_QUOTES
STR2:   .string "unclosed quote  ; ERROR_STRING_UNCLOSED
STR3:   .string "valid string"   ; Should be OK

; === Entry/Extern Error Tests ===
.entry                           ; ERROR_ENTRY_MISSING_SYMBOL
.extern                          ; ERROR_EXTERN_MISSING_SYMBOL  
.entry MAIN EXTRA                ; ERROR_ENTRY_EXTRA_TEXT
.entry 123INVALID                ; ERROR_INVALID_LABEL
.extern VALID_SYMBOL             ; Should be OK

; === Label on Entry/Extern Tests ===
LABEL1: .entry MAIN              ; ERROR_LABEL_ON_EXTERN (or similar)
LABEL2: .extern EXT              ; ERROR_LABEL_ON_EXTERN

; === Register Error Tests ===
START:  mov r0, r1               ; Should be OK
        mov r8, r1               ; ERROR_INVALID_REGISTER
        mov R1, r2               ; ERROR_INVALID_REGISTER (uppercase)
        mov r-1, r3              ; ERROR_INVALID_REGISTER
        mov r, r4                ; ERROR_INVALID_REGISTER
        mov r01, r5              ; ERROR_INVALID_REGISTER

; === Instruction Error Tests ===
        invalidinst r0, r1       ; ERROR_UNKNOWN_INSTRUCTION
        mov                      ; ERROR_TOO_FEW_OPERANDS
        mov r0, r1, r2           ; ERROR_TOO_MANY_OPERANDS
        stop r0                  ; ERROR_TOO_MANY_OPERANDS

; === Addressing Mode Error Tests ===
        mov #5, #10              ; ERROR_INVALID_TARGET_ADDRESSING
        lea #5, r1               ; ERROR_INVALID_SOURCE_ADDRESSING
        add r0, #5               ; ERROR_INVALID_TARGET_ADDRESSING

; === Immediate Value Error Tests ===
        mov #, r1                ; ERROR_INVALID_IMMEDIATE_VALUE
        mov ##5, r1              ; ERROR_INVALID_IMMEDIATE_VALUE
        mov #abc, r1             ; ERROR_INVALID_IMMEDIATE_VALUE
        mov #1000, r1            ; ERROR_DATA_OUT_OF_RANGE
        mov #-1000, r1           ; ERROR_DATA_OUT_OF_RANGE

; === Matrix Error Tests ===
        mov ARRAY[#5][r0], r1    ; ERROR_MATRIX_IMMEDIATE_NOT_ALLOWED
        mov ARRAY[r8][r0], r1    ; ERROR_MATRIX_INVALID_REGISTER
        mov ARRAY[r0][r9], r1    ; ERROR_MATRIX_INVALID_REGISTER

; === Valid instructions for comparison ===
VALID:  mov #0, r0               ; Should be OK
        mov r0, r1               ; Should be OK
        cmp #5, r1               ; Should be OK
        stop                     ; Should be OK

; === Test data ===
ARRAY:  .mat [3][3] 1,2,3,4,5,6,7,8,9
COUNTER: .data 0
MESSAGE: .string "Hello World"