#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	uint16_t full = 0xAFBC;
	uint8_t hi = (full >> 8) & 0xFF;
	uint8_t lo = full & 0xFF;
	printf("hi: %X lo : %X", hi, lo);
	return 0;
}
