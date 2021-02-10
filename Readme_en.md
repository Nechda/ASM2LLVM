# ASM2LLVM
This directory contain source code for binary translator from [custom cpu architecture](https://github.com/Nechda/CPU-emulator) to LLVM IR.
## Requerements
This project was built with [llvm](https://github.com/llvm/llvm-project) (ver 10.0.0) and [Boost](https://www.boost.org/) (ver 1.72.0). The language standart is c++14.
## Command prompt
After building the project, for example in file "translator", you can use flags such as:
* **-i &lt;filename&gt;**  
Sets the input file, this flag is required.
* **-o &lt;filename&gt;**  
Sets the output file. If this flag is not specified, then IR code prints on the screen.
* **-l &lt;filename&gt;**  
Sets the logfile name. If this flag is not specified, then by default logfile is "log.log".
* **-r**  
Run the programm after translation.
* **--memorySize=&lt;amount&gt;**  
Sets the virual RAM size in bytes. The minimum value is 128 bytes, and the maximum is 512 kB.

## Examples
There are some basic programms in [examples directory](https://github.com/Nechda/ASM2LLVM/tree/main/examples). Lets consider some examples of running translator in different modes:

    >translator -i sqr.bin -r --memorySize=1024
Translator will read program from sqr.bin file, then run it with a memory limit in 1024 bytes. LLVM IR code will show on the screen.

    >translator -i factorial.bin -o factorial.ll -l my_log.txt
Translator will read program from factorial.bin, a result of translation will write in factorial.ll. If during execution appear any error, then information will write in my_log.txt file.

## Writing your own programs
If you want to get some binary files of your programs, then there are two ways:
* Build the asm compiler from the [CPU-emulator](https://github.com/Nechda/CPU-emulator) project.
* Build the compiler of a simple high-level programming language from the [Programming language](https://github.com/Nechda/Techno_track/tree/master/Programming%20language) project.

After you have built a compilator, you can use ASM2LLVM translator to generate LLVM IR code.
