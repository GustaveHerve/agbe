#ifndef SAVE_H
#define SAVE_H

#include <stdio.h>

struct mbc_base;

FILE *open_save_file(struct mbc_base *mbc);

int save_ram_to_file(struct mbc_base *mbc);

#endif
