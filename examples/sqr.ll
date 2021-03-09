; ModuleID = 'Main_module'
source_filename = "sqr.bin"

@regTable = external global [13 x i32]
@memory = external global [256 x i8]

define void @main_func() {
entry:
  call void @func_0()
  ret void
}

define void @func_0() {
entry:
  %0 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %1 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %2 = sext i32 %1 to i64
  %3 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %2
  %4 = bitcast i8* %3 to i32*
  store i32 %0, i32* %4, align 4
  %5 = add i32 %1, 4
  store i32 %5, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %5, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 5), align 4
  %6 = sext i32 %5 to i64
  %7 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %6
  %8 = bitcast i8* %7 to i32*
  store i32 2781, i32* %8, align 4
  %9 = add i32 %1, 8
  store i32 %9, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %5, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %10 = add i32 %1, 12
  store i32 %10, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  call void @func_1()
  %11 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %12 = add i32 %11, -4
  store i32 %12, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %13 = sext i32 %12 to i64
  %14 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %13
  %15 = bitcast i8* %14 to i32*
  %16 = load i32, i32* %15, align 4
  store i32 %16, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  ret void
}

define void @func_1() {
entry:
  %0 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %1 = sext i32 %0 to i64
  %2 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %1
  %3 = bitcast i8* %2 to i32*
  %4 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %5 = add i32 %4, -4
  store i32 %5, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %6 = sext i32 %5 to i64
  %7 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %6
  %8 = bitcast i8* %7 to i32*
  %9 = load i32, i32* %8, align 4
  store i32 %9, i32* %3, align 4
  %10 = add i32 %0, 4
  store i32 %10, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %11 = add i32 %5, 12
  store i32 %11, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  call void @run_FIN(i16 6401, i32 1, i32 0)
  %12 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %13 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %14 = sext i32 %13 to i64
  %15 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %14
  %16 = bitcast i8* %15 to i32*
  store i32 %12, i32* %16, align 4
  %17 = add i32 %13, 4
  store i32 %17, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %18 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %19 = sext i32 %17 to i64
  %20 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %19
  %21 = bitcast i8* %20 to i32*
  store i32 %18, i32* %21, align 4
  %22 = add i32 %13, 8
  store i32 %22, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %18, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %17, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %18, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %23 = sext i32 %18 to i64
  %24 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %23
  %25 = bitcast i8* %24 to i32*
  store i32 %13, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %26 = load i32, i32* %16, align 4
  store i32 %26, i32* %25, align 4
  call void @run_FIN(i16 6401, i32 1, i32 0)
  %27 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %28 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %29 = sext i32 %28 to i64
  %30 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %29
  %31 = bitcast i8* %30 to i32*
  store i32 %27, i32* %31, align 4
  %32 = add i32 %28, 4
  store i32 %32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %33 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %34 = sext i32 %32 to i64
  %35 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %34
  %36 = bitcast i8* %35 to i32*
  store i32 %33, i32* %36, align 4
  %37 = add i32 %28, 8
  store i32 %37, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %38 = add i32 %33, 4
  store i32 %38, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %38, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %33, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %39 = sext i32 %38 to i64
  %40 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %39
  %41 = bitcast i8* %40 to i32*
  store i32 %28, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %42 = load i32, i32* %31, align 4
  store i32 %42, i32* %41, align 4
  call void @run_FIN(i16 6401, i32 1, i32 0)
  %43 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %44 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %45 = sext i32 %44 to i64
  %46 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %45
  %47 = bitcast i8* %46 to i32*
  store i32 %43, i32* %47, align 4
  %48 = add i32 %44, 4
  store i32 %48, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %49 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %50 = sext i32 %48 to i64
  %51 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %50
  %52 = bitcast i8* %51 to i32*
  store i32 %49, i32* %52, align 4
  %53 = add i32 %44, 8
  store i32 %53, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %54 = add i32 %49, 8
  store i32 %54, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %54, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %48, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %49, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %55 = sext i32 %54 to i64
  %56 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %55
  %57 = bitcast i8* %56 to i32*
  store i32 %44, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %58 = load i32, i32* %47, align 4
  store i32 %58, i32* %57, align 4
  store i32 %49, i32* %47, align 4
  store i32 %48, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %48, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 5), align 4
  store i32 2781, i32* %52, align 4
  store i32 %53, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %59 = sext i32 %53 to i64
  %60 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %59
  %61 = bitcast i8* %60 to i32*
  store i32 %49, i32* %61, align 4
  %62 = add i32 %44, 12
  store i32 %62, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %49, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %53, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %49, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %63 = sext i32 %49 to i64
  %64 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %63
  %65 = bitcast i8* %64 to i32*
  %66 = load i32, i32* %65, align 4
  store i32 %66, i32* %61, align 4
  %67 = add i32 %53, 4
  store i32 %67, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %68 = sext i32 %67 to i64
  %69 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %68
  %70 = bitcast i8* %69 to i32*
  store i32 %49, i32* %70, align 4
  %71 = add i32 %53, 8
  store i32 %71, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %72 = add i32 %49, 4
  store i32 %72, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %72, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %67, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %49, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %73 = sext i32 %72 to i64
  %74 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %73
  %75 = bitcast i8* %74 to i32*
  %76 = load i32, i32* %75, align 4
  store i32 %76, i32* %70, align 4
  %77 = add i32 %67, 4
  store i32 %77, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %78 = sext i32 %77 to i64
  %79 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %78
  %80 = bitcast i8* %79 to i32*
  store i32 %49, i32* %80, align 4
  %81 = add i32 %67, 8
  store i32 %81, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %54, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %54, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %77, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %49, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %82 = load i32, i32* %57, align 4
  store i32 %82, i32* %80, align 4
  %83 = add i32 %77, 4
  store i32 %83, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %48, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %84 = add i32 %77, 8
  store i32 %84, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  call void @func_2()
  %85 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %86 = add i32 %85, -4
  store i32 %86, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %87 = sext i32 %86 to i64
  %88 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %87
  %89 = bitcast i8* %88 to i32*
  %90 = load i32, i32* %89, align 4
  store i32 %90, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %91 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %91, i32* %89, align 4
  store i32 %86, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %91, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %92 = add i32 %90, -4
  store i32 %92, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  ret void
}

