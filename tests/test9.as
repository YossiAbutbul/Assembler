; Test various valid label formats
a:              mov #1, r1       ; Single character label
A123:           mov #2, r2       ; Mixed case with numbers
longlabelname: mov #3, r3      ; Long label with underscores
UPPERCASE:     mov #4, r4       ; All uppercase
lowercase:     mov #5, r5       ; All lowercase
mixedCase123: mov #6, r6       ; Mixed everything

        stop