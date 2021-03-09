mkdir build
clang++ -c staticLib.cpp -o build/static.o -I . -D STATIC_LIB_BUILD -w
llc t.ll -filetype=obj -o build/t.o
clang++ -g build/static.o build/t.o -o build/prog.out
rm -f build/*.o
