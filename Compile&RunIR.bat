mkdir build 
clang++ -c staticLib.cpp -o build/static.obj -I . -D STATIC_LIB_BUILD -D ASM2LLVM_PROJECT -w -m32 -D STATIC_MEMORY_SIZE=128
llc t.ll -filetype=obj -march=x86 -o build/t.obj
clang++ -g build/static.obj build/t.obj -o build/program.exe -m32