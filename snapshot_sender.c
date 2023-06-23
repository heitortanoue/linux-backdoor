#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/net.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <linux/inet.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Seu nome");
MODULE_DESCRIPTION("Módulo de kernel para capturar e enviar print da tela via socket");

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080
#define BUFFER_SIZE   (SCREEN_WIDTH * SCREEN_HEIGHT * 4)
#define PORT 8888

static struct socket *sock = NULL;

static void captureScreen(unsigned char *buffer) {
    Display *display = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(display);
    XImage *image = XGetImage(display, root, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, AllPlanes, ZPixmap);

    memcpy(buffer, image->data, BUFFER_SIZE);

    XDestroyImage(image);
    XCloseDisplay(display);
}

static int send_via_socket(char *buffer, size_t length)
{
    struct sockaddr_in server;
    int ret;

    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Envio para localhost

    ret = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
    if (ret < 0)
    {
        printk(KERN_ERR "Erro ao criar o socket\n");
        return ret;
    }

    ret = kernel_connect(sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in), 0);
    if (ret < 0)
    {
        printk(KERN_ERR "Erro ao conectar ao servidor de destino\n");
        sock_release(sock);
        return ret;
    }

    ret = kernel_sendmsg(sock, &msg, 1, len);
    if (ret < 0)
    {
        printk(KERN_ERR "Erro ao enviar dados via socket\n");
    }

    kernel_sock_shutdown(sock, SHUT_RDWR);
    sock_release(sock);

    return ret;
}

static int __init screenshot_module_init(void)
{
    char buffer[BUFFER_SIZE];
    int ret;

    ret = capture_screen(buffer, sizeof(buffer));
    if (ret < 0)
    {
        printk(KERN_ERR "Erro ao capturar a tela\n");
        return ret;
    }

    ret = send_via_socket(buffer, strlen(buffer));
    if (ret < 0)
    {
        printk(KERN_ERR "Erro ao enviar via socket\n");
        return ret;
    }

    return 0;
}

static void __exit screenshot_module_exit(void)
{
    // Limpeza e liberação de recursos, se necessário
}

module_init(screenshot_module_init);
module_exit(screenshot_module_exit);
