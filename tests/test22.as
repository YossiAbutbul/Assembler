; Test multiple macro syntax errors
mcro add extra text              ; Error: reserved word + extra text
    inc r1
mcroend

mcro clr                         ; Error: reserved word 'clr'
    dec r2
mcroend

mcro good_macro                  ; This would be valid
    mov r1, r2
; Missing mcroend - but should stop at first error

START:  stop