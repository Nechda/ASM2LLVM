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
#define EPI_REG_INDEX 8
#define EFL_REG_INDEX 9
#define ECS_REG_INDEX 10
#define EDS_REG_INDEX 11
#define ESS_REG_INDEX 12

#define BYTES_IN_REGISTER sizeof(ui32)
#define COUNT_REGISTERS 13