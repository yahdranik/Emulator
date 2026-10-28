    PUSH 16
    POP Ax              ; центр (16,16)
    
    PUSH 10
    PUSH 10
    MUL
    POP Dx            ; Dx = r2
    
    PUSH 0
    POP Ix              ; Ix = y - счетчик строк
    PUSH 0
    POP Mx              ; Mx = y*32 + x - укащатель

y_loop:
    PUSH I
    PUSH 32
    JB   y_ok
    JUMP done

y_ok:
    PUSH Ix
    PUSH Ax
    SUB
    PUSH Ix
    PUSH Ax
    SUB
    MUL
    POP Kx              ; Kx = (y-cy)2
    
    PUSH 0
    POP Hx             ; Hx = x

x_loop:
    PUSH Hx
    PUSH 32
    JB   x_ok
    JUMP y++

x_ok:
    PUSH Hx
    PUSH Ax
    SUB
    PUSH Hx
    PUSH Ax
    SUB
    MUL                ; (x-cx)2
    PUSH Kx
    ADD                ; dx2 + dy2
    PUSH Dx            ; проверка на неравенство и заполнениеи
    JA x++
    
    PUSH 42
    POP [Mx]            ;'*'

x++:
    PUSH Hx
    PUSH 1
    ADD
    POP Hx              ; x++
    PUSH Mx
    PUSH 1
    ADD
    POP Mx              ; M++
    
    JUMP x_loop

y++:
    PUSH Ix
    PUSH 1
    ADD
    POP Ix           
    JUMP y_loop

done:
    DRAW
    HLT
