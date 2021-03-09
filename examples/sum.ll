; ModuleID = 'Main_module'
source_filename = "proga.bin"

@regTable = external global [13 x i32]
@memory = external global [256 x i8]

define void @main_func() {
entry:
  call void @func_0()
  ret void
}

define void @func_0() {
entry:
  call void @run_IN(i16 6145, i32 1, i32 0)
  call void @run_IN(i16 6145, i32 2, i32 0)
  %0 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %1 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  %2 = add i32 %0, %1
  store i32 %2, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_OUT(i16 5889, i32 1, i32 0)
  ret void
}

declare void @run_IN(i16, i32, i32)

declare void @run_OUT(i16, i32, i32)

