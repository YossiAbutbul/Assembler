; Test 2: External Conflict  
; Expected: ERROR_EXTERNAL_CONFLICT

; === Valid definitions ===
VALID_DATA: .data 42
TEST_MATRIX: .mat [2][2] 1,2,3,4

; === External symbol definition ===
.extern EXT_SYMBOL

; === Instructions to ensure first pass succeeds ===
MAIN: mov r0, r1
      add r1, r2

; === Test entry on external symbol (should conflict) ===
.entry EXT_SYMBOL

; === End ===
END: stop