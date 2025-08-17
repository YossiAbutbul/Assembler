; Test maximum label length (30 characters)
ABCDEFGHIJKLMNOPQRSTUVWXYZ1234:  mov #1, r1  ; 30 chars - valid
ABCDEFGHIJKLMNOPQRSTUVWXYZ12345: mov #1, r1  ; 31 chars - should error

; Test labels with underscores
VALID_LABEL_WITH_UNDERSCORES: mov #1, r1

; Test invalid label characters
INVALID-LABEL: mov #1, r1        ; Hyphen not allowed
INVALID@LABEL: mov #1, r1        ; @ not allowed

; Test label starting with number
123INVALID: mov #1, r1           ; Should error

; Test reserved words as labels
mov: mov #1, r1                  ; Should error
r1: mov #1, r1                   ; Should error