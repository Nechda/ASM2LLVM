in eax
mov ebx, 1
lbl_loop:
mul ebx, eax
sub eax, 1
cmp eax, 1
je end
jmp lbl_loop
end:
out ebx
hlt