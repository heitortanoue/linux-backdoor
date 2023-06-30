#ifndef SNAPSHOT_H
#define SNAPSHOT_H

struct buffer_tela
{
    int xres;
    int yres;
    u8 *rgb_buffer;
};

int coleta_buffer_tela(struct buffer_tela *bt);
int limpa_buffer_tela(struct buffer_tela *bt);
int envia_buffer_tela(void);

#endif