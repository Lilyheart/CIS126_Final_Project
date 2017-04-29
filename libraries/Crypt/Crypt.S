.data

cipherTable:
        .byte 8, 3, 15, 11, 7, 0, 14, 5, 6, 9, 1, 10, 13, 4, 12, 2

.text

.global cryptEncodeNibble ;declaring cryptEncodeNibble as a function
.global cryptDecodeNibble ;declaring cryptDecodeNibble as a function

cryptEncodeNibble:
        cpi r24, 16                     ;comparing input with 16 (max)
        brge failedCrypt                ;if (input >= 16), goto failedCrypt
        ldi r30, lo8(cipherTable)       ;loading table address into z lo8
        ldi r31, hi8(cipherTable)       ;loading table address into z hi8
        add r30, r24                    ;adding the shift to the address
        ld r24, z                       ;return the new value
        ret

cryptDecodeNibble:
        cpi r24, 16                     ;comparing input with 16 (max)
        brge failedCrypt                ;if (input >= 16), goto failedCrypt
        ldi r30, lo8(cipherTable)       ;loading table address into z lo8
        ldi r31, hi8(cipherTable)       ;loading table address into z hi8
        ldi r16, 0                      ;start i = 0
        jmp decodeLoopStart             ;goto decodeLoopStart

decodeLoop:
        inc r16                         ;i++
        cpi r16, 16                     ;compare i with 16
        breq failedCrypt                ;if (i == 16), goto failedCrypt
        inc r30                         ;r30

decodeLoopStart:
        ld r17, z                       ;value of z is in r17
        cp r24, r17                     ;comparing r24 and z
        brne decodeLoop                 ;if (r24 != z), goto decodeLoop

        mov r24, r16                    ;if equal, return i
        ret

failedCrypt:
        ldi r24, 16
        ret