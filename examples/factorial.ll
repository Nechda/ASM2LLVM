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
  br label %BB_4

BB_4:                                             ; preds = %BB_0
  call void @run_IN(i16 21505, i32 1, i32 0, i32 0, i32 0)
  %0 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 6)
  %1 = add i32 %0, 4
  store i32 %1, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 6)
  call void @func_1()
  br label %BB_5

BB_5:                                             ; preds = %BB_4
  ret void
}

define void @func_1() {
entry:
  br label %BB_1

BB_1:                                             ; preds = %entry
  %0 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 1)
  store i32 1, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 1)
  br label %BB_2

BB_2:                                             ; preds = %BB_2, %BB_1
  %1 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 1)
  %2 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 1)
  %3 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 0)
  %4 = mul i32 %2, %3
  store i32 %4, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 1)
  %5 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 0)
  %6 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 0)
  %7 = sub i32 %6, 1
  store i32 %7, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 0)
  %8 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 0)
  %9 = icmp ne i32 %8, 1
  br i1 %9, label %BB_2, label %BB_3

BB_3:                                             ; preds = %BB_2
  call void @run_OUT(i16 20481, i32 2, i32 0, i32 0, i32 0)
  %10 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 6)
  %11 = sub i32 %10, 4
  store i32 %11, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 6)
  ret void
}

declare void @run_OUT(i16, i32, i32, i32, i32)

declare void @run_IN(i16, i32, i32, i32, i32)