define void @func_2() {
entry:
  %0 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %1 = sext i32 %0 to i64
  %2 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %1
  %3 = bitcast i8* %2 to i32*
  %4 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %5 = add i32 %4, -4
  store i32 %5, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %6 = sext i32 %5 to i64
  %7 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %6
  %8 = bitcast i8* %7 to i32*
  %9 = load i32, i32* %8, align 4
  store i32 %9, i32* %3, align 4
  %10 = add i32 %0, 4
  store i32 %10, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %11 = add i32 %5, 8
  store i32 %11, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %12 = sext i32 %11 to i64
  %13 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %12
  %14 = bitcast i8* %13 to i32*
  store i32 %10, i32* %14, align 4
  %15 = add i32 %5, 12
  store i32 %15, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %15, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 5), align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %16
  %18 = bitcast i8* %17 to i32*
  store i32 2781, i32* %18, align 4
  %19 = add i32 %5, 16
  store i32 %19, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %20 = sext i32 %19 to i64
  %21 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %20
  %22 = bitcast i8* %21 to i32*
  store i32 %10, i32* %22, align 4
  %23 = add i32 %5, 20
  store i32 %23, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %24 = add i32 %10, 8
  store i32 %24, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %24, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %19, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %10, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %25 = sext i32 %24 to i64
  %26 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %25
  %27 = bitcast i8* %26 to i32*
  %28 = load i32, i32* %27, align 4
  store i32 %28, i32* %22, align 4
  %29 = add i32 %19, 4
  store i32 %29, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %15, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %30 = add i32 %19, 8
  store i32 %30, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  call void @func_3()
  %31 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %32 = add i32 %31, -4
  store i32 %32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %33 = sext i32 %32 to i64
  %34 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %33
  %35 = bitcast i8* %34 to i32*
  %36 = load i32, i32* %35, align 4
  store i32 %36, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %37 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %37, i32* %35, align 4
  store i32 %31, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %38 = sext i32 %31 to i64
  %39 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %38
  %40 = bitcast i8* %39 to i32*
  store i32 %36, i32* %40, align 4
  %41 = add i32 %32, 8
  store i32 %41, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %41, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 5), align 4
  %42 = sext i32 %41 to i64
  %43 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %42
  %44 = bitcast i8* %43 to i32*
  store i32 2781, i32* %44, align 4
  %45 = add i32 %32, 12
  store i32 %45, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %46 = sext i32 %45 to i64
  %47 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %46
  %48 = bitcast i8* %47 to i32*
  store i32 %36, i32* %48, align 4
  %49 = add i32 %32, 16
  store i32 %49, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %50 = add i32 %36, 4
  store i32 %50, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %50, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %45, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %36, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %51 = sext i32 %50 to i64
  %52 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %51
  %53 = bitcast i8* %52 to i32*
  %54 = load i32, i32* %53, align 4
  store i32 %54, i32* %48, align 4
  %55 = add i32 %45, 4
  store i32 %55, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %41, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %56 = add i32 %45, 8
  store i32 %56, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  call void @func_3()
  %57 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %58 = add i32 %57, -4
  store i32 %58, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %59 = sext i32 %58 to i64
  %60 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %59
  %61 = bitcast i8* %60 to i32*
  %62 = load i32, i32* %61, align 4
  store i32 %62, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %63 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %63, i32* %61, align 4
  store i32 %58, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %63, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %64 = add i32 %58, -4
  store i32 %64, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %65 = sext i32 %64 to i64
  %66 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %65
  %67 = bitcast i8* %66 to i32*
  %68 = load i32, i32* %67, align 4
  store i32 %68, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  %69 = icmp ne i32 %63, 0
  %70 = icmp ne i32 %68, 0
  %71 = and i1 %69, %70
  %72 = sext i1 %71 to i32
  store i32 %72, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %72, i32* %67, align 4
  store i32 %58, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %62, i32* %61, align 4
  store i32 %57, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %57, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 5), align 4
  %73 = sext i32 %57 to i64
  %74 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %73
  %75 = bitcast i8* %74 to i32*
  store i32 2781, i32* %75, align 4
  %76 = add i32 %64, 12
  store i32 %76, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %77 = sext i32 %76 to i64
  %78 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %77
  %79 = bitcast i8* %78 to i32*
  store i32 %62, i32* %79, align 4
  %80 = add i32 %64, 16
  store i32 %80, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %62, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %76, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %62, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %81 = sext i32 %62 to i64
  %82 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %81
  %83 = bitcast i8* %82 to i32*
  %84 = load i32, i32* %83, align 4
  store i32 %84, i32* %79, align 4
  %85 = add i32 %76, 4
  store i32 %85, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %57, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %86 = add i32 %76, 8
  store i32 %86, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  call void @func_3()
  %87 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %88 = add i32 %87, -4
  store i32 %88, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %89 = sext i32 %88 to i64
  %90 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %89
  %91 = bitcast i8* %90 to i32*
  %92 = load i32, i32* %91, align 4
  store i32 %92, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %93 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %93, i32* %91, align 4
  store i32 %88, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %93, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %94 = add i32 %88, -4
  store i32 %94, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %95 = sext i32 %94 to i64
  %96 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %95
  %97 = bitcast i8* %96 to i32*
  %98 = load i32, i32* %97, align 4
  store i32 %98, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  %99 = icmp ne i32 %93, 0
  %100 = icmp ne i32 %98, 0
  %101 = and i1 %99, %100
  %102 = sext i1 %101 to i32
  store i32 %102, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %102, i32* %97, align 4
  store i32 %94, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %102, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_FCOMP(i16 7430, i32 1, i32 0)
  %103 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 9), align 4
  %104 = and i32 %103, 64
  %105 = icmp eq i32 %104, 0
  br i1 %105, label %BB_17, label %BB_16

