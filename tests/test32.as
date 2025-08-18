; Test .data edge cases
DATA1:  .data                    ; No values (should error)
DATA2:  .data 1,                 ; Trailing comma (should error)
DATA3:  .data ,1                 ; Leading comma (should error)
DATA4:  .data 1,,2               ; Double comma (should error)
DATA5:  .data 1 2 3              ; Missing commas (should error)
DATA6:  .data -512,511           ; Boundary values
DATA7:  .data -513,512           ; Out of range (should error)
STR1:   .string missing opening quotes"

; Test .entry/.extern edge cases
.entry                           ; Missing symbol (should error)
.extern                          ; Missing symbol (should error)
.entry MAIN EXTRA                ; Extra text (should error)
.entry 123INVALID                ; Invalid symbol name (should error)

LABEL1: .entry MAIN ; Label on .entry (should error/warn)
LABEL2: .extern EXT ; Label on .extern (should error/warn)