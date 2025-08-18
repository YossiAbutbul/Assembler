; Test 5: Multiple Undefined Symbols
; Expected: Multiple ERROR_UNDEFINED_SYMBOL (or stops at first)

; === Valid definitions ===
VALID_DATA: .data 42
TEST_MATRIX: .mat [2][2] 1,2,3,4

; === Instructions with multiple undefined symbols ===
MAIN: mov FIRST_UNDEFINED, r0     ; First undefined symbol
      add SECOND_UNDEFINED, r1    ; Second undefined symbol  
      lea THIRD_UNDEFINED, r2     ; Third undefined symbol

; === End ===
END: stop