BB_16:                                            ; preds = %entry
  br i1 false, label %BB_17, label %BB_18

BB_17:                                            ; preds = %BB_16, %entry
  %106 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %107 = add i32 %106, -4
  store i32 %107, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %108 = sext i32 %107 to i64
  %109 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %108
  %110 = bitcast i8* %109 to i32*
  %111 = load i32, i32* %110, align 4
  store i32 %111, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %112 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %113 = add i32 %112, -4
  store i32 %113, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  ret void

BB_18:                                            ; preds = %BB_16
  %114 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %115 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %116 = sext i32 %115 to i64
  %117 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %116
  %118 = bitcast i8* %117 to i32*
  store i32 %114, i32* %118, align 4
  %119 = add i32 %115, 4
  store i32 %119, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %119, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 5), align 4
  %120 = sext i32 %119 to i64
  %121 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %120
  %122 = bitcast i8* %121 to i32*
  store i32 2781, i32* %122, align 4
  %123 = add i32 %115, 8
  store i32 %123, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %124 = sext i32 %123 to i64
  %125 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %124
  %126 = bitcast i8* %125 to i32*
  store i32 %114, i32* %126, align 4
  %127 = add i32 %115, 12
  store i32 %127, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %114, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %123, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %114, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %128 = sext i32 %114 to i64
  %129 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %128
  %130 = bitcast i8* %129 to i32*
  %131 = load i32, i32* %130, align 4
  store i32 %131, i32* %126, align 4
  %132 = add i32 %123, 4
  store i32 %132, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %119, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %133 = add i32 %123, 8
  store i32 %133, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  call void @func_3()
  %134 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %135 = add i32 %134, -4
  store i32 %135, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %136 = sext i32 %135 to i64
  %137 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %136
  %138 = bitcast i8* %137 to i32*
  %139 = load i32, i32* %138, align 4
  store i32 %139, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %140 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %140, i32* %138, align 4
  store i32 %135, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %140, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_FCOMP(i16 7430, i32 1, i32 0)
  %141 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 9), align 4
  %142 = and i32 %141, 64
  %143 = icmp eq i32 %142, 0
  br i1 %143, label %BB_21, label %BB_20

BB_20:                                            ; preds = %BB_18
  br i1 false, label %BB_21, label %BB_23

BB_21:                                            ; preds = %BB_20, %BB_18
  %144 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %145 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %146 = sext i32 %145 to i64
  %147 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %146
  %148 = bitcast i8* %147 to i32*
  store i32 %144, i32* %148, align 4
  %149 = add i32 %145, 4
  store i32 %149, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %149, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 5), align 4
  %150 = sext i32 %149 to i64
  %151 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %150
  %152 = bitcast i8* %151 to i32*
  store i32 2781, i32* %152, align 4
  %153 = add i32 %145, 8
  store i32 %153, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %154 = sext i32 %153 to i64
  %155 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %154
  %156 = bitcast i8* %155 to i32*
  store i32 %144, i32* %156, align 4
  %157 = add i32 %145, 12
  store i32 %157, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %158 = add i32 %144, 4
  store i32 %158, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %158, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %153, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %144, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %159 = sext i32 %158 to i64
  %160 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %159
  %161 = bitcast i8* %160 to i32*
  %162 = load i32, i32* %161, align 4
  store i32 %162, i32* %156, align 4
  %163 = add i32 %153, 4
  store i32 %163, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %164 = sext i32 %163 to i64
  %165 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %164
  %166 = bitcast i8* %165 to i32*
  store i32 %144, i32* %166, align 4
  %167 = add i32 %153, 8
  store i32 %167, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %168 = add i32 %144, 8
  store i32 %168, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %168, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %163, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %144, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %169 = sext i32 %168 to i64
  %170 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %169
  %171 = bitcast i8* %170 to i32*
  %172 = load i32, i32* %171, align 4
  store i32 %172, i32* %166, align 4
  %173 = add i32 %163, 4
  store i32 %173, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %149, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %174 = add i32 %163, 8
  store i32 %174, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  call void @func_4()
  %175 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %176 = add i32 %175, -4
  store i32 %176, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %177 = sext i32 %176 to i64
  %178 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %177
  %179 = bitcast i8* %178 to i32*
  %180 = load i32, i32* %179, align 4
  store i32 %180, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %181 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %181, i32* %179, align 4
  store i32 %176, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %181, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_OUT(i16 5889, i32 1, i32 0)
  %182 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %183 = add i32 %182, -4
  store i32 %183, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %184 = sext i32 %183 to i64
  %185 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %184
  %186 = bitcast i8* %185 to i32*
  %187 = load i32, i32* %186, align 4
  store i32 %187, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %188 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %189 = add i32 %188, -4
  store i32 %189, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  ret void

