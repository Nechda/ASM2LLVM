; ModuleID = 'Main_module'
source_filename = "sum.bin"

@ptr_reg = external global i32*
@ptr_mem = external global i8*

define void @main_func() {
entry:
  call void @func_0()
  ret void
}

define void @func_0() {
entry:
  %registers_0 = load i32*, i32** @ptr_reg
  %memory_0 = load i8*, i8** @ptr_mem
  br label %BB_0

BB_0:                                             ; preds = %entry
  call void @run_IN(i64 23368576)
  call void @run_IN(i64 23368592)
  %0 = getelementptr i32, i32* %registers_0, i32 0
  %1 = load i32, i32* %0
  %2 = getelementptr i32, i32* %registers_0, i32 1
  %3 = load i32, i32* %2
  %4 = mul i32 %1, %3
  store i32 %4, i32* %0
  call void @run_OUT(i64 23368624)
  ret void
}

declare void @run_IN(i8*)

declare void @run_OUT(i8*)