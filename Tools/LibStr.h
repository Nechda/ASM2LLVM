#pragma once


const int STANDART_ERROR_CODE = -1;

int readFullFile(const char* filename, char** outString);
int removeExtraChar(char** ptrStr, const char* dontDelChar);