; ModuleID = 'Main_module'
source_filename = "factorial.bin"

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
  call void @run_IN(i64 11233368)
  %0 = getelementptr i32, i32* %registers_0, i32 1
  %1 = load i32, i32* %0
  store i32 1, i32* %0
  br label %BB_1

BB_1:                                             ; preds = %BB_2, %BB_0
  %2 = getelementptr i32, i32* %registers_0, i32 1
  %3 = load i32, i32* %2
  %4 = getelementptr i32, i32* %registers_0, i32 0
  %5 = load i32, i32* %4
  %6 = mul i32 %3, %5
  store i32 %6, i32* %2
  %7 = getelementptr i32, i32* %registers_0, i32 0
  %8 = load i32, i32* %7
  %9 = sub i32 %8, 1
  store i32 %9, i32* %7
  call void @run_CMP(i64 11233432)
  %10 = getelementptr i32, i32* %registers_0, i32 9
  %11 = load i32, i32* %10
  %12 = and i32 %11, 64
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %BB_3, label %BB_2

BB_2:                                             ; preds = %BB_1
  br label %BB_1

BB_3:                                             ; preds = %BB_1
  call void @run_OUT(i64 11233480)
  ret void
}

declare void @run_IN(i8*)

declare void @run_CMP(i8*)

declare void @run_OUT(i8*)