.text
    push 13.2
    push 0.5
    pop eax
    pop ebx
    fmul eax, eax, ebx
    out eax
    hlt