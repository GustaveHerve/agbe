#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mbc.h"

FILE *open_save_file(struct mbc *mbc)
{
    size_t len = strlen(mbc->rom_path) + 5;
    char *save_path = malloc(len);
    snprintf(save_path, len, "%s.sav", mbc->rom_path);

    FILE *res = fopen(save_path, "wb+");
    free(save_path);

    size_t fsize = fseek(res, SEEK_END, 0);
    rewind(res);
    if (fsize <= mbc->ram_total_size)
        fread(mbc->ram, 1, fsize, res);
    rewind(res);
    return res;
}

int save_ram_to_file(struct mbc *mbc)
{
    rewind(mbc->save_file);
    int res = fwrite(mbc->ram, 1, mbc->ram_total_size, mbc->save_file);
    rewind(mbc->save_file);
    return res;
}
