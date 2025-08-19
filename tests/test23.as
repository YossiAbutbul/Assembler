; Test macro edge cases
mcro                             ; Error: missing macro name
mcroend

mcro valid_name
    mov r1, r2
mcroend

START:  stop