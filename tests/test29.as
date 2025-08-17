; Test various string formats
STR1:   .string ""                    ; Empty string
STR2:   .string "a"                   ; Single character
STR3:   .string "Quote: \" inside"    ; Quotes inside (if supported)
STR4:   .string "Multiple    spaces"  ; Multiple spaces
STR5:   .string "ñáéíóú"             ; Non-ASCII (should error)

; Test invalid string formats
STR6:   .string Missing quotes        ; Should error
STR7:   .string "Unclosed             ; Should error
STR8:   .string Too" many" quotes     ; Should error