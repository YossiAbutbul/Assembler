; Test boundary values (-512 to +511)
START:  mov #-512, r1            ; Minimum valid value
        mov #511, r2             ; Maximum valid value
        mov #-513, r3            ; Should error (too small)
        mov #512, r4             ; Should error (too large)
        
; Test immediate value formats
        mov #+123, r1            ; Plus sign
        mov #0, r2               ; Zero
        mov #00123, r3           ; Leading zeros
        
; Test invalid immediate formats
        mov #, r1                ; Missing value
        mov # 123, r2            ; Space after #
        mov #abc, r3             ; Non-numeric
        mov ##123, r4            ; Double #