#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x3674777c, "alloc_pages" },
	{ 0x97651e6c, "vmemmap_base" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0x3fba0e1b, "kernel_sendpage" },
	{ 0x92997ed8, "_printk" },
	{ 0x1810938a, "__free_pages" },
	{ 0xdb4c71a5, "sock_create" },
	{ 0x1b6314fd, "in_aton" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0x5eb9726b, "sock_release" },
	{ 0x96554810, "register_keyboard_notifier" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0x9ed554b3, "unregister_keyboard_notifier" },
	{ 0x541a6db8, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "6D6BAF31D0E1C9B222CF778");
