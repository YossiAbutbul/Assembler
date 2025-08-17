; Test various string cases
EMPTY:  .string ""               ; Empty string
SHORT:  .string "a"              ; Single character
LONG:   .string "This is a moderately long string to test parsing"
SPECIAL: .string "123!@#$%^&*()" ; Numbers and special characters

START:  lea EMPTY, r1
        lea SHORT, r2
        lea LONG, r3
        lea SPECIAL, r4
        stop