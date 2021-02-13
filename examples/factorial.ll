; ModuleID = 'Main_module'
source_filename = "factorial.bin"

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
  call void @run_IN(i64 2610731986672)
  %0 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @register, i32 0, i32 1)
  store i32 1, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @register, i32 0, i32 1)
  br label %BB_1

BB_1:                                             ; preds = %BB_2, %BB_0
  %1 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @register, i32 0, i32 1)
  %2 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @register, i32 0, i32 0)
  %3 = mul i32 %1, %2
  store i32 %3, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @register, i32 0, i32 1)
  %4 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @register, i32 0, i32 0)
  %5 = sub i32 %4, 1
  store i32 %5, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @register, i32 0, i32 0)
  call void @run_CMP(i64 2610731986736)
  %6 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @register, i32 0, i32 9)
  %7 = and i32 %6, 64
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %BB_3, label %BB_2

BB_2:                                             ; preds = %BB_1
  br label %BB_1

BB_3:                                             ; preds = %BB_1
  call void @run_OUT(i64 2610731986784)
  ret void
}

declare void @run_IN(i64)

declare void @run_CMP(i64)

declare void @run_OUT(i64)

