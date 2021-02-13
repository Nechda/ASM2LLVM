; ModuleID = 'Main_module'
source_filename = "sum.bin"

@register = external global [13 x i32]
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
  call void @run_IN(i64 1766155364384)
  call void @run_IN(i64 1766155364400)
  %0 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @register, i32 0, i32 0)
  %1 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @register, i32 0, i32 1)
  %2 = add i32 %0, %1
  store i32 %2, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @register, i32 0, i32 0)
  call void @run_OUT(i64 1766155364432)
  ret void
}

declare void @run_IN(i64)

declare void @run_OUT(i64)

