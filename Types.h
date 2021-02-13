#pragma once
/*
\brief Упростим себе жизнь, введя короткие названия стандартных типов
\note  Для каждой реализации компилятора некоторые typedef'ы могут не
       соттветствовать ожидаемому размеру. При использовании данного 
       файла в проекте следует проверить размеры каждого typedef.
@{
*/
typedef unsigned char       ui8;
typedef unsigned short      ui16;
typedef unsigned int        ui32;
typedef unsigned long long  ui64;

typedef char        i8;
typedef short       i16;
typedef int         i32;
typedef long long   i64;

typedef i8* C_string;

/*
@}
*/