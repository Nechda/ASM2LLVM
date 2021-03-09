mkdir build 
clang++ -c staticLib.cpp -o build/static.obj -I . -D STATIC_LIB_BUILD -w -m32
llc t.ll -filetype=obj -march=x86 -o build/t.obj
clang++ -g build/static.obj build/t.obj -o build/program.exe -m32