BB_23:                                            ; preds = %BB_20
  %190 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %191 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %192 = sext i32 %191 to i64
  %193 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %192
  %194 = bitcast i8* %193 to i32*
  store i32 %190, i32* %194, align 4
  %195 = add i32 %191, 4
  store i32 %195, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %195, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 5), align 4
  %196 = sext i32 %195 to i64
  %197 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %196
  %198 = bitcast i8* %197 to i32*
  store i32 2781, i32* %198, align 4
  %199 = add i32 %191, 8
  store i32 %199, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %200 = sext i32 %199 to i64
  %201 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %200
  %202 = bitcast i8* %201 to i32*
  store i32 %190, i32* %202, align 4
  %203 = add i32 %191, 12
  store i32 %203, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %204 = add i32 %190, 8
  store i32 %204, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %204, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %199, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %190, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %205 = sext i32 %204 to i64
  %206 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %205
  %207 = bitcast i8* %206 to i32*
  %208 = load i32, i32* %207, align 4
  store i32 %208, i32* %202, align 4
  %209 = add i32 %199, 4
  store i32 %209, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %195, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %210 = add i32 %199, 8
  store i32 %210, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  call void @func_3()
  %211 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %212 = add i32 %211, -4
  store i32 %212, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %213 = sext i32 %212 to i64
  %214 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %213
  %215 = bitcast i8* %214 to i32*
  %216 = load i32, i32* %215, align 4
  store i32 %216, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %217 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %217, i32* %215, align 4
  store i32 %212, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %217, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_FCOMP(i16 7430, i32 1, i32 0)
  %218 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 9), align 4
  %219 = and i32 %218, 64
  %220 = icmp eq i32 %219, 0
  br i1 %220, label %BB_26, label %BB_25

BB_25:                                            ; preds = %BB_23
  br i1 false, label %BB_26, label %BB_28

BB_26:                                            ; preds = %BB_25, %BB_23
  %221 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %222 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %223 = sext i32 %222 to i64
  %224 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %223
  %225 = bitcast i8* %224 to i32*
  store i32 %221, i32* %225, align 4
  %226 = add i32 %222, 4
  store i32 %226, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %226, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 5), align 4
  %227 = sext i32 %226 to i64
  %228 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %227
  %229 = bitcast i8* %228 to i32*
  store i32 2781, i32* %229, align 4
  %230 = add i32 %222, 8
  store i32 %230, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %231 = sext i32 %230 to i64
  %232 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %231
  %233 = bitcast i8* %232 to i32*
  store i32 %221, i32* %233, align 4
  %234 = add i32 %222, 12
  store i32 %234, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %221, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %230, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %221, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %235 = sext i32 %221 to i64
  %236 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %235
  %237 = bitcast i8* %236 to i32*
  %238 = load i32, i32* %237, align 4
  store i32 %238, i32* %233, align 4
  %239 = add i32 %230, 4
  store i32 %239, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %240 = sext i32 %239 to i64
  %241 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %240
  %242 = bitcast i8* %241 to i32*
  store i32 %221, i32* %242, align 4
  %243 = add i32 %230, 8
  store i32 %243, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %244 = add i32 %221, 4
  store i32 %244, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %244, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %239, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %221, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %245 = sext i32 %244 to i64
  %246 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %245
  %247 = bitcast i8* %246 to i32*
  %248 = load i32, i32* %247, align 4
  store i32 %248, i32* %242, align 4
  %249 = add i32 %239, 4
  store i32 %249, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %226, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %250 = add i32 %239, 8
  store i32 %250, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  call void @func_4()
  %251 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %252 = add i32 %251, -4
  store i32 %252, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %253 = sext i32 %252 to i64
  %254 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %253
  %255 = bitcast i8* %254 to i32*
  %256 = load i32, i32* %255, align 4
  store i32 %256, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %257 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %257, i32* %255, align 4
  store i32 %252, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %257, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_OUT(i16 5889, i32 1, i32 0)
  %258 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %259 = sext i32 %258 to i64
  %260 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %259
  %261 = bitcast i8* %260 to i32*
  store i32 0, i32* %261, align 4
  store i32 %258, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 0, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_OUT(i16 5889, i32 1, i32 0)
  %262 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %263 = add i32 %262, -4
  store i32 %263, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %264 = sext i32 %263 to i64
  %265 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %264
  %266 = bitcast i8* %265 to i32*
  %267 = load i32, i32* %266, align 4
  store i32 %267, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %268 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %269 = add i32 %268, -4
  store i32 %269, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  ret void

