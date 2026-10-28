IN
CALL fib
OUT
HLT

fib:
    POP AX

    PUSH AX
    PUSH 0
    JE RET_0

    PUSH AX
    PUSH 1
    JE RET_1


    PUSH AX
    PUSH AX
    PUSH 1
    SUB
    CALL fib

    POP BX
    POP AX
    PUSH BX
    PUSH AX
    PUSH 2
    SUB
    CALL fib

    ADD
    RET

ret_for_0:
    PUSH 0
    RET

ret_for_1:
    PUSH 1
    RET
