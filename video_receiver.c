#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_PORT 8888
#define BUFFER_SIZE 4096

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    // Cria um socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha ao criar o socket");
        exit(EXIT_FAILURE);
    }

    // Define as opções do socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Falha ao definir as opções do socket");
        exit(EXIT_FAILURE);
    }

    // Configura o endereço do socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);

    // Associa o socket ao endereço e à porta
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Falha ao associar o socket ao endereço e à porta");
        exit(EXIT_FAILURE);
    }

    // Aguarda por conexões
    if (listen(server_fd, 1) < 0) {
        perror("Falha ao aguardar por conexões");
        exit(EXIT_FAILURE);
    }

    printf("Aguardando conexão...\n");

    // Aceita a conexão
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Falha ao aceitar a conexão");
        exit(EXIT_FAILURE);
    }

    printf("Conexão estabelecida\n");

    // Recebe a resolução da tela
    int xres, yres;
    if (recv(new_socket, &xres, sizeof(int), 0) <= 0) {
        perror("Falha ao receber a resolução da tela");
        exit(EXIT_FAILURE);
    }
    if (recv(new_socket, &yres, sizeof(int), 0) <= 0) {
        perror("Falha ao receber a resolução da tela");
        exit(EXIT_FAILURE);
    }

    // Recebe o buffer RGB
    int total_pixels = xres * yres;
    int total_bytes = total_pixels * 3;
    int received_bytes = 0;
    char *image_buffer = (char *)malloc(total_bytes);

    while (received_bytes < total_bytes) {
        int bytes_to_receive = total_bytes - received_bytes;
        int bytes_received = recv(new_socket, image_buffer + received_bytes, bytes_to_receive, 0);
        if (bytes_received <= 0) {
            perror("Falha ao receber o buffer RGB");
            exit(EXIT_FAILURE);
        }
        received_bytes += bytes_received;
    }

    // Cria o arquivo PPM e escreve os dados da imagem
    FILE *ppm_file = fopen("out.ppm", "wb");
    if (ppm_file == NULL) {
        perror("Falha ao criar o arquivo PPM");
        exit(EXIT_FAILURE);
    }

    fprintf(ppm_file, "P6\n");
    fprintf(ppm_file, "%d %d\n", xres, yres);
    fprintf(ppm_file, "255\n");
    fwrite(image_buffer, sizeof(char), total_bytes, ppm_file);

    fclose(ppm_file);

    printf("Imagem salva com sucesso\n");

    // Libera a memória alocada
    free(image_buffer);

    // Fecha o socket
    close(new_socket);
    close(server_fd);

    return 0;
}