BB_28:                                            ; preds = %BB_25
  %270 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %271 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %272 = sext i32 %271 to i64
  %273 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %272
  %274 = bitcast i8* %273 to i32*
  store i32 %270, i32* %274, align 4
  %275 = add i32 %271, 4
  store i32 %275, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %276 = add i32 %270, 8
  store i32 %276, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %276, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %271, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %270, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %277 = sext i32 %276 to i64
  %278 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %277
  %279 = bitcast i8* %278 to i32*
  %280 = load i32, i32* %279, align 4
  store i32 %280, i32* %274, align 4
  store i32 %275, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %281 = sext i32 %275 to i64
  %282 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %281
  %283 = bitcast i8* %282 to i32*
  store i32 %270, i32* %283, align 4
  %284 = add i32 %271, 8
  store i32 %284, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %270, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %275, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %270, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %285 = sext i32 %270 to i64
  %286 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %285
  %287 = bitcast i8* %286 to i32*
  %288 = load i32, i32* %287, align 4
  store i32 %288, i32* %283, align 4
  store i32 %275, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %288, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %271, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %289 = load i32, i32* %274, align 4
  store i32 %289, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  %290 = load float, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %.cast = bitcast i32 %289 to float
  %291 = fmul float %290, %.cast
  store float %291, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %292 = bitcast i8* %273 to float*
  store float %291, float* %292, align 4
  store i32 %275, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 1082130432, i32* %283, align 4
  store i32 %275, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 1082130432, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %271, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %293 = load i32, i32* %274, align 4
  store i32 %293, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  %.cast2 = bitcast i32 %293 to float
  %294 = fmul float 4.000000e+00, %.cast2
  store float %294, float* bitcast ([13 x i32]* @regTable to float*), align 4
  store float %294, float* %292, align 4
  store i32 %275, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 1073741824, i32* %283, align 4
  store i32 %284, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %295 = sext i32 %284 to i64
  %296 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %295
  %297 = bitcast i8* %296 to i32*
  store i32 %270, i32* %297, align 4
  %298 = add i32 %271, 12
  store i32 %298, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %299 = add i32 %270, 4
  store i32 %299, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %299, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %284, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %270, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %300 = sext i32 %299 to i64
  %301 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %300
  %302 = bitcast i8* %301 to i32*
  %303 = load i32, i32* %302, align 4
  store i32 %303, i32* %297, align 4
  store i32 %284, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %303, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %304 = add i32 %284, -4
  store i32 %304, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %305 = sext i32 %304 to i64
  %306 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %305
  %307 = bitcast i8* %306 to i32*
  %308 = load i32, i32* %307, align 4
  store i32 %308, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  call void @run_FPOW(i16 9986, i32 1, i32 2)
  %309 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %310 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %311 = sext i32 %310 to i64
  %312 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %311
  %313 = bitcast i8* %312 to i32*
  store i32 %309, i32* %313, align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %309, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %314 = add i32 %310, -4
  store i32 %314, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %315 = sext i32 %314 to i64
  %316 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %315
  %317 = bitcast i8* %316 to i32*
  %318 = load i32, i32* %317, align 4
  store i32 %318, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  %319 = load float, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %.cast4 = bitcast i32 %318 to float
  %320 = fsub float %319, %.cast4
  store float %320, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %321 = bitcast i8* %316 to float*
  store float %320, float* %321, align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %322 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %322, i32* %313, align 4
  %323 = add i32 %314, 8
  store i32 %323, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %324 = add i32 %322, 12
  store i32 %324, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %324, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %322, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %325 = sext i32 %324 to i64
  %326 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %325
  %327 = bitcast i8* %326 to i32*
  store i32 %314, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %328 = load i32, i32* %317, align 4
  store i32 %328, i32* %327, align 4
  store i32 %322, i32* %317, align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %322, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %314, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %322, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %329 = sext i32 %322 to i64
  %330 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %329
  %331 = bitcast i8* %330 to i32*
  %332 = load i32, i32* %331, align 4
  store i32 %332, i32* %317, align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 1073741824, i32* %313, align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 1073741824, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %314, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %332, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  %.cast6 = bitcast i32 %332 to float
  %333 = fmul float 2.000000e+00, %.cast6
  store float %333, float* bitcast ([13 x i32]* @regTable to float*), align 4
  store float %333, float* %321, align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %322, i32* %313, align 4
  store i32 %323, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %334 = add i32 %322, 4
  store i32 %334, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %334, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %322, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %335 = sext i32 %334 to i64
  %336 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %335
  %337 = bitcast i8* %336 to i32*
  %338 = load i32, i32* %337, align 4
  store i32 %338, i32* %313, align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %338, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %.cast8 = bitcast i32 %338 to float
  %339 = fsub float -0.000000e+00, %.cast8
  store float %339, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %340 = bitcast i8* %312 to float*
  store float %339, float* %340, align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %341 = load i32, i32* %313, align 4
  store i32 %341, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %314, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %342 = load i32, i32* %317, align 4
  store i32 %342, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  %343 = load float, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %.cast10 = bitcast i32 %342 to float
  %344 = fdiv float %343, %.cast10
  store float %344, float* bitcast ([13 x i32]* @regTable to float*), align 4
  store float %344, float* %321, align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %322, i32* %313, align 4
  store i32 %323, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %345 = add i32 %322, 16
  store i32 %345, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %345, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %322, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %346 = sext i32 %345 to i64
  %347 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %346
  %348 = bitcast i8* %347 to i32*
  store i32 %314, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %349 = load i32, i32* %317, align 4
  store i32 %349, i32* %348, align 4
  store i32 0, i32* %317, align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %322, i32* %313, align 4
  %350 = add i32 %310, 4
  store i32 %350, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %324, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %324, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %322, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %351 = load i32, i32* %327, align 4
  store i32 %351, i32* %313, align 4
  store i32 %310, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %351, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %314, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %352 = load i32, i32* %317, align 4
  store i32 %352, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  call void @run_FCOMP(i16 7426, i32 1, i32 2)
  %353 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 9), align 4
  %354 = and i32 %353, 65
  %355 = icmp eq i32 %354, 0
  %356 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %357 = sext i32 %356 to i64
  %358 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %357
  %359 = bitcast i8* %358 to i32*
  br i1 %355, label %BB_30, label %BB_29

