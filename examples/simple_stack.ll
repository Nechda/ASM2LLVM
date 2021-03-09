; ModuleID = 'Main_module'
source_filename = "simple_stack.bin"

@regTable = external global [13 x i32]
@memory = external global [256 x i8]

define void @main_func() {
entry:
  call void @func_0()
  ret void
}

define void @func_0() {
entry:
  %0 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %1 = sext i32 %0 to i64
  %2 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %1
  %3 = bitcast i8* %2 to i32*
  store i32 1095971635, i32* %3, align 4
  %4 = add i32 %0, 4
  store i32 %4, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %5 = sext i32 %4 to i64
  %6 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %5
  %7 = bitcast i8* %6 to i32*
  store i32 1056964608, i32* %7, align 4
  store i32 %4, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 1056964608, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %0, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 1095971635, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  store float 0x401A666660000000, float* bitcast ([13 x i32]* @regTable to float*), align 4
  call void @run_OUT(i16 5889, i32 1, i32 0)
  ret void
}

declare void @run_OUT(i16, i32, i32)

