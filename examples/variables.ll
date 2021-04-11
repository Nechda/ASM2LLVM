; ModuleID = 'Main_module'
source_filename = "proga.bin"

@regTable = external global [41 x i32]
@memory = external global [128 x i8]

define void @main_func() {
entry:
  store i8 0, i8* getelementptr inbounds ([128 x i8], [128 x i8]* @memory, i32 0, i32 36)
  store i8 0, i8* getelementptr inbounds ([128 x i8], [128 x i8]* @memory, i32 0, i32 37)
  store i8 0, i8* getelementptr inbounds ([128 x i8], [128 x i8]* @memory, i32 0, i32 38)
  store i8 0, i8* getelementptr inbounds ([128 x i8], [128 x i8]* @memory, i32 0, i32 39)
  call void @func_0()
  ret void
}

define void @func_0() {
entry:
  br label %BB_0

BB_0:                                             ; preds = %entry
  %0 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 0)
  store i32 4, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 0)
  %1 = load i32, i32* bitcast (i8* getelementptr inbounds ([128 x i8], [128 x i8]* @memory, i32 0, i32 36) to i32*)
  store i32 5, i32* bitcast (i8* getelementptr inbounds ([128 x i8], [128 x i8]* @memory, i32 0, i32 36) to i32*)
  %2 = load i32, i32* bitcast (i8* getelementptr inbounds ([128 x i8], [128 x i8]* @memory, i32 0, i32 36) to i32*)
  %3 = load i32, i32* bitcast (i8* getelementptr inbounds ([128 x i8], [128 x i8]* @memory, i32 0, i32 36) to i32*)
  %4 = load i32, i32* getelementptr inbounds ([41 x i32], [41 x i32]* @regTable, i32 0, i32 0)
  %5 = mul i32 %3, %4
  store i32 %5, i32* bitcast (i8* getelementptr inbounds ([128 x i8], [128 x i8]* @memory, i32 0, i32 36) to i32*)
  call void @run_OUT(i16 20489, i32 36, i32 0, i32 0, i32 0)
  ret void
}

declare void @run_OUT(i16, i32, i32, i32, i32)

