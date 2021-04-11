#pragma once

/*
\brief номера битов в регистре EFLAGS
*/
#define FLAG_CF  0
#define FLAG_ZF  6
#define FLAG_SF  7

/*
\brief номера регистров
*/
#define EAX_REG_INDEX 0
#define EBX_REG_INDEX 1
#define ECX_REG_INDEX 2
#define EDX_REG_INDEX 3
#define ESI_REG_INDEX 4
#define EDI_REG_INDEX 5
#define ESP_REG_INDEX 6
#define EBP_REG_INDEX 7
#define LR0_REG_INDEX 9

#define BYTES_IN_REGISTER sizeof(ui32)
#define COUNT_REGISTERS 41