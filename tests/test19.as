; Test reserved word as macro name
mcro mov                         ; Error: reserved word 'mov'
    inc r1
    dec r2
mcroend

START:  mov #10, r1
        stop