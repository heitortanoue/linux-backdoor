#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define SERVER_PORT 12345

int main() {
    // Criando o socket do servidor
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Erro ao criar o socket");
        exit(1);
    }

    struct sockaddr_in serverAddr, clientAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Erro ao realizar o bind");
        exit(1);
    }

    if (listen(sockfd, 1) == -1) {
        perror("Erro ao aguardar conexões");
        exit(1);
    }

    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSockfd == -1) {
        perror("Erro ao aceitar a conexão do cliente");
        exit(1);
    }

    // Recebendo a imagem do capturador
    int screen_width, screen_height;
    if (recv(clientSockfd, &screen_width, sizeof(screen_width), 0) == -1 ||
        recv(clientSockfd, &screen_height, sizeof(screen_height), 0) == -1) {
        perror("Erro ao receber a largura e a altura da imagem");
        exit(1);
    }

    int image_size = screen_width * screen_height * 4;
    unsigned char* data = (unsigned char*)malloc(image_size);
    if (recv(clientSockfd, data, image_size, 0) == -1) {
        perror("Erro ao receber a imagem");
        exit(1);
    }

    // Exibindo a imagem recebida
    Display* display = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(display);

    XImage* image = XCreateImage(display, DefaultVisual(display, 0), DefaultDepth(display, 0),
                                 ZPixmap, 0, (char*)data, screen_width, screen_height, 32, 0);

    XPutImage(display, root, DefaultGC(display, 0), image, 0, 0, 0, 0, screen_width, screen_height);
    XFlush(display);

    // Aguardando um tempo para visualizar a imagem
    sleep(5);

    // Liberando recursos
    XDestroyImage(image);
    XCloseDisplay(display);
    close(clientSockfd);
    close(sockfd);

    return 0;
}