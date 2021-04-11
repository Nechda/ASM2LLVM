; ModuleID = 'Main_module'
source_filename = "proga.bin"

@regTable = external global [41 x i32]
@memory = external global [128 x i8]

define void @main_func() {
entry:
  call void @func_0()
  ret void
}

define void @func_0() {
entry:
  br label %BB_0

BB_0:                                             ; preds = %entry
  %0 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 6)
  %1 = getelementptr [128 x i8], [128 x i8]* @memory, i32 0, i32 %0
  %2 = bitcast i8* %1 to i32*
  store i32 1095971635, i32* %2
  %3 = add i32 %0, 4
  store i32 %3, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 6)
  %4 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 6)
  %5 = getelementptr [128 x i8], [128 x i8]* @memory, i32 0, i32 %4
  %6 = bitcast i8* %5 to i32*
  store i32 1056964608, i32* %6
  %7 = add i32 %4, 4
  store i32 %7, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 6)
  %8 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 0)
  %9 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 6)
  %10 = sub i32 %9, 4
  store i32 %10, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 6)
  %11 = getelementptr [128 x i8], [128 x i8]* @memory, i32 0, i32 %10
  %12 = bitcast i8* %11 to i32*
  %13 = load i32, i32* %12
  store i32 %13, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 0)
  %14 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 1)
  %15 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 6)
  %16 = sub i32 %15, 4
  store i32 %16, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 6)
  %17 = getelementptr [128 x i8], [128 x i8]* @memory, i32 0, i32 %16
  %18 = bitcast i8* %17 to i32*
  %19 = load i32, i32* %18
  store i32 %19, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 1)
  %20 = load float, float* bitcast ([41 x i32]* @regTable to float*)
  %21 = load float, float* bitcast ([41 x i32]* @regTable to float*)
  %22 = load float, float* bitcast (i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 1) to float*)
  %23 = fmul float %21, %22
  store float %23, float* bitcast ([41 x i32]* @regTable to float*)
  call void @run_OUT(i16 20481, i32 1, i32 0, i32 0, i32 0)
  ret void
}

declare void @run_OUT(i16, i32, i32, i32, i32)

