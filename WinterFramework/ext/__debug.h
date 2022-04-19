#pragma once

#include <stdio.h>

inline
void __debug_print_memory(void* address, size_t columns, size_t rows)
{
	for (size_t j = 0; j < rows; j++)
	{
		char* addr = (char*)address + columns * j;

		printf("\n 0x%p |", addr);
		for (size_t i = 0; i < columns; i++)
		{
			printf(" %02hhx", *(addr + i));
		}

		printf(" | ");
		for (size_t i = 0; i < columns; i++)
		{
			char c = *(addr + i);
			if (c > 31 && c < 127) printf("%c", c);
			else                   printf("_");
		}
	}
}