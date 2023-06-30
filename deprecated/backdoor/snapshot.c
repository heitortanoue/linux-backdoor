#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/fb.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>

#include "snapshot.h"
#include "socket.h"

struct socket *sock;

int coleta_buffer_tela(struct buffer_tela *bt)
{
    struct file *arquivo_tela;
    struct fb_info *info;

    void *buffer_tela;
    void *endereco;

    u8 *buffer_rgb;
    u32 cor, *pixel;
    long int tamanho_buffer, tamanho_buffer_rgb;

    // Abre o arquivo de tela
    arquivo_tela = filp_open("/dev/fb0", O_RDONLY, 0);
    if (IS_ERR(arquivo_tela))
    {
        printk(KERN_ERR "Erro ao abrir o arquivo /dev/fb0\n");
        return -1;
    }

    // Coleta as informações da tela
    info = (struct fb_info *)arquivo_tela->private_data;
    bt->xres = info->var.xres;
    bt->yres = info->var.yres;
    tamanho_buffer = info->screen_size;           // Tamanho do buffer
    tamanho_buffer_rgb = bt->xres * bt->yres * 3; // Tamanho do buffer RGB
    endereco = info->screen_base;                 // Endereço inicial do buffer

    // Aloca memória para o buffer
    buffer_tela = vmalloc(tamanho_buffer);
    if (!buffer_tela)
    {
        printk(KERN_ERR "Erro ao alocar memória para o buffer\n");
        return -1;
    }
    buffer_rgb = (u8 *)vmalloc(sizeof(u8) * tamanho_buffer_rgb);
    if (!buffer_rgb)
    {
        printk(KERN_ERR "Erro ao alocar memória para o buffer RGB\n");
        return -1;
    }

    // Realiza o print da tela
    memcpy(buffer_tela, endereco, tamanho_buffer);

    pixel = (u32 *)buffer_tela;
    for (int i = 0; i < tamanho_buffer_rgb;)
    {
        cor = *pixel++;
        buffer_rgb[i++] = (cor >> info->var.red.offset) &
                          ((1 << info->var.red.length) - 1); // RED
        buffer_rgb[i++] = (cor >> info->var.green.offset) &
                          ((1 << info->var.green.length) - 1); // GREEN
        buffer_rgb[i++] = (cor >> info->var.blue.offset) &
                          ((1 << info->var.blue.length) - 1); // BLUE
    }

    filp_close(arquivo_tela, NULL); // Fecha o arquivo de tela

    bt->rgb_buffer = buffer_rgb;
    vfree(buffer_tela);

    return 0;
}

int limpa_buffer_tela(struct buffer_tela *bt)
{
    vfree(bt->rgb_buffer);
    return 0;
}

int envia_buffer_tela(void)
{
    struct msghdr msg = {0};
    if (sock)
    {
        struct buffer_tela bt;

        struct kvec iov = {
            .iov_len = BUFFER_SIZE,
        };

        coleta_buffer_tela(&bt);

        ((int *)iov.iov_base)[0] = bt.xres;
        ((int *)iov.iov_base)[1] = bt.yres;

        // Enviando a resolução da tela
        if (kernel_sendmsg(sock, &msg, &iov, 1, sizeof(int) * 2) < 0)
        {
            printk(KERN_ERR "Erro ao enviar resolução tela\n");
            return -1;
        }

        // Enviando o buffer RGB
        for (int i = 0; i < 3 * bt.xres * bt.yres; i += BUFFER_SIZE)
        {
            memcpy(iov.iov_base, bt.rgb_buffer + i, BUFFER_SIZE);
            if (kernel_sendmsg(sock, &msg, &iov, 1, BUFFER_SIZE) < 0)
            {
                printk(KERN_ERR "Erro ao enviar buffer RGB\n");
                return -1;
            }
        }

        // Enviando padding
        memcpy(iov.iov_base, bt.rgb_buffer, BUFFER_SIZE - ((3 * bt.xres * bt.yres) % BUFFER_SIZE));
        if (kernel_sendmsg(sock, &msg, &iov, 1, BUFFER_SIZE - ((3 * bt.xres * bt.yres) % BUFFER_SIZE)) < 0)
        {
            printk(KERN_ERR "Erro ao enviar padding\n");
            return -1;
        }

        limpa_buffer_tela(&bt);
        kfree(iov.iov_base);
    }
    return 0;
}