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
  call void @run_IN(i16 21505, i32 2, i32 0, i32 0, i32 0)
  call void @run_IN(i16 21505, i32 3, i32 0, i32 0, i32 0)
  %0 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 0)
  %1 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 1)
  %2 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 2)
  %3 = add i32 %1, %2
  store i32 %3, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 0)
  call void @run_OUT(i16 20481, i32 1, i32 0, i32 0, i32 0)
  ret void
}

declare void @run_IN(i16, i32, i32, i32, i32)

declare void @run_OUT(i16, i32, i32, i32, i32)