BB_29:                                            ; preds = %BB_28
  br label %BB_31

BB_30:                                            ; preds = %BB_28
  br label %BB_31

BB_31:                                            ; preds = %BB_30, %BB_29
  %storemerge20 = phi i32 [ 0, %BB_29 ], [ 1065353216, %BB_30 ]
  store i32 %storemerge20, i32* %359, align 4
  store i32 %356, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %storemerge20, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_FCOMP(i16 7430, i32 1, i32 0)
  %360 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 9), align 4
  %361 = and i32 %360, 64
  %362 = icmp eq i32 %361, 0
  br i1 %362, label %BB_33, label %BB_32

BB_32:                                            ; preds = %BB_31
  br i1 false, label %BB_33, label %BB_34

BB_33:                                            ; preds = %BB_32, %BB_31
  %363 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %364 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %365 = sext i32 %364 to i64
  %366 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %365
  %367 = bitcast i8* %366 to i32*
  store i32 %363, i32* %367, align 4
  %368 = add i32 %364, 4
  store i32 %368, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %369 = add i32 %363, 12
  store i32 %369, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %369, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %364, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %363, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %370 = sext i32 %369 to i64
  %371 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %370
  %372 = bitcast i8* %371 to i32*
  %373 = load i32, i32* %372, align 4
  store i32 %373, i32* %367, align 4
  store i32 %364, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %373, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_FSQRT(i16 8705, i32 1, i32 0)
  %374 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %375 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %376 = sext i32 %375 to i64
  %377 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %376
  %378 = bitcast i8* %377 to i32*
  store i32 %374, i32* %378, align 4
  %379 = add i32 %375, 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %380 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %381 = sext i32 %379 to i64
  %382 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %381
  %383 = bitcast i8* %382 to i32*
  store i32 %380, i32* %383, align 4
  %384 = add i32 %375, 8
  store i32 %384, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %385 = add i32 %380, 12
  store i32 %385, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %385, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %380, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %386 = sext i32 %385 to i64
  %387 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %386
  %388 = bitcast i8* %387 to i32*
  store i32 %375, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %389 = load i32, i32* %378, align 4
  store i32 %389, i32* %388, align 4
  store i32 %380, i32* %378, align 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %380, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %375, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %380, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %390 = sext i32 %380 to i64
  %391 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %390
  %392 = bitcast i8* %391 to i32*
  %393 = load i32, i32* %392, align 4
  store i32 %393, i32* %378, align 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 1073741824, i32* %383, align 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 1073741824, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %375, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %393, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  %.cast12 = bitcast i32 %393 to float
  %394 = fmul float 2.000000e+00, %.cast12
  store float %394, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %395 = bitcast i8* %377 to float*
  store float %394, float* %395, align 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %380, i32* %383, align 4
  store i32 %384, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %385, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %385, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %380, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %396 = load i32, i32* %388, align 4
  store i32 %396, i32* %383, align 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %396, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %375, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %397 = load i32, i32* %378, align 4
  store i32 %397, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  %398 = load float, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %.cast14 = bitcast i32 %397 to float
  %399 = fdiv float %398, %.cast14
  store float %399, float* bitcast ([13 x i32]* @regTable to float*), align 4
  store float %399, float* %395, align 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %380, i32* %383, align 4
  store i32 %384, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %385, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %385, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %380, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %375, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %400 = load i32, i32* %378, align 4
  store i32 %400, i32* %388, align 4
  store i32 %380, i32* %378, align 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %385, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %385, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %375, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %380, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %401 = load i32, i32* %388, align 4
  store i32 %401, i32* %378, align 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %380, i32* %383, align 4
  store i32 %384, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %402 = add i32 %380, 16
  store i32 %402, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %402, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %380, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %403 = sext i32 %402 to i64
  %404 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %403
  %405 = bitcast i8* %404 to i32*
  %406 = load i32, i32* %405, align 4
  store i32 %406, i32* %383, align 4
  store i32 %379, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %406, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %375, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %407 = load i32, i32* %378, align 4
  store i32 %407, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  %408 = load float, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %.cast16 = bitcast i32 %407 to float
  %409 = fadd float %408, %.cast16
  store float %409, float* bitcast ([13 x i32]* @regTable to float*), align 4
  store float %409, float* %395, align 4
  store i32 %375, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %410 = load i32, i32* %378, align 4
  store i32 %410, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_OUT(i16 5889, i32 1, i32 0)
  %411 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %412 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %413 = sext i32 %412 to i64
  %414 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %413
  %415 = bitcast i8* %414 to i32*
  store i32 %411, i32* %415, align 4
  %416 = add i32 %412, 4
  store i32 %416, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %417 = add i32 %411, 12
  store i32 %417, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %417, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %412, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %411, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %418 = sext i32 %417 to i64
  %419 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %418
  %420 = bitcast i8* %419 to i32*
  %421 = load i32, i32* %420, align 4
  store i32 %421, i32* %415, align 4
  store i32 %416, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %422 = sext i32 %416 to i64
  %423 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %422
  %424 = bitcast i8* %423 to i32*
  store i32 %411, i32* %424, align 4
  %425 = add i32 %412, 8
  store i32 %425, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %426 = add i32 %411, 16
  store i32 %426, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %426, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %416, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %411, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %427 = sext i32 %426 to i64
  %428 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %427
  %429 = bitcast i8* %428 to i32*
  %430 = load i32, i32* %429, align 4
  store i32 %430, i32* %424, align 4
  store i32 %416, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %430, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %412, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %431 = load i32, i32* %415, align 4
  store i32 %431, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  %432 = load float, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %.cast18 = bitcast i32 %431 to float
  %433 = fsub float %432, %.cast18
  store float %433, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %434 = bitcast i8* %414 to float*
  store float %433, float* %434, align 4
  store i32 %412, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %435 = load i32, i32* %415, align 4
  store i32 %435, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_OUT(i16 5889, i32 1, i32 0)
  %436 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %437 = add i32 %436, -4
  store i32 %437, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %438 = sext i32 %437 to i64
  %439 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %438
  %440 = bitcast i8* %439 to i32*
  %441 = load i32, i32* %440, align 4
  store i32 %441, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %442 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %443 = add i32 %442, -4
  store i32 %443, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  ret void

