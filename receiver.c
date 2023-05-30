#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    // Cria o socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erro ao criar o socket");
        exit(1);
    }

    // Configura as informações do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Liga o socket ao endereço local
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro ao realizar o bind");
        exit(1);
    }

    // Aguarda por conexões
    if (listen(sockfd, 1) < 0) {
        perror("Erro ao aguardar por conexões");
        exit(1);
    }

    printf("Aguardando por conexões...\n");

    // Aceita a conexão
    client_len = sizeof(client_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (newsockfd < 0) {
        perror("Erro ao aceitar a conexão");
        exit(1);
    }

    printf("Conexão estabelecida!\n");

    // Recebe e exibe os dados recebidos
    ssize_t bytes_received;
    while ((bytes_received = recv(newsockfd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        // Processa os dados recebidos aqui
        buffer[bytes_received] = '\0';
        printf("Dados recebidos: %s\n", buffer);
    }

    if (bytes_received < 0) {
        perror("Erro ao receber dados");
        exit(1);
    }

    // Fecha os sockets 
    close(newsockfd);
    close(sockfd);

    return 0;
}
