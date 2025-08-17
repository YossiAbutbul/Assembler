; Test macro edge cases
mcro                             ; Error: missing macro name
mcroend

mcro valid_name
    mov r1, r2
    mcro nested                  ; Nested macro (should work in body)
mcroend

START:  stop