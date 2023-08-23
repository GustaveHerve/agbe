#ifndef INPUT_H
#define INPUT_H

struct cpu;

void handle_input_up(struct cpu *cpu, int bit);
void handle_input_down(struct cpu *cpu, int bit);

#endif
