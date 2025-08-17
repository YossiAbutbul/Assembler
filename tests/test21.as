; Test missing mcroend
mcro valid_macro                 ; Valid macro start
    mov r1, r2
    inc r3
; Missing mcroend here!

START:  mov #15, r1
        stop