#include <linux/module.h>
#include <linux/kmod.h>
#include <linux/kernel.h>

// char pointer arrays for environment and arguments
char *envp[] = {"HOME=/", "TERM=linux", "PATH=/sbin:/usr/sbin:/bin:/usr/bin", NULL};
char *argv[] = {"/bin/bash", "-c", "/media/sf_Shared/images/script.sh", NULL};

// module settings
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MURAT DEMIRTAS");
// initial callback
static int __init example_init(void)
{
    int ret = 0;
    printk("module loaded\n");
    ret = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);

    if (ret != 0)
        printk("error in call to usermodehelper: %i\n", ret);
    else
    {
        printk("ok\n");
        return 0;
    }

    return 0;
}

// exit callback
static void __exit example_exit(void)
{
    printk("module removed\n");
}

// set callback functions
module_init(example_init);
module_exit(example_exit);