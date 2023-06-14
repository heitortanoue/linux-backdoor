#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define PORT 8888

void sendScreenData(int socket, const unsigned char *image_data, int width, int height)
{
    // Envia as informações da largura e altura da tela
    send(socket, &width, sizeof(width), 0);
    send(socket, &height, sizeof(height), 0);

    // Envia os dados da imagem
    send(socket, image_data, width * height * 4, 0);
}

int main()
{
    Display *display;
    Window root;
    XImage *image;
    XWindowAttributes window_attr;
    int screen_width, screen_height;

    // Conexão com o servidor X
    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        fprintf(stderr, "Erro ao conectar-se ao servidor X\n");
        exit(EXIT_FAILURE);
    }

    root = DefaultRootWindow(display);
    XGetWindowAttributes(display, root, &window_attr);
    screen_width = window_attr.width;
    screen_height = window_attr.height;

    // Criação do socket
    int socket_desc;
    struct sockaddr_in server;
    char *server_ip = "172.20.18.47"; // IP do servidor
    char *message;

    // Criação do socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        fprintf(stderr, "Erro ao criar o socket\n");
        exit(EXIT_FAILURE);
    }

    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Conexão com o servidor
    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        fprintf(stderr, "Erro ao conectar-se ao servidor\n");
        exit(EXIT_FAILURE);
    }

    // Loop para capturar e enviar continuamente as informações da tela
    while (1)
    {
        image = XGetImage(display, root, 0, 0, screen_width, screen_height, AllPlanes, ZPixmap);

        // Envia os dados da imagem para o receiver
        sendScreenData(socket_desc, image->data, screen_width, screen_height);

        XDestroyImage(image);

        usleep(100000); // Aguarda 100ms antes de capturar novamente a tela
    }

    // Fecha a conexão
    close(socket_desc);

    return 0;
}