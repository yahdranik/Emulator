start:
    IN                   
    CALL fib          
    OUT
    HLT

fib:
    POP Ax               

    PUSHr Ax
    PUSH 0
    JE fib_ret_0

    PUSHr Ax
    PUSH 1
    JE fib_ret_1


    PUSHr Ax
    PUSHr Ax
    PUSH 1
    SUB                  
    CALL fib          

    POP Bx               
    POP Ax               
    PUSHr Bx            
    PUSHr Ax
    PUSH 2
    SUB                  
    CALL fib             

    ADD                  
    RET

fib_ret_0:
    PUSH 0
    RET

fib_ret_1:
    PUSH 1
    RET