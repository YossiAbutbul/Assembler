; Test invalid string syntax
STR1: .string Hello              ; Missing quotes
STR2: .string "Unclosed string  ; Missing closing quote
STR3: .string "Non-ASCII: ñ"    ; Non-ASCII characters (if not supported)