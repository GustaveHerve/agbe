#include <stdlib.h>

int cpu_init()
{
	return 0;
}

/////////////////
//Instruction set
/////////////////

//nop
//x00	1 MCycle
int nop(uint16_t *pc)
{
	*pc++;
	return 1;
}

//ld BC,d16
//x01	3 MCycle
int ld_16bit(uint16_t *pc, int opcode)
{
	*pc++;
	int *rr;
	//TODO
	short nn;
	//nn = //TODO  //Get MSB of nn
	nn == nn << 8;
	//nn += //TODO
	pc += 3;
	return 3;
}

//ld (BC),A
//x02	2 MCycle
//