BB_34:                                            ; preds = %BB_32
  %444 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %445 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %446 = sext i32 %445 to i64
  %447 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %446
  %448 = bitcast i8* %447 to i32*
  store i32 %444, i32* %448, align 4
  %449 = add i32 %445, 4
  store i32 %449, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %449, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 5), align 4
  %450 = sext i32 %449 to i64
  %451 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %450
  %452 = bitcast i8* %451 to i32*
  store i32 2781, i32* %452, align 4
  %453 = add i32 %445, 8
  store i32 %453, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %454 = sext i32 %453 to i64
  %455 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %454
  %456 = bitcast i8* %455 to i32*
  store i32 %444, i32* %456, align 4
  %457 = add i32 %445, 12
  store i32 %457, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %458 = add i32 %444, 12
  store i32 %458, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %458, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %453, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %444, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %459 = sext i32 %458 to i64
  %460 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %459
  %461 = bitcast i8* %460 to i32*
  %462 = load i32, i32* %461, align 4
  store i32 %462, i32* %456, align 4
  %463 = add i32 %453, 4
  store i32 %463, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %449, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %464 = add i32 %453, 8
  store i32 %464, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  call void @func_3()
  %465 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %466 = add i32 %465, -4
  store i32 %466, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %467 = sext i32 %466 to i64
  %468 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %467
  %469 = bitcast i8* %468 to i32*
  %470 = load i32, i32* %469, align 4
  store i32 %470, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %471 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %471, i32* %469, align 4
  store i32 %466, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %471, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_FCOMP(i16 7430, i32 1, i32 0)
  %472 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 9), align 4
  %473 = and i32 %472, 64
  %474 = icmp eq i32 %473, 0
  br i1 %474, label %BB_37, label %BB_36

BB_36:                                            ; preds = %BB_34
  br i1 false, label %BB_37, label %BB_38

BB_37:                                            ; preds = %BB_36, %BB_34
  %475 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %476 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %477 = sext i32 %476 to i64
  %478 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %477
  %479 = bitcast i8* %478 to i32*
  store i32 %475, i32* %479, align 4
  %480 = add i32 %476, 4
  store i32 %480, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %481 = add i32 %475, 16
  store i32 %481, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %481, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %476, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %475, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %482 = sext i32 %481 to i64
  %483 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %482
  %484 = bitcast i8* %483 to i32*
  %485 = load i32, i32* %484, align 4
  store i32 %485, i32* %479, align 4
  store i32 %476, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %485, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_OUT(i16 5889, i32 1, i32 0)
  %486 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %487 = add i32 %486, -4
  store i32 %487, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %488 = sext i32 %487 to i64
  %489 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %488
  %490 = bitcast i8* %489 to i32*
  %491 = load i32, i32* %490, align 4
  store i32 %491, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %492 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %493 = add i32 %492, -4
  store i32 %493, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  ret void

BB_38:                                            ; preds = %BB_36
  %494 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %495 = add i32 %494, -4
  store i32 %495, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %496 = sext i32 %495 to i64
  %497 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %496
  %498 = bitcast i8* %497 to i32*
  %499 = load i32, i32* %498, align 4
  store i32 %499, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %500 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %501 = add i32 %500, -4
  store i32 %501, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  ret void
}

