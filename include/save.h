#ifndef SAVE_H
#define SAVE_H

#include <stdio.h>

struct mbc;

FILE *open_save_file(struct mbc *mbc);

int save_ram_to_file(struct mbc *mbc);

#endif
