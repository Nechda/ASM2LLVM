.text
    mov eax, 4
    mov [var], 5
    mul [var], [var], eax
    out [var]
    hlt
.data
    @var dq 0 alloc 1