define void @func_3() {
entry:
  %0 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %1 = sext i32 %0 to i64
  %2 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %1
  %3 = bitcast i8* %2 to i32*
  %4 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %5 = add i32 %4, -4
  store i32 %5, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %6 = sext i32 %5 to i64
  %7 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %6
  %8 = bitcast i8* %7 to i32*
  %9 = load i32, i32* %8, align 4
  store i32 %9, i32* %3, align 4
  %10 = add i32 %0, 4
  store i32 %10, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 1008981770, i32* %8, align 4
  store i32 %4, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %11 = sext i32 %4 to i64
  %12 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %11
  %13 = bitcast i8* %12 to i32*
  store i32 %10, i32* %13, align 4
  %14 = add i32 %5, 8
  store i32 %14, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %10, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %4, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %10, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %15 = sext i32 %10 to i64
  %16 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %15
  %17 = bitcast i8* %16 to i32*
  %18 = load i32, i32* %17, align 4
  store i32 %18, i32* %13, align 4
  store i32 %4, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %18, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_FABS(i16 9729, i32 1, i32 0)
  %19 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %20 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %21 = sext i32 %20 to i64
  %22 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %21
  %23 = bitcast i8* %22 to i32*
  store i32 %19, i32* %23, align 4
  store i32 %20, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %19, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %24 = add i32 %20, -4
  store i32 %24, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %25 = sext i32 %24 to i64
  %26 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %25
  %27 = bitcast i8* %26 to i32*
  %28 = load i32, i32* %27, align 4
  store i32 %28, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  call void @run_FCOMP(i16 7426, i32 1, i32 2)
  %29 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 9), align 4
  %30 = and i32 %29, 1
  %31 = icmp eq i32 %30, 0
  %32 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %33 = sext i32 %32 to i64
  %34 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %33
  %35 = bitcast i8* %34 to i32*
  br i1 %31, label %BB_3, label %BB_4

BB_3:                                             ; preds = %entry
  br label %BB_5

BB_4:                                             ; preds = %entry
  br label %BB_5

BB_5:                                             ; preds = %BB_4, %BB_3
  %storemerge1 = phi i32 [ 1065353216, %BB_4 ], [ 0, %BB_3 ]
  store i32 %storemerge1, i32* %35, align 4
  store i32 %32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %storemerge1, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  call void @run_FCOMP(i16 7430, i32 1, i32 0)
  %36 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 9), align 4
  %37 = and i32 %36, 64
  %38 = icmp eq i32 %37, 0
  br i1 %38, label %BB_7, label %BB_6

BB_6:                                             ; preds = %BB_5
  br i1 false, label %BB_7, label %BB_8

BB_7:                                             ; preds = %BB_6, %BB_5
  %39 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %40 = sext i32 %39 to i64
  %41 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %40
  %42 = bitcast i8* %41 to i32*
  store i32 1065353216, i32* %42, align 4
  store i32 %39, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 1065353216, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %43 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %44 = add i32 %43, -4
  store i32 %44, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  ret void

BB_8:                                             ; preds = %BB_6
  %45 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %46 = sext i32 %45 to i64
  %47 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %46
  %48 = bitcast i8* %47 to i32*
  store i32 0, i32* %48, align 4
  store i32 %45, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 0, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %49 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %50 = add i32 %49, -4
  store i32 %50, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  ret void
}

define void @func_4() {
entry:
  %0 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %1 = sext i32 %0 to i64
  %2 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %1
  %3 = bitcast i8* %2 to i32*
  %4 = load i32, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %5 = add i32 %4, -4
  store i32 %5, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %6 = sext i32 %5 to i64
  %7 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %6
  %8 = bitcast i8* %7 to i32*
  %9 = load i32, i32* %8, align 4
  store i32 %9, i32* %3, align 4
  %10 = add i32 %0, 4
  store i32 %10, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %10, i32* %8, align 4
  store i32 %4, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %10, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %5, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %10, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %11 = sext i32 %10 to i64
  %12 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %11
  %13 = bitcast i8* %12 to i32*
  %14 = load i32, i32* %13, align 4
  store i32 %14, i32* %8, align 4
  store i32 %4, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %15 = sext i32 %4 to i64
  %16 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %15
  %17 = bitcast i8* %16 to i32*
  store i32 %10, i32* %17, align 4
  %18 = add i32 %5, 8
  store i32 %18, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %19 = add i32 %10, 4
  store i32 %19, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  store i32 %19, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 4), align 4
  store i32 %4, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %10, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 7), align 4
  %20 = sext i32 %19 to i64
  %21 = getelementptr [256 x i8], [256 x i8]* @memory, i64 0, i64 %20
  %22 = bitcast i8* %21 to i32*
  %23 = load i32, i32* %22, align 4
  store i32 %23, i32* %17, align 4
  store i32 %4, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  store i32 %23, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  %.cast = bitcast i32 %23 to float
  %24 = fsub float -0.000000e+00, %.cast
  store float %24, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %25 = bitcast i8* %16 to float*
  store float %24, float* %25, align 4
  store i32 %4, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %26 = load i32, i32* %17, align 4
  store i32 %26, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %5, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %27 = load i32, i32* %8, align 4
  store i32 %27, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 1), align 4
  %28 = load float, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %.cast2 = bitcast i32 %27 to float
  %29 = fdiv float %28, %.cast2
  store float %29, float* bitcast ([13 x i32]* @regTable to float*), align 4
  %30 = bitcast i8* %7 to float*
  store float %29, float* %30, align 4
  store i32 %5, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  %31 = load i32, i32* %8, align 4
  store i32 %31, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 0), align 4
  store i32 %0, i32* getelementptr inbounds ([13 x i32], [13 x i32]* @regTable, i64 0, i64 6), align 4
  ret void
}

declare void @run_FABS(i16, i32, i32)

declare void @run_FCOMP(i16, i32, i32)

declare void @run_FIN(i16, i32, i32)

declare void @run_OUT(i16, i32, i32)

declare void @run_FPOW(i16, i32, i32)

declare void @run_FSQRT(i16, i32, i32)

