#ifndef _EXPLORER_H
#define _EXPLORER_H


#define siz    30


char* explorer(void);
u8 open_dir(char *path);
void icon(u8 wh, u16 x, u16 y, u16 lx, u16 ly);
void check_ext(char *f, u8 il);

#endif
