; Test boundary values for 10-bit range
MIN_VAL: .data -512          ; Minimum valid value
MAX_VAL: .data 511           ; Maximum valid value
ZERO:    .data 0             ; Zero value

START:   mov #-512, r1       ; Minimum immediate
         mov #511, r2        ; Maximum immediate
         mov #0, r3          ; Zero immediate
         prn #-512
         prn #511
         stop