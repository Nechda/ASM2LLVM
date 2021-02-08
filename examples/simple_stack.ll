; ModuleID = 'Main_module'
source_filename = "simple_stack.bin"

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
  %0 = getelementptr i32, i32* %registers_0, i32 6
  %1 = load i32, i32* %0
  %2 = getelementptr i8, i8* %memory_0, i32 %1
  %3 = bitcast i8* %2 to i32*
  store i32 1095971635, i32* %3
  %4 = add i32 %1, 4
  store i32 %4, i32* %0
  %5 = getelementptr i32, i32* %registers_0, i32 6
  %6 = load i32, i32* %5
  %7 = getelementptr i8, i8* %memory_0, i32 %6
  %8 = bitcast i8* %7 to i32*
  store i32 1056964608, i32* %8
  %9 = add i32 %6, 4
  store i32 %9, i32* %5
  %10 = getelementptr i32, i32* %registers_0, i32 0
  %11 = load i32, i32* %10
  %12 = getelementptr i32, i32* %registers_0, i32 6
  %13 = load i32, i32* %12
  %14 = sub i32 %13, 4
  store i32 %14, i32* %12
  %15 = getelementptr i8, i8* %memory_0, i32 %14
  %16 = bitcast i8* %15 to i32*
  %17 = load i32, i32* %16
  store i32 %17, i32* %10
  %18 = getelementptr i32, i32* %registers_0, i32 1
  %19 = load i32, i32* %18
  %20 = getelementptr i32, i32* %registers_0, i32 6
  %21 = load i32, i32* %20
  %22 = sub i32 %21, 4
  store i32 %22, i32* %20
  %23 = getelementptr i8, i8* %memory_0, i32 %22
  %24 = bitcast i8* %23 to i32*
  %25 = load i32, i32* %24
  store i32 %25, i32* %18
  %26 = getelementptr i32, i32* %registers_0, i32 0
  %27 = bitcast i32* %26 to float*
  %28 = load float, float* %27
  %29 = getelementptr i32, i32* %registers_0, i32 1
  %30 = bitcast i32* %29 to float*
  %31 = load float, float* %30
  %32 = fmul float %28, %31
  store float %32, float* %27
  call void @run_OUT(i64 13133992)
  ret void
}

declare void @run_OUT(i8*)